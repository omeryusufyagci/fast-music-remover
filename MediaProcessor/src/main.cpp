#include <iostream>

#include "Engine.h"

using namespace MediaProcessor;

int main(int argc, char* argv[]) {
    /**
     * @brief Processes a media file (audio or video) to isolate vocals and output the processed
     * result.
     *
     * This program removes music, sound effects, and noise while retaining clear vocals.
     * It supports both audio and video files, adapting the workflow based on the file type.
     *
     * Workflow:
     *   1. Load configuration from "config.json".
     *   2. Determine if the input file is audio or video.
     *   3. For audio files:
     *      - Directly process audio to isolate vocals.
     *   4. For video files:
     *      - Extract and isolate vocals from the audio track.
     *      - Process the audio in chunks with parallel processing.
     *      - Merge the isolated vocals back with the original video.
     *
     * @param argc Number of command-line arguments.
     * @param argv Array of command-line argument strings.
     * @return Exit status code (0 for success, non-zero for failure).
     *
     * Usage: <executable> <media_file_path>
     *
     * Example:
     *   - For video: <executable> input_video.mp4
     *   - For audio: <executable> input_audio.wav
     */

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <media_file_path>" << std::endl;
        return 1;
    }

    MediaProcessor::Engine engine(argv[1]);
    if (!engine.processMedia()) {
        std::cerr << "Media processing failed." << std::endl;
        return 1;
    }

    return 0;
}
