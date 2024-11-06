import logging
import subprocess
import json
from pathlib import Path

import yt_dlp
from backend.utils import Utils  
from typing import Optional, Union
from backend.response_handler import ResponseHandler

class MediaHandler:
    """Class to handle video download and processing logic."""

    @staticmethod
    def download_media(url: str, base_directory: Union[Path, str]) -> Optional[str]:
        try:
            # Extract media info first to sanitize title
            with yt_dlp.YoutubeDL() as ydl:
                info_dict = ydl.extract_info(url, download=False)
                base_title = info_dict["title"]
                sanitized_title = Utils.sanitize_filename(base_title)

            ydl_opts = {
                "format": "bestvideo+bestaudio/best",
                "outtmpl": str(Path(base_directory)/f"{sanitized_title}.%(ext)s"),
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
            video_file = Path(base_directory)/f"{sanitized_title}.{merged_ext}"
            return str(video_file.resolve())

        except Exception as e:
            logging.error(f"Error downloading video: {e}")
            return None

    @staticmethod
    def process_with_media_processor(video_path: str, base_directory:Union[Path, str], config_path:  str) -> Optional[str]:
        """Run the C++ MediaProcessor binary with the video path"""
        try:
            logging.info(f"Processing video at path: {video_path}")
            base_directory = Path(base_directory)


            result = subprocess.run(
                [str(base_directory / "MediaProcessor/build/MediaProcessor"), str(video_path)], capture_output=True, text=True
            )

            if result.returncode != 0:
                logging.error(f"Error processing video: {result.stderr}")
                return None
            
            #This is to see the communication messages from the Core
            logging.error(result.stderr)

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
