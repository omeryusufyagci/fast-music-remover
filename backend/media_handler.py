import subprocess
import logging
from pathlib import Path
import yt_dlp
from typing import Optional
from .utils import Utils

class MediaHandler:
    """Class to handle video download and processing logic."""

    @staticmethod
    def download_media(url: str, upload_folder: Path) -> Optional[Path]:
        try:
            # Extract media info first to sanitize title
            with yt_dlp.YoutubeDL() as ydl:
                info_dict = ydl.extract_info(url, download=False)
                base_title = info_dict["title"]
                sanitized_title = Utils.sanitize_filename(base_title)

            ydl_opts = {
                "format": "bestvideo+bestaudio/best",
                "outtmpl": str(upload_folder / f"{sanitized_title}.%(ext)s"),
                "noplaylist": True,
                "keepvideo": True,
                "n_threads": 6,
            }

            with yt_dlp.YoutubeDL(ydl_opts) as ydl:
                result = ydl.extract_info(url, download=True)
                merged_ext = result.get("ext", "mp4")

            # Return the sanitized file path for processing
            return upload_folder / f"{sanitized_title}.{merged_ext}"

        except Exception as e:
            logging.error(f"Error downloading video: {e}")
            return None

    @staticmethod
    def process_with_media_processor(video_path: Path) -> Optional[Path]:
        """Run the C++ MediaProcessor binary with the video path."""
        try:
            logging.info(f"Processing video at path: {video_path}")

            result = subprocess.run(
                ["./MediaProcessor/build/MediaProcessor", str(video_path)], capture_output=True, text=True
            )

            if result.returncode != 0:
                logging.error(f"Error processing video: {result.stderr}")
                return None

            for line in result.stdout.splitlines():
                if "Video processed successfully" in line:
                    processed_video_path = Path(line.split(": ", 1)[1].strip().strip('"'))
                    return processed_video_path

            return None
        except Exception as e:
            logging.error(f"Error running C++ binary: {e}")
            return None
