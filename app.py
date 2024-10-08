from flask import Flask, jsonify, render_template, request, url_for
from pathlib import Path
from backend.utils import Utils
from backend.media_handler import MediaHandler
from backend.response_handler import ResponseHandler

app = Flask(__name__)
app.config["UPLOAD_FOLDER"] = Path(__file__).resolve().parent / "uploads"
Utils.ensure_dir_exists(app.config["UPLOAD_FOLDER"])

@app.route("/", methods=["GET", "POST"])
def index():
    if request.method == "POST":
        url = request.form["url"]
        if not Utils.validate_url(url):
            return ResponseHandler.error("Invalid URL provided.")

        video_path = MediaHandler.download_media(url, app.config["UPLOAD_FOLDER"])
        if not video_path:
            return ResponseHandler.error("Failed to download video.")

        processed_video_path = MediaHandler.process_with_media_processor(video_path)
        if processed_video_path:
            return ResponseHandler.success("Processing completed.", {
                "video_url": url_for("serve_video", filename=processed_video_path.name)
            })
        else:
            return ResponseHandler.error("Failed to process video.")
    return render_template("index.html")

@app.route("/video/<filename>")
def serve_video(filename):
    try:
        file_path = UPLOAD_FOLDER / filename
        if not file_path.exists():
            return ResponseHandler.error("File not found."), 404

        return send_from_directory(directory=str(UPLOAD_FOLDER), path=filename, mimetype="video/mp4")
    except Exception as e:
        logging.error(f"Error serving video: {e}")
        return ResponseHandler.error("Failed to serve video."), 500


if __name__ == "__main__":
    app.run(port=8080, debug=True)