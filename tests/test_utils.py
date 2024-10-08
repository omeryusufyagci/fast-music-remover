from backend.utils import Utils
from pathlib import Path

def test_sanitize_filename():
    assert Utils.sanitize_filename("test@file!.mp4") == "test_file_.mp4"

def test_validate_url():
    assert Utils.validate_url("https://www.youtube.com") is True
    assert Utils.validate_url("invalid_url") is False
