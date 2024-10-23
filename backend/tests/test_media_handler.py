import tempfile
import unittest
from unittest.mock import patch, MagicMock
from pathlib import Path
from media_handler import MediaHandler
import json

class TestMediaHandler(unittest.TestCase):

    def setUp(self):
        """Set up base directory and mock paths for testing."""
        self.base_directory = tempfile.mkdtemp()
        self.video_url = "https://www.youtube.com/watch?v=dQw4w9WgXcQ"
        self.config_path = "/path/to/config.json"
        self.video_path = "/path/to/video.mp4"
        self.input_data = {
            "video_file_path": self.video_path,
            "config_file_path": self.config_path
        }

    @patch("media_handler.yt_dlp.YoutubeDL")
    def test_download_media(self, mock_yt_dlp):
        # Mock YoutubeDL instance
        mock_ydl_instance = MagicMock()
        
        # Mock extract_info for download=False and download=True
        mock_ydl_instance.extract_info.side_effect = [
            {"title": "Test Video", "ext": "mp4"},  # For download=False
            {"ext": "mp4"}  # For download=True
        ]
        
        # Set the return value when instantiating YoutubeDL
        mock_yt_dl_context_manager = MagicMock()
        mock_yt_dl_context_manager.__enter__.return_value = mock_ydl_instance
        mock_yt_dlp.return_value = mock_yt_dl_context_manager

        # Test download_media method
        result = MediaHandler.download_media(self.video_url, self.base_directory)
        expected_file = Path(self.base_directory) / "Test_Video.mp4"
        self.assertEqual(result, str(expected_file.resolve()))

    @patch("media_handler.subprocess.run")
    @patch("media_handler.ResponseHandler.core_data_passer")
    def test_process_with_media_processor_success(self, mock_core_data_passer, mock_subprocess_run):
        # Mock response from core_data_passer
        mock_core_data_passer.return_value = json.dumps(
        {   "status": "success",
            "message": "the input data",
            "data": self.input_data}
        )
        # Mock subprocess.run to simulate a successful response
        mock_subprocess_run.return_value = MagicMock(
            returncode=0,
            stdout=json.dumps({
                "status": "success",
                "data": {"processed_video_path": "/path/to/processed_video.mp4"}
            }),
            stderr=""
        )

        # Test process_with_media_processor
        result = MediaHandler.process_with_media_processor(
            self.video_path, self.base_directory, self.config_path
        )

        self.assertEqual(result, "/path/to/processed_video.mp4")





    @patch("media_handler.subprocess.run")
    def test_process_with_media_processor_failure(self, mock_subprocess_run):
        # Mock subprocess.run to simulate a failed response
        mock_subprocess_run.return_value = MagicMock(
            returncode=1,
            stdout="",
            stderr="Error processing video"
        )

        # Test process_with_media_processor with failure
        result = MediaHandler.process_with_media_processor(
            self.video_path, self.base_directory, self.config_path
        )

        self.assertIsNone(result)

    @patch("media_handler.subprocess.run")
    def test_process_with_media_processor_invalid_json(self, mock_subprocess_run):
        # Mock subprocess.run to simulate invalid JSON output
        mock_subprocess_run.return_value = MagicMock(
            returncode=0,
            stdout="Invalid JSON Output",
            stderr=""
        )

        # Test process_with_media_processor with invalid JSON
        result = MediaHandler.process_with_media_processor(
            self.video_path, self.base_directory, self.config_path
        )

        self.assertIsNone(result)

if __name__ == "__main__":
    unittest.main()
