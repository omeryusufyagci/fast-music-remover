import json
import logging
import os
from pathlib import Path
from backend.utils import Utils
from backend.media_handler import MediaHandler

from typing import Union
from backend.response_handler import ResponseHandler

from flask import Flask, render_template, request, send_from_directory, url_for, Response

"""
This is the backend of the Fast Music Remover tool.

Workflow:
1) Download YouTube video via yt-dlp
2) Send a processing request to the MediaProcessor C++ binary, which uses DeepFilterNet for fast filtering
3) Serve the processed video on the frontend

"""

app = Flask(__name__, template_folder="templates")

BASE_DIR = Path(__file__).parent.resolve()
# Construct the path to config.json
config_path = str((BASE_DIR / "config.json").resolve())


# Load the config file
with open(config_path) as config_file:
    config = json.load(config_file)


# Define base paths using absolute references
DOWNLOADS_PATH = str((BASE_DIR/config["downloads_path"]).resolve())
UPLOADS_PATH   = str((BASE_DIR / config.get("uploads_path", "uploads")).resolve()) 
DEEPFILTERNET_PATH = str((BASE_DIR / config["deep_filter_path"]).resolve())
FFMPEG_PATH = str(Path(config["ffmpeg_path"]).resolve())
os.environ["DEEPFILTERNET_PATH"] = DEEPFILTERNET_PATH
app.config["UPLOAD_FOLDER"] = UPLOADS_PATH

#Log for dev reference
print(f"Base directory: {BASE_DIR}")
print(f"Deep filter config path: {config['deep_filter_path']}")
print(f"Resolved Deepfile path: {DEEPFILTERNET_PATH}")

print(f"Config path: {config_path}\nDownlad path: {DOWNLOADS_PATH} \nUpload path: {UPLOADS_PATH}\nDeepfile:{DEEPFILTERNET_PATH}")

Utils.ensure_dir_exists(app.config["UPLOAD_FOLDER"])

@app.route("/", methods=["GET", "POST"])
def index()-> Union[Response, str]:
    if request.method == "POST":
        url = request.form["url"]

        if not Utils.validate_url(url):
            return ResponseHandler.error("Invalid URL provided.", 400)

        if url:
            video_path = MediaHandler.download_media(url,UPLOADS_PATH)

            if not video_path:
                return ResponseHandler.error("Failed to download video.", 500)

            processed_video_path = MediaHandler.process_with_media_processor(video_path,BASE_DIR,config_path)
            #Since backend is in a different directory compared to config, am explicity passing the config_path

            if processed_video_path: 
                return ResponseHandler.success(
                "Video processed successfully.",
                {
                    "video_url": url_for("serve_video", filename=Path(processed_video_path).name)
                }
            )
            else:
                 return ResponseHandler.error("Failed to process video.", 500)

    return render_template("index.html")

@app.route("/video/<filename>")
def serve_video(filename: str) -> Response:
    try:
        # Construct the abs path for the file to be served (TODO: encapsulate)
        file_path = Path(app.config["UPLOAD_FOLDER"]) / filename
        abs_file_path = str(Path(file_path).resolve())
        logging.debug(f"Attempting to serve video from path: {abs_file_path}")

        if not Path(abs_file_path).exists():
            logging.error(f"File does not exist: {abs_file_path}")
            return ResponseHandler.error("File not found.", 404)

        # Serve the file from the uploads directory
        return send_from_directory(directory=app.config["UPLOAD_FOLDER"], path=filename, mimetype="video/mp4")
    
    except Exception as e:
        logging.error(f"Error serving video: {e}")
        return ResponseHandler.error("Failed to serve video.", 500)

if __name__ == "__main__":
    app.run(port=9090, debug=True)