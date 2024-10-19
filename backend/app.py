import json
import logging
import os
import typing
import flask
from pathlib import Path
from urllib.parse import urlparse
from utils import Utils
from media_handler import MediaHandler

from flask import Flask, jsonify, render_template, request, send_from_directory, url_for

"""
This is the backend of the Fast Music Remover tool.

Workflow:
1) Download YouTube video via yt-dlp
2) Send a processing request to the MediaProcessor C++ binary, which uses DeepFilterNet for fast filtering
3) Serve the processed video on the frontend

"""

app = Flask(__name__, template_folder="../templates")

BASE_DIR = Path(__file__).parent.parent.resolve()

# Construct the path to config.json
config_path = str((BASE_DIR / "config.json").resolve())


# Load the config file
with open(config_path) as config_file:
    config = json.load(config_file)

# Define base paths using absolute references

DOWNLOADS_PATH = str((BASE_DIR/config["downloads_path"]).resolve())
UPLOADS_PATH   = str((BASE_DIR / config.get("uploads_path", "uploads")).resolve()) # Defaults to uploads/
DEEPFILTERNET_PATH = str((BASE_DIR / config["deep_filter_path"]).resolve())
FFMPEG_PATH = str(Path(config["ffmpeg_path"]).resolve())


print(f"Config path: {config_path}\n Downlad path: {DOWNLOADS_PATH} \nUpload path: {UPLOADS_PATH}\n deepfile:{DEEPFILTERNET_PATH}")

os.environ["DEEPFILTERNET_PATH"] = DEEPFILTERNET_PATH
app.config["UPLOAD_FOLDER"] = UPLOADS_PATH

config["deep_filter_path"] = str(BASE_DIR / config["deep_filter_path"])
config["deep_filter_tarball_path"] = str(BASE_DIR / config["deep_filter_tarball_path"])
config["deep_filter_enoder_path"] = str(BASE_DIR / config["deep_filter_enoder_path"])
config["deep_filter_decoder_path"] = str(BASE_DIR / config["deep_filter_decoder_path"])
config["downloads_path"] = str(BASE_DIR / config["downloads_path"])
config["uploads_path"] = str(BASE_DIR / config["uploads_path"])

Utils.ensure_dir_exists(app.config["UPLOAD_FOLDER"])

@app.route("/", methods=["GET", "POST"])
def index()-> typing.Union[flask.Response, str]:
    if request.method == "POST":
        url = request.form["url"]

        if not Utils.validate_url(url):
            return jsonify({"status": "error", "message": "Invalid URL provided."})

        if url:
            video_path = MediaHandler.download_media(url,app.config["UPLOAD_FOLDER"])

            if not video_path:
                return jsonify({"status": "error", "message": "Failed to download video."})

            processed_video_path = MediaHandler.process_with_media_processor(video_path,BASE_DIR,config_path)


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