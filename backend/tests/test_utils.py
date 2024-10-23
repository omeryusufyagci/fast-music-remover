import unittest
import os
import tempfile
from pathlib import Path
from utils import Utils

class TestUtils(unittest.TestCase):

    def setUp(self):
        """Set up temporary directories and files for testing."""
        self.test_dir = tempfile.mkdtemp()
        self.upload_folder = self.test_dir
        self.base_filename = "test_file"

    def tearDown(self):
        """Clean up the temporary directory."""
        for root, dirs, files in os.walk(self.test_dir, topdown=False):
            for name in files:
                os.remove(os.path.join(root, name))
            for name in dirs:
                os.rmdir(os.path.join(root, name))
    
    def test_ensure_dir_exists(self):
        # Test creating a new directory
        new_dir = Path(self.test_dir) / "new_folder"
        Utils.ensure_dir_exists(str(new_dir))
        self.assertTrue(new_dir.exists())

    def test_remove_files_by_base(self):
        # Create temporary files for testing
        file_paths = [
            Path(self.upload_folder) / (f"{self.base_filename}.webm"),
            Path(self.upload_folder) / (f"{self.base_filename}_isolated_audio.wav"),
            Path(self.upload_folder) / (f"{self.base_filename}_processed_video.mp4")
        ]
        # Create the files
        for file_path in file_paths:
            file_path.touch()

        # Ensure the files were created
        for file_path in file_paths:
            self.assertTrue(file_path.exists())

        # Remove files using the method
        Utils.remove_files_by_base(self.base_filename, self.upload_folder)

        # Check if files are removed
        for file_path in file_paths:
            self.assertFalse(file_path.exists())
            

    def test_sanitize_filename(self):
        # Test filename sanitization
        sanitized_filename = Utils.sanitize_filename("test@file!.mp4")
        self.assertEqual(sanitized_filename, "test_file_.mp4")

        sanitized_filename = Utils.sanitize_filename("valid_file-name.mp4")
        self.assertEqual(sanitized_filename, "valid_file-name.mp4")

    def test_validate_url(self):
        # Test valid URLs
        valid_url = "https://www.youtube.com/watch?v=dQw4w9WgXcQ"
        self.assertTrue(Utils.validate_url(valid_url))

        # Test invalid URLs
        invalid_url = "invalid_url"
        self.assertFalse(Utils.validate_url(invalid_url))

        no_scheme_url = "www.youtube.com"
        self.assertFalse(Utils.validate_url(no_scheme_url))

if __name__ == "__main__":
    unittest.main()
