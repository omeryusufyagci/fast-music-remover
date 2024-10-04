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
#ifdef _WIN32
    return getConfigValue("deep_filter_path_windows");
#else
    return getConfigValue("deep_filter_path_unix");
#endif
}

std::string ConfigManager::getFFmpegPath() const {
#ifdef _WIN32
    return getConfigValue("ffmpeg_path_windows");
#else
    return getConfigValue("ffmpeg_path_unix");
#endif
}

std::string ConfigManager::getConfigValue(const std::string &key) const {
    auto it = m_config.find(key);
    if (it != m_config.end()) {
        return it->get<std::string>();
    } else {
        std::cerr << "Error: Config value not found for key: " << key << std::endl;
        return "";
    }
}

}  // namespace MediaProcessor
