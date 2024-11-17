import yt_dlp

def compare_files(file_path1, file_path2):

    with open(file_path1, 'rb') as downloaded_file, open(file_path2, 'rb') as expected_file:
            downloaded_content = downloaded_file.read()
            expected_content = expected_file.read()
            assert downloaded_content == expected_content, "Downloaded video content does not match expected content"


def extract_info(url):
    """Extract metadata using yt_dlp."""
    ydl_opts = {"skip_download": True, "quiet": True}
    with yt_dlp.YoutubeDL(ydl_opts) as ydl:
        return ydl.extract_info(url, download=False)
    
DOWNLOAD_URL = "https://www.youtube.com/watch?v=TK4N5W22Gts"
DEEP_FILTER_PATH = "MediaProcessor/res/deep-filter-0.5.6-x86_64-unknown-linux-musl"
TEST_VIDEO_PATH = "Media/test.webm"
PROCESSED_VIDEO_PATH = "Media/test_processed.mp4"