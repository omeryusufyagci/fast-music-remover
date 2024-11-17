import logging
from concurrent.futures import ThreadPoolExecutor, TimeoutError as FuturesTimeoutError
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

            # Function to extract media info
            def extract_info():
                with yt_dlp.YoutubeDL() as ydl:
                    return ydl.extract_info(url, download=False)                

            # Use ThreadPoolExecutor to enforce timeout
            with ThreadPoolExecutor(max_workers=1) as executor:
                future = executor.submit(extract_info)
                try:
                    info_dict = future.result(timeout=10)  # Timeout set to 10 seconds
                except FuturesTimeoutError:
                    logging.error("Timeout occurred while extracting video info")
                    return None
                except Exception as e:
                    logging.error(f"Error extracting video info: {e}")
                    return None

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
    def process_with_media_processor(video_path: str, base_directory:Union[Path, str]) -> Optional[str]:
        """Run the C++ MediaProcessor binary with the video path"""
        try:
            logging.info(f"Processing video at path: {video_path}")
            base_directory = Path(base_directory)


            result = subprocess.run(
                [str(base_directory / "MediaProcessor/build/MediaProcessor"), str(video_path)], capture_output=True, text=True, cwd=str(base_directory)
            )

            if result.returncode != 0:
                logging.error(f"Error processing video: {result.stderr}")
                return None
            
            #This is to see the communication messages from the Core
            logging.error(result.stderr)

            processed_video_path = Utils.get_processed_video_path(result.stdout.splitlines())

            if processed_video_path:
                # Remove any surrounding quotes
                processed_video_path = Utils.remove_surrounding_quotes(processed_video_path)
                processed_video_path = str(Path(processed_video_path).resolve())
                logging.info(f"Processed video path returned: {processed_video_path}")
                return processed_video_path
            else:
                logging.error("Processed video path not found in the output.")
                return None

            return None
        except Exception as e:
            logging.error(f"Error running C++ binary: {e}")
            return None
