import logging
import subprocess
import json
from pathlib import Path

import yt_dlp
from utils import Utils  
from typing import Optional, Union
from flask import Response
from response_handler import ResponseHandler

class MediaHandler:
    """Class to handle video download and processing logic."""

    @staticmethod
    def download_media(url: str, base_directory: str) -> Optional[str]:
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
    def process_with_media_processor(video_path: str, base_directory:Union[Response, str], config_path:  str) -> Optional[str]:
        """Run the C++ MediaProcessor binary with the video path"""

        try:
            logging.info(f"Processing video at path: {video_path}")

            input_data = {
                "video_file_path": video_path,
                "config_file_path": config_path
            }

            result = subprocess.run(
                [str(base_directory / "MediaProcessor" / "build" / "MediaProcessor")], 
                input= ResponseHandler.core_data_passer("The input data",input_data), 
                capture_output=True, 
                text=True
            )

            if result.returncode != 0:
                logging.error(f"Error processing video: {result.stderr}")
                return None
            print(f"Return code: {result.returncode}")
            print(f"stdout: {result.stdout}")
            print(f"stderr: {result.stderr}")
        
            
            # Parse the output to get the processed video path (TODO: encapsulate)
            try:
               
                response = json.loads(result.stdout)
                if response.get("status") == "success":
                    processed_video_path = response["data"]["processed_video_path"]
                    # Log the processed video path
                    logging.info(f"Processed video path returned: {processed_video_path}")
                    # Return success response
                    return  processed_video_path
                    
                else:
                    logging.error("Unexpected response from MediaProcessor")

            except json.JSONDecodeError:
                logging.error("Failed to parse JSON from MediaProcessor output")

            return None
        except Exception as e:
            logging.error(f"Error running C++ binary: {e}")
            return None
