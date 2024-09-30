# Fast Music Remover

`Fast Music Remover` aims to provide a lightweight and fast tool to remove music, sound effects, and noise from any internet media. Even at its early stages, it shows promise for near-realtime usage, with processing times around just 8% of the original media length —meaning it can process a 10-minute video in less than a minute! While only offline analysis is currently supported, the project has a clear aim to support live feeds as well. Please consider [contributing](CONTRIBUTING.md) if this interests you!

### Project Structure
* An **HTML frontend** for providing input media URL, and playback of the processed media
* A **Flask backend** for serving requests
* A **MediaProcessor** that chunks the media and processes it in parallel, utilizing [`DeepFilterNet`](https://github.com/Rikorose/DeepFilterNet).

### Background

This project builds on an initial attempt using a `demucs`-based Python implementation, which had a filtering time cost greater than the source media length. This is due to `demucs`'s advanced capabilities, such as identifying individual instruments, which are features beyond the scope of this project. That initial version can still be found [here](https://github.com/omeryusufyagci/music-remover).

### Use Cases

`Fast Music Remover` can serve a wide range of needs, such as:

* **Interview Editing**: Extract clear voice tracks for professional editing of interviews, especially field reports, removing ambient noise without distorting speech.
* **Education & Lectures**: Remove background noise and improve the overall audio quality of recorded lectures, making them clearer and easier for students to understand.
* **Streaming & Social Media**: Clean up your media before posting it on your preferred platform, avoiding copyright issues by removing background music or sound effects.
* **Home Recording Enthusiasts**: Enhance the quality of home-recorded content by removing unnecessary noise or isolating specific audio tracks for better post-production.
* **Regular Media Consumers**: Remove distractions from your favorite podcast or video, allowing you to focus on the content without the sound effects.

### Benchmarks

The processed audio quality shows no audible degradation so far. A formal benchmarking process for performance and quality is planned.

### Contributing
Contributions are always welcome! If you're interested in helping improve `Fast Music Remover`, there are several ways to get involved:

* **Report Bugs or Suggest Features**: Open an issue to report a bug or suggest new features you'd like to see.
* **Improve the Code**: Check the open issues and contribute with a pull request.
* **Enhance Documentation**: Documentation improvements are highly appreciated, whether it's the README, tutorials, or code comments.

Please see the [contributing](CONTRIBUTING.md) for more details on how to get started.

## Batteries Included!

If you have Docker installed, you can quickly try `Fast Music Remover` by running the following command:
```sh
docker-compose up --build
```
> **Note**: You may need `sudo` to run this command, depending on your system.


Once it's up, open `http://localhost:8080` in your browser, and you can test the service right away by submitting a YouTube URL.

This will spin up a containerized version of the app with everything set up: the Flask backend, the C++ MediaProcessor, and the frontend. Once processing is done, you'll be able to watch the filtered video in the media player.

### How it works

1. Enter the URL of the video you'd like to filter.
2. The Flask backend will download the media with `yt-dlp`, and call the `MediaProcessor` (C++ binary) with the downloaded file path.
3. The `MediaProcessor` will extract the audio with `FFmpeg`, apply the `DeepFilterNet` to remove music and noise, and generate two files: `<media_name>_processed_video.mp4` and `<media_name>_isolated_audio.wav`.
4. The backend will then serve the processed media in the frontend player for playback.

## Installation

### FFmpeg

FFmpeg is required as it's the primary tool used to extract, probe, and process audio from media files. 

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

### Usage

After setting up the dependencies and compiling the C++ project, go back to the project root and start the backend server:
```sh
python3 app.py 
```

This will start a development server at `http://127.0.0.1:8080`. Go to this address on your browser, enter a YouTube video URL, and follow the logs on your terminal. 

Once processing is completed, the processed version of the video will be available for playback. You can find the `isolated_audio` and `processed_media` files in the downloads directory under the project root.


## License

`Fast Music Remover` is released under the MIT license. The license can be found in the [LICENSE](LICENSE) file.
