#include "Utils.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstdlib>

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
    std::cout << "Running command: " << command << std::endl;
    int result = system(command.c_str());
    return result == 0;
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
