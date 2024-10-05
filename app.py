import os
import subprocess
from flask import Flask, request, jsonify, url_for, send_from_directory, render_template
import yt_dlp
import json
import logging
import re
import platform

"""
This is the backend of the Fast Music Remover tool.

How it works:
1) Download any YouTube video via `yt-dlp`
2) Send a processing request to the `MediaProcessor` C++ binary, which uses DeepFilterNet for fast filtering
3) Serve the processed video on the frontend

"""

app = Flask(__name__)

# Load config and set paths
with open('config.json') as config_file:
    config = json.load(config_file)

DOWNLOADS_DIR = config['downloads_dir']
if platform.system() == "Windows":
    FFMPEG_PATH = config['ffmpeg_path_windows']  # for Windows
    DEEPFILTERNET_PATH = config['deep_filter_path_windows']
else:
    FFMPEG_PATH = config['ffmpeg_path_unix']  # for Linux or other OS
    DEEPFILTERNET_PATH = config['deep_filter_path_unix']


UPLOAD_FOLDER = config.get('upload_folder', 'uploads')  # defaults to uploads/

# Set env var for DeepFilterNet path
os.environ['DEEPFILTERNET_PATH'] = DEEPFILTERNET_PATH

# Set the uploads/ for serving media; mkdir if not found
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER
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

def sanitize_filename(filename):
    # Replace non-alphanumerics apart from periods and underscores, with underscores
    return re.sub(r'[^a-zA-Z0-9._-]', '_', filename)

def download_youtube_video(url):
    try:
        # Extract media info first to sanitize title
        with yt_dlp.YoutubeDL() as ydl:
            info_dict = ydl.extract_info(url, download=False)
            base_title = info_dict['title']
            sanitized_title = sanitize_filename(base_title)

        ydl_opts = {
            'format': 'bestvideo+bestaudio/best',
            'outtmpl': os.path.join(app.config['UPLOAD_FOLDER'], sanitized_title + '.%(ext)s'),
            'noplaylist': True,
            'keepvideo': True,  # Keep source files for merging
            'n_threads': 6
        }

        with yt_dlp.YoutubeDL(ydl_opts) as ydl:
            result = ydl.extract_info(url, download=True)

            # Post-download file-ext gymnastics
            if 'requested_formats' in result:

                # If separate video and audio were downloaded and merged 
                merged_ext = result['ext']
            else:
                # If a single file was downloaded; fallback to mp4 (which probably isn't the best idea)
                merged_ext = result.get('ext', 'mp4')

        # Return the sanitized file path with for processing
        video_file = os.path.join(app.config['UPLOAD_FOLDER'], sanitized_title + "." + merged_ext)
        return os.path.abspath(video_file)

    except Exception as e:
        logging.error(f"Error downloading video: {e}")
        return None

def process_video_with_cpp(video_path):
    try:
        if platform.system() == 'Windows':
            executable_path = os.path.join('MediaProcessor', 'build', 'MediaProcessor.exe')
        else:
            executable_path = os.path.join('MediaProcessor', 'build', 'MediaProcessor')
        result = subprocess.run([
            executable_path, video_path
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
    app.run(port=8080, debug=True)
