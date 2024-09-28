#include "ConfigManager.h"

#include <fstream>
#include <iostream>

namespace MediaProcessor {

ConfigManager &ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

bool ConfigManager::loadConfig(const std::string &configFilePath) {
    std::ifstream config_file(configFilePath);
    if (!config_file.is_open()) {
        std::cerr << "Error: Could not open " << configFilePath << std::endl;
        return false;
    }

    config_file >> m_config;
    return true;
}

std::string ConfigManager::getDeepFilterPath() const {
    return m_config["deep_filter_path"].get<std::string>();
}

std::string ConfigManager::getFFmpegPath() const {
    return m_config["ffmpeg_path"].get<std::string>();
}

}  // namespace MediaProcessor
