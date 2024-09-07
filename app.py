import os
import subprocess
from flask import Flask, request, jsonify, url_for, send_from_directory, render_template
import yt_dlp
import logging

"""
This is the backend of the Fast Music Remover tool.

Current state: Initial Test/PoC

The aim is to rapidly filter out music (and noise) from internet media. 

What currently works:
1) Download any YouTube video via `yt-dlp`
2) Send a processing request to the `media_processor` C++ binary, which uses DeepFilterNet for fast filtering
3) Serve the processed video on the frontend

Where I want to go:
Perform soft-realtime processing on any media with a few seconds of initial delay.
The rest should be managed by sliding chunks, only offsetting the original video by the initial delay.

TODO: 
Remove the hardcoded abs paths
Add chunking and parallel processing support for the backend
Refactor app.py for clarity and encapsulate subcomponents
"""

app = Flask(__name__)
app.config['UPLOAD_FOLDER'] = 'downloads'

# set the DeepFilterNet path env variable
os.environ['DEEPFILTERNET_PATH'] = '/home/oyagci/Documents/ws_fast_music_remover/media_processor/res/deep-filter-0.5.6-x86_64-unknown-linux-musl'

if not os.path.exists(app.config['UPLOAD_FOLDER']):
    os.makedirs(app.config['UPLOAD_FOLDER'])

def cleanup_old_files(base_filename):
    """Removes any existing files with the same base name"""
    file_paths = [
        os.path.join(app.config['UPLOAD_FOLDER'], base_filename + '.webm'),
        os.path.join(app.config['UPLOAD_FOLDER'], base_filename + '_isolated_audio.wav'),
        os.path.join(app.config['UPLOAD_FOLDER'], base_filename + '_processed_video.mp4')
    ]
    for path in file_paths:
        if os.path.exists(path):
            logging.info(f"Removing old file: {path}")
            os.remove(path)

def download_youtube_video(url):
    ydl_opts = {
        'format': 'bestvideo+bestaudio/best',
        'outtmpl': os.path.join(app.config['UPLOAD_FOLDER'], '%(title)s.%(ext)s'),
        'noplaylist': True,
        'keepvideo': True  # Source files needed for merging back the media
    }

    try:
        with yt_dlp.YoutubeDL(ydl_opts) as ydl:
            info_dict = ydl.extract_info(url, download=True)

            base_title = info_dict['title']
            original_video_file = os.path.join(app.config['UPLOAD_FOLDER'], base_title + ".webm")

            # Don't allow spaces as it tends to cause many issues
            renamed_video_file = os.path.join(app.config['UPLOAD_FOLDER'], base_title.replace(' ', '_') + ".webm")

            logging.info(f"Checking if merged file exists: {original_video_file}")
            
            if os.path.exists(original_video_file):
                logging.info(f"Renaming merged video: {original_video_file} -> {renamed_video_file}")
                os.rename(original_video_file, renamed_video_file)
            else:
                logging.error(f"Error: Final merged video {original_video_file} not found!")

            return os.path.abspath(renamed_video_file)
    except Exception as e:
        logging.error(f"Error downloading video: {e}")
        return None


def process_video_with_cpp(video_path):
    try:
        logging.info(f"Processing video at path: {video_path}")

        result = subprocess.run([
            './media_processor/build/VideoSpeechProcessing', video_path
        ], capture_output=True, text=True)

        if result.returncode != 0:
            logging.error(f"Error processing video: {result.stderr}")
            return None

        for line in result.stdout.splitlines():
            if "Video processed successfully" in line:
                processed_video_path = line.split(": ")[1].strip()
                return processed_video_path

        return None
    except Exception as e:
        logging.error(f"Error running C++ binary: {e}")
        return None

@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'POST':
        url = request.form['url']
        if url:
            video_path = download_youtube_video(url)
            
            if not video_path:
                return jsonify({"status": "error", "message": "Failed to download video."})
            
            processed_video_path = process_video_with_cpp(video_path)
            
            if processed_video_path:
                return jsonify({
                    "status": "completed",
                    "video_url": url_for('serve_video', filename=os.path.basename(processed_video_path))
                })
            else:
                return jsonify({"status": "error", "message": "Failed to process video."})
    
    return render_template('index.html')

@app.route('/video/<filename>')
def serve_video(filename):
    return send_from_directory(app.config['UPLOAD_FOLDER'], filename)

if __name__ == '__main__':
    app.run(debug=True)
