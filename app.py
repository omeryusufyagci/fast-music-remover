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
1) Accept a video URL or an uploaded video file from the user.
   - If a URL is provided, download the video via `yt-dlp` and sanitize the filename.
   - If a file is uploaded, save it directly to the configured upload directory after sanitizing the filename.

2) Send a processing request to the `MediaProcessor` C++ binary.
   - The `MediaProcessor` filters input file saves it in the same directory with a unique name.

3) Serve the processed video on the frontend.
   - A JSON response with the URL to the processed video is returned to the frontend, allowing the user to view or download the final output.
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


#Log for dev reference:
logging.info(f"Config path: {config_path}\nDownlad path: {DOWNLOADS_PATH} \nUpload path: {UPLOADS_PATH}\nDeepfile:{DEEPFILTERNET_PATH}")


Utils.ensure_dir_exists(app.config["UPLOAD_FOLDER"])
@app.route("/", methods=["GET", "POST"])
def index() -> Union[dict, str]:
    if request.method == "POST":
        url = request.form.get("url")
        file = request.files.get("file")

        # Ensure only one of URL or file is provided
        if not url and not file:
            return ResponseHandler.generate_error_response("Please provide a URL or upload a file.", 400)
        if url and file:
            return ResponseHandler.generate_error_response("Please provide only one input: either a URL or a file.", 400)

        # Handle URL case
        if url:
            if not Utils.validate_url(url):
                return ResponseHandler.generate_error_response("Invalid URL provided.", 400)

            video_path = MediaHandler.download_media(url, UPLOADS_PATH)
            if not video_path:
                return ResponseHandler.generate_error_response("Failed to download video: URL may be invalid or restricted.", 500)

        # Handle file upload case
        elif file:
            sanitized_filename = Utils.sanitize_filename(file.filename)
            video_path = str((Path(UPLOADS_PATH) / sanitized_filename).resolve())
            try:
                file.save(video_path)
            except Exception as e:
                logging.error(f"Error saving uploaded file: {e}")
                return ResponseHandler.generate_error_response("Failed to save uploaded file.", 500)

        # Process video
        processed_video_path = MediaHandler.process_with_media_processor(video_path, BASE_DIR)
        if not processed_video_path:
            return ResponseHandler.generate_error_response("Failed to process video.", 500)

        return ResponseHandler.generate_success_response(
            "Video processed successfully.",
            {
                "video_url": url_for("serve_video", filename=Path(processed_video_path).name)
            }
        )

    return render_template("index.html")



@app.route("/video/<filename>")
def serve_video(filename: str) -> Response:
    try:
        file_path = Path(app.config["UPLOAD_FOLDER"]) / filename
        abs_file_path = str(Path(file_path).resolve())
        logging.debug(f"Attempting to serve video from path: {abs_file_path}")

        if not Path(abs_file_path).exists():
            logging.generate_error_response(f"File does not exist: {abs_file_path}")
            return ResponseHandler.generate_error_response("File not found.", 404)


        return send_from_directory(directory=app.config["UPLOAD_FOLDER"], path=filename, mimetype="video/mp4")
    
    except Exception as e:
        logging.error(f"Error serving video: {e}")
        return ResponseHandler.generate_error_response("Failed to serve video.", 500)

if __name__ == "__main__":
    app.run(port=9090, debug=True)