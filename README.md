<div align="center">

<img src="docs/images/logo.jpg" alt="Fast Music Remover Logo" width="160">

# Fast Music Remover

</div>

<div align="center">

  [![GitHub license](https://img.shields.io/github/license/omeryusufyagci/fast-music-remover)](https://github.com/omeryusufyagci/fast-music-remover/blob/main/LICENSE)
  [![GitHub issues](https://img.shields.io/github/issues/omeryusufyagci/fast-music-remover?color=yellow)](https://github.com/omeryusufyagci/fast-music-remover/issues)
  [![Tests](https://img.shields.io/github/actions/workflow/status/omeryusufyagci/fast-music-remover/build_test_and_format_core.yml?label=Linux&logo=linux&style=flat-square&color=success)](https://github.com/omeryusufyagci/fast-music-remover/actions/workflows/build_test_and_format_core.yml)
  [![Discord](https://img.shields.io/discord/1291805536911622265?label=&logo=discord&logoColor=white&color=7289DA&style=flat-square)](https://discord.gg/xje3PQTEYp)
  [![GitHub stars](https://img.shields.io/github/stars/omeryusufyagci/fast-music-remover?style=social)](https://github.com/omeryusufyagci/fast-music-remover/stargazers)
  [![GitHub forks](https://img.shields.io/github/forks/omeryusufyagci/fast-music-remover?style=social)](https://github.com/omeryusufyagci/fast-music-remover/network/members)

</div>

`Fast Music Remover` aims to provide a lightweight tool to remove music, sound effects and noise from internet media. Processing takes about 8% of the original source length -that is under 5 seconds for a minute long video to be ready for playback! 

Support for realtime processing is currently in the works, and there are many other features and improvements we're working on in parallel. If this sounds interesting to you, consider [contributing](CONTRIBUTING.md)!

The primary objective of the project is to enhance the audio tracks of typical mainstream media, such as lecture recordings, field reports and interviews. To see `Fast Music Remover` in action, take a look at our demo video below!

---

<div align="center">

https://github.com/user-attachments/assets/e46c161b-0178-4213-b468-245e9f829d5e

</div>

> The [original interview video](https://www.youtube.com/watch?v=aujFci9AuXE) is by Fisher College of Business, licensed under a [Creative Commons Attribution license (reuse allowed)](https://support.google.com/youtube/answer/2797468?hl=en).


## Background

This project started as an experiment with a `demucs`-based Python tool. While this was powerful in isolating individual audio components, the processing performance was limited. Although effective, `demucs` was a bit overqualified for the job; great for instrument separation but not ideal for the streamlined, low-latency filtering we’re aiming for here. If you're curious to see where it all began, you can still check out that initial version [here](https://github.com/omeryusufyagci/music-remover).

## Motivation and Use Cases

`Fast Music Remover` was built to tackle shared pain points and offer a completely free, open solution for enhancing audio in everyday media. Here’s where it shines:

* **Interview Editing**: Extract clear voice tracks for interviews and field reports, cutting through ambient noise without losing speech clarity.
* **Education & Lectures**: Strip away background noise to improve the sound quality of lecture recordings.
* **Streaming & Social Media**: Prepare your media for posting by removing background music or effects to avoid copyright issues.
* **Home Recording Enthusiasts**: Clean up home recordings, isolating specific tracks and eliminating unwanted noise for a polished final product.
* **Regular Media Consumers**: Enjoy your favorite podcast or video without the extra sound effects, keeping the focus on what matters.

## Roadmap

Our immediate priority is to provide a stable first release with cross-platform support for Linux, macOS, and Windows. We’re focused on getting this early version out as soon as possible, and your feedback will help shape the direction of the project.

Following the first release, we plan to introduce a separate, unstable release with some features in alpha stage, such as realtime support.

## Contributing

We have a wide array of interesting technical challenges spanning multiple domains. Take part and join us in building a free an open tool that addresses practical needs! 

Check out the [contributing guidelines](CONTRIBUTING.md) for details on how to get started.

## Benchmarks

The processed audio quality shows no audible degradation so far. A formal benchmarking process for performance and quality is planned. If you're interested in contributing to the benchmarking process, you can open an issue, reach out via the [discussions](https://github.com/omeryusufyagci/fast-music-remover/discussions) or [discord](https://discord.gg/xje3PQTEYp).

## Prerequisites

> **Quick Tip:** If you're just looking to test `Fast Music Remover`, you can skip these prerequisites and jump straight to the [Docker Quick Start](#option-1-quick-start-with-docker) below!

To get started with `Fast Music Remover`, ensure that you have the following software installed on your system. These dependencies are necessary for running the backend server, compiling the C++ processor, and handling media files.

- **Python 3.9+**: Required for running the backend server.
- **FFmpeg**: For extracting, probing, and processing audio files.
- **CMake**: Needed to compile the C++ `MediaProcessor`.
- **nlohmann-json**: A JSON library required for parsing configuration files in the `MediaProcessor`.
- **libsndfile**: Required for sampled audio file operations in the `MediaProcessor`.
- **Docker and Docker Compose** (optional but recommended for a quick setup):

<details>
  <summary>Click here for installation commands for Ubuntu/Debian and macOS</summary>

  ### Installation Commands

  **FFmpeg**:
  - **On Ubuntu/Debian**: 
    ```sh
    sudo apt update
    sudo apt install ffmpeg
    ```
  - **On macOS**:
    ```sh
    brew install ffmpeg
    ```

    After installing FFmpeg, ensure the correct path is set in the `config.json` file. By default, it is set to `/usr/bin/ffmpeg`. If you are using macOS and installed FFmpeg via Homebrew, update the path in `config.json` to:

    ```json
    "ffmpeg_path": "/opt/homebrew/bin/ffmpeg"
    ```

  **CMake**:
  - **On Ubuntu/Debian**: 
    ```sh
    sudo apt update
    sudo apt install cmake
    ```
  - **On macOS**:
    ```sh
    brew install cmake
    ```

  **nlohmann-json**:
  - **On Ubuntu/Debian**: 
    ```sh
    sudo apt update
    sudo apt install nlohmann-json3-dev
    ```
  - **On macOS**:
    ```sh
    brew install nlohmann-json
    ```

  **libsndfile**:
  - **On Ubuntu/Debian**: 
    ```sh
    sudo apt update
    sudo apt install libsndfile1-dev
    ```
  - **On macOS**:
    ```sh
    brew install libsndfile
    ```

  **Docker and Docker Compose**:
  - **On Ubuntu**:
    ```sh
    sudo apt install docker.io docker-compose
    ```
  - **On macOS**:
    ```sh
    brew install docker
    brew install docker-compose
    ```

</details>

> **Note**: Ensure all the above dependencies are installed before proceeding with the setup.

## Getting Started

To get started with `Fast Music Remover`, you have two main options: running it directly via the provided Dockerfile or installing all the necessary dependencies manually.

### Option 1: Quick Start with Docker

Ensure Docker is installed by running:
```sh
docker --version
```

If Docker is installed, you can quickly try `Fast Music Remover` by running the following command:
```sh
docker-compose up --build
```
> **Note**: You may need `sudo` to run this command, depending on how your system is setup.

Once it's up, open `http://localhost:8080` in your browser, and you can test the service right away by submitting a URL.

This Docker setup will spin up a containerized version of the tool with everything set up: the Flask backend, the C++ MediaProcessor, and the frontend. Once processing is done, a playback of the processed media will be available on the frontend.

### Option 2: Manual Installation

For those who want more control or are looking to contribute, follow these steps to set up `Fast Music Remover` manually.

#### Step 1: Ensure Dependencies Are Installed

Ensure all dependencies mentioned in the [Prerequisites](#prerequisites) section are installed before proceeding.

#### Step 2: Install Python Dependencies

Install the Python dependencies with:
```sh
pip install -r requirements.txt
```
#### Step 3: Compile the Media Processor (C++)

1. Navigate to the `MediaProcessor` directory:
```sh
cd MediaProcessor
```

2. Make a build directory and navigate into it:

```sh
mkdir build
cd build
```
3. Run CMake and compile
```sh
cmake ..
make
```
> **Note**: If you encounter errors here, double-check that all prerequisites are installed.

#### Step 4: Start the Backend Server

After setting up the dependencies and compiling the C++ project, go back to the project root and start the backend server:
```sh
python3 app.py 
```
> **Note**: The server should indicate it is running on http://127.0.0.1:8080. Visit this address in your browser and submit a URL to begin.

## License

`Fast Music Remover` is released under the MIT [license](LICENSE).