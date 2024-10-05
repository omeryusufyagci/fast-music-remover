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

std::pair<std::string, std::string> prepareOutputPaths(const std::string &videoPath) {
    /*
     * Prepares and returns the output paths for the vocals and processed video files.
     */

    std::filesystem::path videoFilePath(videoPath);
    std::string baseFilename = videoFilePath.stem().string();
    std::string outputDir = videoFilePath.parent_path().string();

    std::string vocalsPath = outputDir + "/" + baseFilename + "_isolated_audio.wav";
    std::string processedVideoPath = outputDir + "/" + baseFilename + "_processed_video.mp4";

    return {vocalsPath, processedVideoPath};
}

bool ensureDirectoryExists(const std::string &path) {
    /*
     * Ensures the specified directory exists by making the directory if necessary
     */

    if (!std::filesystem::exists(path)) {
        std::cout << "Output directory does not exist, creating it: " << path << std::endl;
        std::filesystem::create_directories(path);
        return true;
    }
    return false;
}

bool removeFileIfExists(const std::string &filePath) {
    if (std::filesystem::exists(filePath)) {
        std::cout << "File already exists, removing it: " << filePath << std::endl;
        std::filesystem::remove(filePath);
        return true;
    }
    return false;
}

double getAudioDuration(const std::string &audioPath) {
    /*
     * TODO: ffprobe command to be used via the to-be-made FFMpegUtils class...
     */

    std::string command =
        "ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 \"" +
        audioPath + "\"";
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "Error: Failed to run ffprobe to get audio duration." << std::endl;
        return -1;
    }

    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof buffer, pipe) != NULL) {
        result += buffer;
    }
    pclose(pipe);

    try {
        return std::stod(result);
    } catch (std::exception &e) {
        std::cerr << "Error: Could not parse audio duration." << std::endl;
        return -1;
    }
}

bool containsWhitespace(const std::string &str) {
    return str.find(' ') != std::string::npos;
}

std::string trimTrailingSpace(const std::string &str) {
    if (str.empty() || str.back() != ' ') {
        return str;
    }
    return str.substr(0, str.size() - 1);
}

}  // namespace MediaProcessor::Utils
