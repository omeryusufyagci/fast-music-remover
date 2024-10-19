import json
import logging
import os
import re
import subprocess
import typing
import flask
from pathlib import Path
from urllib.parse import urlparse


import yt_dlp
from flask import Flask, jsonify, render_template, request, send_from_directory, url_for

"""
This is the backend of the Fast Music Remover tool.

Workflow:
1) Download YouTube video via `yt-dlp`
2) Send a processing request to the `MediaProcessor` C++ binary, which uses DeepFilterNet for fast filtering
3) Serve the processed video on the frontend

"""

app = Flask(__name__)

# Load config and set paths
with open("config.json") as config_file:
    config = json.load(config_file)

# Define base paths using absolute references


BASE_DIR = str(Path(__file__).parent.resolve())

DOWNLOADS_PATH = str(Path(config["downloads_path"]).resolve())
UPLOADS_PATH = str(Path(config.get("uploads_path", Path(BASE_DIR)/"uploads")).resolve())  # Defaults to uploads/

DEEPFILTERNET_PATH = str(Path(config["deep_filter_path"]).resolve())
FFMPEG_PATH = str(Path(config["ffmpeg_path"]).resolve())


os.environ["DEEPFILTERNET_PATH"] = DEEPFILTERNET_PATH
app.config["UPLOAD_FOLDER"] = UPLOADS_PATH


class Utils:
    """Utility class for common operations like file cleanup and sanitization."""

    @staticmethod
    def ensure_dir_exists(directory: str) -> None:
        if not Path(directory).exists():
            Path(directory).mkdir()

    @staticmethod
    def remove_files_by_base(base_filename: str) -> None:
        """Removes any existing files with the same base name."""
        base_path = Path(app.config["UPLOAD_FOLDER"])/ base_filename
        file_paths = [base_path + ".webm", base_path + "_isolated_audio.wav", base_path + "_processed_video.mp4"]
        for path in file_paths:
            if Path(path).exists():
                logging.info(f"Removing old file: {path}")
                Path(path).unlink()

    @staticmethod
    def sanitize_filename(filename: str) -> str:
        """Replace non-alphanumerics (except periods and underscores) with underscores."""
        return re.sub(r"[^a-zA-Z0-9._-]", "_", filename)

    @staticmethod
    def validate_url(url: str) -> bool:
        """Basic URL validation"""
        parsed_url = urlparse(url)
        return all([parsed_url.scheme, parsed_url.netloc])


Utils.ensure_dir_exists(app.config["UPLOAD_FOLDER"])


class MediaHandler:
    """Class to handle video download and processing logic."""

    @staticmethod
    def download_media(url: str) -> typing.Optional[str]:
        try:
            # Extract media info first to sanitize title
            with yt_dlp.YoutubeDL() as ydl:
                info_dict = ydl.extract_info(url, download=False)
                base_title = info_dict["title"]
                sanitized_title = Utils.sanitize_filename(base_title)

            ydl_opts = {
                "format": "bestvideo+bestaudio/best",
                "outtmpl": str(Path(app.config["UPLOAD_FOLDER"])/f"{sanitized_title}.%(ext)s"),
                "noplaylist": True,
                "keepvideo": True,
                "n_threads": 6,
            }

            with yt_dlp.YoutubeDL(ydl_opts) as ydl:
                result = ydl.extract_info(url, download=True)

                # Determine file extension
                if "requested_formats" in result:
                    merged_ext = result["ext"]
                else:
                    merged_ext = result.get("ext", "mp4")

            # Return the sanitized file path for processing
            video_file = Path(app.config["UPLOAD_FOLDER"])/f"{sanitized_title}.{merged_ext}"
            return str(Path(video_file).resolve())

        except Exception as e:
            logging.error(f"Error downloading video: {e}")
            return None

    @staticmethod
    def process_with_media_processor(video_path: str) -> typing.Optional[str]:
        """Run the C++ MediaProcessor binary with the video path"""

        try:
            logging.info(f"Processing video at path: {video_path}")

            result = subprocess.run(
                ["./MediaProcessor/build/MediaProcessor", str(video_path)], capture_output=True, text=True
            )

            if result.returncode != 0:
                logging.error(f"Error processing video: {result.stderr}")
                return None

            # Parse the output to get the processed video path (TODO: encapsulate)
            for line in result.stdout.splitlines():
                if "Video processed successfully" in line:
                    processed_video_path = line.split(": ", 1)[1].strip()

                    # Remove any surrounding quotes (TODO: encapsulate)
                    if processed_video_path.startswith('"') and processed_video_path.endswith('"'):
                        processed_video_path = processed_video_path[1:-1]
                    processed_video_path = str(Path(processed_video_path).resolve())
                    logging.info(f"Processed video path returned: {processed_video_path}")
                    return processed_video_path

            return None
        except Exception as e:
            logging.error(f"Error running C++ binary: {e}")
            return None


@app.route("/", methods=["GET", "POST"])
def index()-> typing.Union[flask.Response, str]:
    if request.method == "POST":
        url = request.form["url"]

        if not Utils.validate_url(url):
            return jsonify({"status": "error", "message": "Invalid URL provided."})

        if url:
            video_path = MediaHandler.download_media(url)

            if not video_path:
                return jsonify({"status": "error", "message": "Failed to download video."})

            processed_video_path = MediaHandler.process_with_media_processor(video_path)

            if processed_video_path:
                return jsonify(
                    {
                        "status": "completed",
                        "video_url": url_for("serve_video", filename=Path(processed_video_path).name),
                    }
                )
        
            else:
                return jsonify({"status": "error", "message": "Failed to process video."})

    return render_template("index.html")


@app.route("/video/<filename>")
def serve_video(filename: str) -> typing.Union[flask.Response, tuple[flask.Response, int]]:
    try:
        # Construct the abs path for the file to be served (TODO: encapsulate)
        file_path = Path(app.config["UPLOAD_FOLDER"]) / filename
        abs_file_path = str(Path(file_path).resolve())
        logging.debug(f"Attempting to serve video from path: {abs_file_path}")

        if not Path(abs_file_path).exists():
            logging.error(f"File does not exist: {abs_file_path}")
            return jsonify({"status": "error", "message": "File not found."}), 404

        # Serve the file from the uploads directory
        return send_from_directory(directory=app.config["UPLOAD_FOLDER"], path=filename, mimetype="video/mp4")
    except Exception as e:
        logging.error(f"Error serving video: {e}")
        return jsonify({"status": "error", "message": "Failed to serve video."}), 500


if __name__ == "__main__":
    app.run(host='0.0.0.0', port=9090, debug=True)
