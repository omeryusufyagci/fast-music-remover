#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

#include "AudioProcessor.h"
#include "ConfigManager.h"
#include "Utils.h"
#include "VideoProcessor.h"

namespace fs = std::filesystem;
using json = nlohmann::json;
using namespace MediaProcessor;

int main() {
    /**
     * @brief Processes a video file to isolate vocals and merge them back with the original video
     * using JSON input and output.
     *
     * This program removes music, sound effects, and noise while retaining clear vocals.
     *
     * Workflow:
     *   1. Load configuration from "config.json".
     *   2. Extract and isolate vocals using DeepFilterNet.
     *   3. Process the audio in chunks with parallel processing.
     *   4. Merge the isolated vocals back with the original video.
     *
     * @param json_input This input is received from the input cin.
     * @return Exit status code (0 for success, non-zero for failure).
     *
     * Usage: JSON input via stdin
     */

    // Read JSON input from stdin
    std::string json_input;
    std::getline(std::cin, json_input);

    std::cerr << "The subprocess has been started";

    json input;
    try {
        input = json::parse(json_input);
    } catch (const json::parse_error& e) {
        std::cerr << "Invalid JSON input." << e.what() << std::endl;
        return 1;
    }

    // Extract video file path and config file path from JSON
    std::string video_file_path, config_file_path;
    try {
        json data = input.at("data");
        video_file_path = data.at("video_file_path").get<std::string>();
        config_file_path = data.at("config_file_path").get<std::string>();

        std::cerr << "\nThese is the passed parameter\n\n"
                  << "video_file_path: " << video_file_path << "\n"
                  << "config_file_path: " << config_file_path << "\n";

    } catch (const json::exception& e) {
        std::cerr << "Missing or invalid input fields." << e.what() << std::endl;
        return 1;
    }

    fs::path inputMediaPath = fs::absolute(video_file_path);
    fs::path configFilePath = fs::absolute(config_file_path);

    // Load the configuration
    ConfigManager& configManager = ConfigManager::getInstance();
    if (!configManager.loadConfig(configFilePath)) {
        std::cerr << "Error: Could not load configuration from " << configFilePath << std::endl;
        return 1;
    }

    auto [extractedVocalsPath, processedMediaPath] = Utils::prepareOutputPaths(inputMediaPath);

    AudioProcessor audioProcessor(inputMediaPath, extractedVocalsPath);
    if (!audioProcessor.isolateVocals()) {
        std::cerr << "Failed to extract vocals." << std::endl;
        return 1;
    }

    std::cerr << "Before Merging" << std::endl;
    VideoProcessor videoProcessor(inputMediaPath, extractedVocalsPath, processedMediaPath);

    if (!videoProcessor.mergeMedia()) {
        std::cerr << "Failed to merge audio and video." << std::endl;
        return 1;
    }
    // Output JSON success response

    std::cerr << "Before sending the JSON response" << std::endl;

    json success_response = {{"status", "success"},
                             {"message", "Video processed successfully."},
                             {"data", {{"processed_video_path", processedMediaPath.string()}}}};
    std::cout << success_response.dump() << std::endl;
    return 0;
}
