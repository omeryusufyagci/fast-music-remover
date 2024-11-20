import pytest
import os
from concurrent.futures import ThreadPoolExecutor, TimeoutError as FuturesTimeoutError
from pathlib import Path
from backend.media_handler import MediaHandler
import utils as utils


def test_extract_metadata_with_timeout():
    """Test metadata extraction with a timeout."""
    # Known test URL
    url = utils.DOWNLOAD_URL

    with ThreadPoolExecutor(max_workers=1) as executor:
        future = executor.submit(utils.extract_info, url)  
        try:
            metadata = future.result(timeout=10)  # Timeout set to 10 seconds
        except FuturesTimeoutError:
            pytest.fail("Timeout occurred while extracting video info")
        except Exception as e:
            pytest.fail(f"Error extracting video info: {e}")

    # Assert that metadata is retrieved
    assert metadata is not None, "Metadata extraction failed"
    assert "title" in metadata, "Title missing in metadata"
    assert "uploader" in metadata, "Uploader missing in metadata"
    assert "duration" in metadata, "Duration missing in metadata"
    assert metadata["duration"] > 0, "Invalid duration in metadata"



def test_process_with_media_processor():
    """Test video processing with MediaHandler."""
    BASE_DIR = Path(__file__).resolve().parents[2]
    TEST_DIR = Path(__file__).parent.resolve()
    DEEPFILTERNET_PATH = str((BASE_DIR / utils.DEEP_FILTER_PATH).resolve())
    os.environ["DEEPFILTERNET_PATH"] = DEEPFILTERNET_PATH

    test_video_path = str((TEST_DIR / utils.TEST_VIDEO_PATH).resolve())
    expected_processed_video_path = str((TEST_DIR / utils.PROCESSED_VIDEO_PATH).resolve())

    # Ensure the test video file exists
    assert Path(test_video_path).exists(), "Test video file does not exist"

    # Invoke the method
    processed_video_path = MediaHandler.process_with_media_processor(test_video_path, BASE_DIR)

    # Assert that the video was processed
    assert processed_video_path is not None, "process_with_media_processor returned None"
    assert Path(processed_video_path).exists(), "Processed video file does not exist"

    # Compare the processed video with the expected processed video
    utils.compare_files(processed_video_path, expected_processed_video_path)

    # Cleanup: Remove processed video file after the test
    if Path(processed_video_path).exists():
        os.remove(processed_video_path)
