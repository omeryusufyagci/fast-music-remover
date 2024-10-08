from pathlib import Path
import re
import logging
from urllib.parse import urlparse
from typing import Optional

class Utils:
    """Utility class for common operations like file cleanup and sanitization."""

    @staticmethod
    def ensure_dir_exists(directory: Path) -> None:
        """Ensure the directory exists, create it if not."""
        if not directory.exists():
            directory.mkdir(parents=True, exist_ok=True)

    @staticmethod
    def remove_files_by_base(base_filename: str, upload_folder: Path) -> None:
        """Removes files with the same base name."""
        base_path = upload_folder / base_filename
        file_paths = [
            base_path.with_suffix(".webm"),
            base_path.with_suffix("_isolated_audio.wav"),
            base_path.with_suffix("_processed_video.mp4")
        ]
        for path in file_paths:
            if path.exists():
                logging.info(f"Removing old file: {path}")
                path.unlink()

    @staticmethod
    def sanitize_filename(filename: str) -> str:
        """Sanitize the filename by replacing non-alphanumerics with underscores."""
        return re.sub(r"[^a-zA-Z0-9._-]", "_", filename)

    @staticmethod
    def validate_url(url: str) -> bool:
        """Basic URL validation."""
        parsed_url = urlparse(url)
        return all([parsed_url.scheme, parsed_url.netloc])
