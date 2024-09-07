# Fast Music Remover

Fast Music Remover aims to provide a lightweight tool to remove music and noise from any internet media. Currently it's in PoC stage, showing some promise for soft realtime use. 

The project currently consists of a simple html frontend, a barebone Flask backend and the `media_processor`, a C++ binary that processes the media file. 

My initial attempt at this was a `demucs` based python implementation which had a time cost that was greater than the source media length. The current `DeepFilterNet` based C++ implementation, without any optimizations, runs at ~20% of the source media length (linearity not tested yet), already bringing huge performance gains. This is without any chunking and parallel processing, which will be the next steps moving forward.

The processed audio quality doesn't have a degradation that is audible, but I will eventually benchmark all relevant aspects, including audio quality. 

## How it works

1) The frontend expects a YouTube URL to be passed
2) The Flask backend downloads the media with `yt-dlp`, and calls the `media_processor` (C++ binary) with the downloaded file path
3) The `media_processor` extracts the audio with `FFmpeg`, applies the `DeepFilterNet` to remove music and noise, and generates 2 files: `<media_name>.processed_video.mp4` and `<media_name>.isolated_audio.wav`.
4) The backend then serves the processed media in the frontend player for playback. 

## Installation

### FFmpeg

FFmepg is required as it's the primary tool used to extract and process audio from media files. 

**On Ubuntu/Debian:**
```sh
sudo apt update
sudo apt install ffmpeg
```

### Python Dependencies

Install the Python dependencies with:
```sh
pip install -r requirements.txt
```

### Media Processor

Compile the C++ `media_processor`:
```sh
cd media_processor/build
cmake ..
make
```

## Usage

After setting up the dependencies and compiling the C++ project, go back to the project root and start the backend server:
```sh
python3 app.py 
```

This will start a development server at `http://127.0.0.1:5000`. Go to this address on your browser, enter a YouTube video URL, and follow the logs on your terminal. 

Once procesing is completed, the processed version of the video will be available for playback. You can find the `isolated_audio` and `processed_media` files in the downloads directory under the project root.


## License

Fast Music Remover is released under the MIT license. Since it uses DeepFilterNet, which is dual-licensed under MIT and Apache License v2.0, the license can be found in the [LICENSE](LICENSE) file.
