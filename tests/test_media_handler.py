from backend.media_handler import MediaHandler
from pathlib import Path

def test_download_media(mocker):
    mocker.patch('yt_dlp.YoutubeDL.extract_info', return_value={"title": "Test Video", "ext": "mp4"})
    mocker.patch('yt_dlp.YoutubeDL.extract_info', return_value=None)
    download_path = MediaHandler.download_media("https://www.youtube.com", Path("/fake/uploads"))
    assert download_path is None  # Adjust based on test case
te