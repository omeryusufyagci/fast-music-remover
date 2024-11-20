import os
import tempfile
from pathlib import Path
import pytest
from backend.utils import Utils


@pytest.fixture
def test_directory():
    """Fixture to set up and tear down a temporary directory."""
    test_dir = tempfile.mkdtemp()
    yield test_dir
    for root, dirs, files in os.walk(test_dir, topdown=False):
        for name in files:
            os.remove(os.path.join(root, name))
        for name in dirs:
            os.rmdir(os.path.join(root, name))


def test_ensure_dir_exists(test_directory):
    # Test creating a new directory
    new_dir = Path(test_directory) / "new_folder"
    Utils.ensure_dir_exists(str(new_dir))
    assert new_dir.exists()


def test_remove_files_by_base(test_directory):
    upload_folder = test_directory
    base_filename = "test_file"
    
    # Create temporary files for testing
    file_paths = [
        Path(upload_folder) / f"{base_filename}.webm",
        Path(upload_folder) / f"{base_filename}_isolated_audio.wav",
        Path(upload_folder) / f"{base_filename}_processed_video.mp4"
    ]
    
    # Create the files
    for file_path in file_paths:
        file_path.touch()
    
    # Ensure the files were created
    for file_path in file_paths:
        assert file_path.exists()

    # Remove files using the method
    Utils.remove_files_by_base(base_filename, upload_folder)

    # Check if files are removed
    for file_path in file_paths:
        assert not file_path.exists()


def test_sanitize_filename():
    # Test filename sanitization
    assert Utils.sanitize_filename("test@file!.mp4") == "test_file_.mp4"
    assert Utils.sanitize_filename("valid_file-name.mp4") == "valid_file-name.mp4"


def test_validate_url():
    # Test valid URLs
    assert Utils.validate_url("https://www.youtube.com/watch?v=dQw4w9WgXcQ") is True

    # Test invalid URLs
    assert Utils.validate_url("invalid_url") is False
    assert Utils.validate_url("www.youtube.com") is False


def test_remove_surrounding_quotes():
    assert Utils.remove_surrounding_quotes("\"valid_file-name\"") == "valid_file-name"
    assert Utils.remove_surrounding_quotes("valid_file-name") == "valid_file-name"


def test_get_processed_video_path():
    # Test with a valid string that includes the correct success message
    valid_processed_string = Utils.get_processed_video_path(
        ["Video processed successfully: valid_file-name"]
    )
    assert valid_processed_string == "valid_file-name"

    # Test with an invalid string that does not contain the success message
    invalid_processed_string = Utils.get_processed_video_path(
        ["Video processed successful"]
    )
    assert invalid_processed_string is None
