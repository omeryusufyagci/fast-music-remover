#include "ConfigManager.h"

#include <fstream>
#include <iostream>

#include "HardwareUtils.h"
#include "Utils.h"

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
#ifdef _WIN32
    return fs::path(getConfigValue("deep_filter_path_windows"));
#else
    return fs::path(getConfigValue("deep_filter_path_unix"));
#endif
}
fs::path ConfigManager::getFFmpegPath() const {
#ifdef _WIN32
    return fs::path(getConfigValue("ffmpeg_path_windows"));
#else
    return fs::path(getConfigValue("ffmpeg_path_unix"));
#endif
}

std::string ConfigManager::getConfigValue(const std::string& key) const {
    auto it = m_config.find(key);
    if (it != m_config.end()) {
        return it->get<std::string>();
    } else {
        std::cerr << "Error: Config value not found for key: " << key << std::endl;
        return "";
    }
}

unsigned int ConfigManager::getOptimalThreadCount() {
    unsigned int configNumThreads = getNumThreadsValue();
    unsigned int hardwareNumThreads = HardwareUtils::getHardwareThreadCount();

    return determineNumThreads(configNumThreads, hardwareNumThreads);
}

unsigned int ConfigManager::getNumThreadsValue() {
    return (m_config["use_thread_cap"].get<bool>())
               ? m_config["max_threads_if_capped"].get<unsigned int>()
               : 0;
}

unsigned int ConfigManager::determineNumThreads(unsigned int configNumThreads,
                                                unsigned int hardwareNumThreads) {
    return Utils::isWithinRange(configNumThreads, 1, hardwareNumThreads) ? configNumThreads
                                                                         : hardwareNumThreads;
}
}  // namespace MediaProcessor
