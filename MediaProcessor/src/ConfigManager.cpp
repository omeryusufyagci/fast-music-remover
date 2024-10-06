#include "ConfigManager.h"

#include <fstream>
#include <iostream>

#include "HardwareUtils.h"

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

unsigned int ConfigManager::getOptimalThreadCount() {
    unsigned int configNumThreads = getNumThreadsValue();
    unsigned int hardwareNumThreads = HardwareUtils::getHardwareThreadCount();

    return determineNumThreads(configNumThreads, hardwareNumThreads);
}

unsigned int ConfigManager::getNumThreadsValue() {
    return (m_config["limit_thread"].get<bool>())
               ? m_config["max_thread"].get<unsigned int>()
               : 0;
}

unsigned int ConfigManager::determineNumThreads(unsigned int configNumThreads,
                                                unsigned int hardwareNumThreads) {
    return (configNumThreads >= 1 && configNumThreads <= hardwareNumThreads) ? configNumThreads
                                                                             : hardwareNumThreads;
}

}  // namespace MediaProcessor
