#include <filesystem>
#include <iostream>
#include <string>

#include "AudioProcessor.h"
#include "ConfigManager.h"
#include "Utils.h"
#include "VideoProcessor.h"

namespace fs = std::filesystem;

using namespace MediaProcessor;

int main(int argc, char* argv[]) {
    /**
     * @brief Processes a video file to isolate vocals and merge them back with the original video.
     *
     * This program removes music, sound effects, and noise while retaining clear vocals.
     *
     * Workflow:
     *   1. Load configuration from "config.json".
     *   2. Extract and isolate vocals using DeepFilterNet.
     *   3. Process the audio in chunks with parallel processing.
     *   4. Merge the isolated vocals back with the original video.
     *
     * @param argc Number of command-line arguments.
     * @param argv Array of command-line argument strings.
     * @return Exit status code (0 for success, non-zero for failure).
     *
     * Usage: <executable> <video_file_path>
     */

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <video_file_path>" << std::endl;
        return 1;
    }
    fs::path inputMediaPath = fs::absolute(argv[1]);

    ConfigManager& configManager = ConfigManager::getInstance();
    if (!configManager.loadConfig("config.json")) {
        std::cerr << "Error: Could not load configuration." << std::endl;
        return 1;
    }

    auto [extractedVocalsPath, processedMediaPath] = Utils::prepareOutputPaths(inputMediaPath);

    AudioProcessor audioProcessor(inputMediaPath, extractedVocalsPath);
    if (!audioProcessor.isolateVocals()) {
        std::cerr << "Failed to extract vocals." << std::endl;
        return 1;
    }

    VideoProcessor videoProcessor(inputMediaPath, extractedVocalsPath, processedMediaPath);
    if (!videoProcessor.mergeMedia()) {
        std::cerr << "Failed to merge audio and video." << std::endl;
        return 1;
    }

    std::cout << "Video processed successfully: " << processedMediaPath << std::endl;
    return 0;
}
