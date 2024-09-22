# Fast Music Remover

`Fast Music Remover` aims to provide a lightweight tool to remove music and noise from any internet media. Currently it's in PoC stage, showing some promise for soft realtime use. 

The project currently consists of a simple HTML frontend, a barebone Flask backend and the `MediaProcessor`, a C++ binary that processes the media file. 

My initial attempt at this was a `demucs`-based python implementation which had a time cost that was greater than the source media length. This is due to the fact that `demucs` has capabilities that far exceed the requirements for this application, such as identifying individual instruments, which are thrown away here. You can find more about that project [here](https://github.com/omeryusufyagci/music-remover).

The current `DeepFilterNet` based C++ implementation, without any optimizations, runs at ~20% of the source media length (linearity not tested yet), already bringing huge performance gains. This is without any chunking and parallel processing, which will be the next steps moving forward.

The processed audio quality doesn't have a degradation that is audible, but I will eventually benchmark all relevant aspects, including audio quality. 

## Demo with Docker

If you have Docker installed, you can quickly test `Fast Music Remover` by running the following command:
```sh
sudo docker-compose up --build
```
> **Note**: If Docker doesn't require `sudo` on your system, you can omit it.


Once it's up, open `http://localhost:8080` in your browser, and you can test the service right away by submitting a YouTube URL.

This will spin up a containerized version of the app with everything set up: the Flask backend, the C++ MediaProcessor, and the frontend. Once processing is done, you'll see the processed video in the media player.

## How it works

1) The frontend expects a YouTube URL to be entered.
2) The Flask backend downloads the media with `yt-dlp`, and calls the `MediaProcessor` (C++ binary) with the downloaded file path.
3) The `MediaProcessor` extracts the audio with `FFmpeg`, applies the `DeepFilterNet` to remove music and noise, and generates 2 files: `<media_name>.processed_video.mp4` and `<media_name>.isolated_audio.wav`.
4) The backend then serves the processed media in the frontend player for playback. 

## Installation

### FFmpeg

FFmpeg is required as it's the primary tool used to extract and process audio from media files. 

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

Compile the C++ `MediaProcessor`:
```sh
cd MediaProcessor/build
cmake ..
make
```

## Usage

After setting up the dependencies and compiling the C++ project, go back to the project root and start the backend server:
```sh
python3 app.py 
```

This will start a development server at `http://127.0.0.1:8080`. Go to this address on your browser, enter a YouTube video URL, and follow the logs on your terminal. 

Once processing is completed, the processed version of the video will be available for playback. You can find the `isolated_audio` and `processed_media` files in the downloads directory under the project root.


## License

`Fast Music Remover` is released under the MIT license. Since it uses DeepFilterNet, which is dual-licensed under MIT and Apache License v2.0, the license can be found in the [LICENSE](LICENSE) file.
