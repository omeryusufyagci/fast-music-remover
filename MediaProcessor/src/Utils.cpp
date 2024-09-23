#include "Utils.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <array>


json Utils::loadConfig(const std::string &configFilePath) {
    json config;
    std::ifstream config_file(configFilePath);
    if (!config_file.is_open()) {
        std::cerr << "Error: Could not open " << configFilePath << std::endl;
        return nullptr;
    }
    config_file >> config;
    return config;
}

bool Utils::runCommand(const std::string &command) {
    std::array<char, 128> buffer;
    std::string result;
    std::string fullCommand = command + " 2>&1"; // Redirect stderr to stdout
    FILE* pipe = popen(fullCommand.c_str(), "r");
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



bool Utils::ensureDirectoryExists(const std::string &path) {
    if (!std::filesystem::exists(path)) {
        std::cout << "Output directory does not exist, creating it: " << path << std::endl;
        std::filesystem::create_directories(path);
        return true;
    }
    return false;
}

bool Utils::removeFileIfExists(const std::string &filePath) {
    if (std::filesystem::exists(filePath)) {
        std::cout << "File already exists, removing it: " << filePath << std::endl;
        std::filesystem::remove(filePath);
        return true;
    }
    return false;
}

double Utils::getAudioDuration(const std::string &audioPath) {
    std::string command = "ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 \"" + audioPath + "\"";
    FILE* pipe = popen(command.c_str(), "r");
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
