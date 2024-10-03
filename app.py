import os
import subprocess
from flask import Flask, request, jsonify, url_for, send_from_directory, render_template
import yt_dlp
import json
import logging
import re
from urllib.parse import urlparse
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

DEEPFILTERNET_PATH = config['deep_filter_path']
DOWNLOADS_DIR = config['downloads_dir']
FFMPEG_PATH = config['ffmpeg_path']
UPLOAD_FOLDER = config.get('upload_folder', 'uploads')  # defaults to uploads/

# Set env var for DeepFilterNet path
os.environ['DEEPFILTERNET_PATH'] = DEEPFILTERNET_PATH

# Set the uploads/ for serving media; mkdir if not found
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER

class Utils:
    """Utility class for common operations like file cleanup and sanitization."""
    @staticmethod
    def ensure_dir_exists(directory):
        if not os.path.exists(directory):
            os.makedirs(directory)

    @staticmethod
    def remove_files_by_base(base_filename):
        """Removes any existing files with the same base name."""
        base_path = os.path.join(app.config['UPLOAD_FOLDER'], base_filename)
        file_paths = [
            base_path + '.webm',
            base_path + '_isolated_audio.wav',
            base_path + '_processed_video.mp4'
        ]
        for path in file_paths:
            if os.path.exists(path):
                logging.info(f"Removing old file: {path}")
                os.remove(path)

    @staticmethod
    def sanitize_filename(filename):
        """Replace non-alphanumerics (except periods and underscores) with underscores."""
        return re.sub(r'[^a-zA-Z0-9._-]', '_', filename)
    
    @staticmethod
    def validate_url(url):
        """Basic URL validation"""
        parsed_url = urlparse(url)
        return all([parsed_url.scheme, parsed_url.netloc])

Utils.ensure_dir_exists(app.config['UPLOAD_FOLDER'])

class MediaHandler:
    """Class to handle video download and processing logic."""

    @staticmethod
    def download_media(url):
        try:
            # Extract media info first to sanitize title
            with yt_dlp.YoutubeDL() as ydl:
                info_dict = ydl.extract_info(url, download=False)
                base_title = info_dict['title']
                sanitized_title = Utils.sanitize_filename(base_title)

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

            # Return the sanitized file path for processing
            video_file = os.path.join(app.config['UPLOAD_FOLDER'], sanitized_title + "." + merged_ext)
            return os.path.abspath(video_file)

        except Exception as e:
            logging.error(f"Error downloading video: {e}")
            return None

    @staticmethod
    def process_with_media_processor(video_path):
        try:
            logging.info(f"Processing video at path: {video_path}")

            result = subprocess.run([
                './MediaProcessor/build/MediaProcessor', video_path
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

        if not Utils.validate_url(url):
            return jsonify({"status": "error", "message": "Invalid URL provided."})
        
        if url:
            # Download video via MediaHandler class
            video_path = MediaHandler.download_media(url)
            
            if not video_path:
                return jsonify({"status": "error", "message": "Failed to download video."})
            
            # Process video using MediaHandler class
            processed_video_path = MediaHandler.process_with_media_processor(video_path)
            
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
