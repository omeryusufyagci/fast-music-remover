#include "ConfigManager.h"

#include <fstream>
#include <iostream>

namespace MediaProcessor {

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

bool ConfigManager::loadConfig(const fs::path& configFilePath) {
    std::ifstream config_file(configFilePath);
    if (!config_file.is_open()) {
        std::cerr << "Error: Could not open " << configFilePath << std::endl;
        return false;
    }

    config_file >> m_config;
    return true;
}

fs::path ConfigManager::getDeepFilterPath() const {
    return m_config["deep_filter_path"].get<std::string>();
}

fs::path ConfigManager::getFFmpegPath() const {
    return m_config["ffmpeg_path"].get<std::string>();
}

}  // namespace MediaProcessor
