#include "Utils.h"

#include <array>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace MediaProcessor::Utils {

bool runCommand(const std::string &command) {
    /*
     * Executes a system command, captures its output, and returns true if successful.
     */

    std::array<char, 128> buffer;
    std::string result;
    std::string fullCommand = command + " 2>&1";  // Redirect stderr to stdout
    FILE *pipe = popen(fullCommand.c_str(), "r");
    if (!pipe) {
        std::cerr << "Error: Failed to run command: " << command << std::endl;
        return false;
    }

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }

    int returnCode = pclose(pipe);
    if (returnCode != 0) {
        std::cerr << "Command failed with return code " << returnCode << ":" << std::endl;
        std::cerr << result << std::endl;
        return false;
    }
    return true;
}

std::pair<std::filesystem::path, std::filesystem::path> prepareOutputPaths(
    const std::filesystem::path &videoPath) {
    /*
     * Prepares and returns the output paths for the vocals and processed video files.
     */

    std::string baseFilename = videoPath.stem().string();

    std::filesystem::path outputDir = videoPath.parent_path();

    std::filesystem::path vocalsPath = outputDir / (baseFilename + "_isolated_audio.wav");
    std::filesystem::path processedVideoPath = outputDir / (baseFilename + "_processed_video.mp4");

    return {vocalsPath, processedVideoPath};
}

bool ensureDirectoryExists(const std::filesystem::path &path) {
    /*
     * Ensures the specified directory exists by making the directory if necessary
     */

    if (!std::filesystem::exists(path)) {
        std::cerr << "Output directory does not exist, creating it: " << path << std::endl;
        std::filesystem::create_directories(path);
        return true;
    }
    return false;
}

bool removeFileIfExists(const std::filesystem::path &filePath) {
    if (std::filesystem::exists(filePath)) {
        std::cerr << "File already exists, removing it: " << filePath << std::endl;
        std::filesystem::remove(filePath);
        return true;
    }
    return false;
}

bool containsWhitespace(const std::string &str) {
    return str.find(' ') != std::string::npos;
}

bool isWithinRange(unsigned int value, unsigned int lowerBound, unsigned int upperBound) {
    return value >= lowerBound && value <= upperBound;
}

std::string trimTrailingSpace(const std::string &str) {
    if (str.empty() || str.back() != ' ') {
        return str;
    }
    return str.substr(0, str.size() - 1);
}

}  // namespace MediaProcessor::Utils
