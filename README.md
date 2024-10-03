# Fast Music Remover

`Fast Music Remover` aims to provide a lightweight and fast tool to remove music, sound effects, and noise from any internet media. Even at its early stages, it shows promise for near-realtime usage, with processing times around just 8% of the original media length —meaning it can process a 10-minute video in less than a minute! While only offline analysis is currently supported, the project has a clear aim to support live feeds as well. Please consider [contributing](CONTRIBUTING.md) if this interests you!

## Background

This project builds on an initial attempt using a `demucs`-based Python implementation, which had a filtering time cost greater than the source media length. This is due to `demucs`'s advanced capabilities, such as identifying individual instruments, which are features beyond the scope of this project. That initial version can still be found [here](https://github.com/omeryusufyagci/music-remover).

## Use Cases

`Fast Music Remover` can serve a wide range of needs, such as:

* **Interview Editing**: Extract clear voice tracks for professional editing of interviews, especially field reports, removing ambient noise without distorting speech.
* **Education & Lectures**: Remove background noise and improve the overall audio quality of recorded lectures, making them clearer and easier for students to understand.
* **Streaming & Social Media**: Clean up your media before posting it on your preferred platform, avoiding copyright issues by removing background music or sound effects.
* **Home Recording Enthusiasts**: Enhance the quality of home-recorded content by removing unnecessary noise or isolating specific audio tracks for better post-production.
* **Regular Media Consumers**: Remove distractions from your favorite podcast or video, allowing you to focus on the content without the sound effects.

## Contributing
Contributions are always welcome! If you're interested in helping improve `Fast Music Remover`, there are several ways to get involved:

* **Report Bugs or Suggest Features**: Open an issue to report a bug or suggest new features you'd like to see.
* **Improve the Code**: Check the open issues and contribute with a pull request.
* **Testing**: Help by manually testing the software or writing automated tests to improve code coverage and robustness.
* **Enhance Documentation**: Improve existing documentation, add examples, or clarify unclear sections. This makes it easier for others to use and contribute.
* **Spread the Word**: Share `Fast Music Remover` with others and participate in [discussions](https://github.com/omeryusufyagci/fast-music-remover/discussions) to shape the project direction.

Please see the [contributing guidelines](CONTRIBUTING.md) for more details on how to get started.

## Benchmarks

The processed audio quality shows no audible degradation so far. A formal benchmarking process for performance and quality is planned. If you're interested in contributing to the benchmarking process, please open an issue or reach out via the [discussions](https://github.com/omeryusufyagci/fast-music-remover/discussions)!

## Prerequisites

To get started with `Fast Music Remover`, ensure that you have the following software installed on your system. These dependencies are necessary for running the backend server, compiling the C++ processor, and handling media files.

- **Python 3.9+**: Required for running the backend server.
- **FFmpeg**: For extracting, probing, and processing audio files.
- **CMake**: Needed to compile the C++ `MediaProcessor`.
- **nlohmann-json**: A JSON library required for parsing configuration files in the `MediaProcessor`.
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

To get started with `Fast Music Remover`, you have two main options: running it directly via Docker (quick and easy) or installing all the necessary dependencies manually for greater customization and control.

### Option 1: Quick Start with Docker

Ensure Docker is installed by running:
```sh
docker --version
```

If Docker is installed, you can quickly try `Fast Music Remover` by running the following command:
```sh
docker-compose up --build
```
> **Note**: You may need `sudo` to run this command, depending on your system.

Once it's up, open `http://localhost:8080` in your browser, and you can test the service right away by submitting a URL.

This Docker setup will spin up a containerized version of the app with everything set up: the Flask backend, the C++ MediaProcessor, and the frontend. Once processing is done, you'll be able to watch the filtered video in the media player.

### Option 2: Manual Installation

For those who want more control or are looking to contribute to development, follow these steps to set up `Fast Music Remover` manually.

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
> **Note**: Ensure there are no errors during compilation. If you encounter an error, double-check that all prerequisites are installed.

#### Step 4: Start the Backend Server

After setting up the dependencies and compiling the C++ project, go back to the project root and start the backend server:
```sh
python3 app.py 
```
> **Note**: The server should indicate it is running on http://127.0.0.1:8080. Visit this address in your browser and submit a URL to begin.

## License

`Fast Music Remover` is released under the MIT license. The license can be found in the [LICENSE](LICENSE) file.
