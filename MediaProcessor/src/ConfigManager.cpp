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
    try {
        config_file >> m_config;
    } catch (const std::exception& e) {
        throw std::runtime_error("Could not read config file: " + std::string(e.what()));
    }
    return true;
}

template <typename valueType>
valueType ConfigManager::getConfigValue(const std::string& optionName) const {
    if (!m_config.contains(optionName)) {
        throw std::runtime_error("Config option '" + optionName + "' not found");
    }
    try {
        return m_config[optionName].get<valueType>();
    } catch (const std::exception& e) {
        throw std::runtime_error("Could not read '" + optionName +
                                 "' value: " + std::string(e.what()));
    }
}

fs::path ConfigManager::getDeepFilterPath() const {
    return getConfigValue<std::string>("deep_filter_path");
}

fs::path ConfigManager::getDeepFilterTarballPath() const {
    return m_config["deep_filter_tarball_path"].get<std::string>();
}

fs::path ConfigManager::getDeepFilterEncoderPath() const {
    return m_config["deep_filter_encoder_path"].get<std::string>();
}

fs::path ConfigManager::getDeepFilterDecoderPath() const {
    return m_config["deep_filter_decoder_path"].get<std::string>();
}

fs::path ConfigManager::getFFmpegPath() const {
    return getConfigValue<std::string>("ffmpeg_path");
}

unsigned int ConfigManager::getOptimalThreadCount() {
    unsigned int configNumThreads = getNumThreadsValue();
    unsigned int hardwareNumThreads = HardwareUtils::getHardwareThreadCount();

    return determineNumThreads(configNumThreads, hardwareNumThreads);
}

unsigned int ConfigManager::getNumThreadsValue() {
    bool useThreadCap = getConfigValue<bool>("use_thread_cap");
    if (useThreadCap) {
        return getConfigValue<unsigned int>("max_threads_if_capped");
    } else {
        return 0;
    }
}

unsigned int ConfigManager::determineNumThreads(unsigned int configNumThreads,
                                                unsigned int hardwareNumThreads) {
    return Utils::isWithinRange(configNumThreads, 1, hardwareNumThreads) ? configNumThreads
                                                                         : hardwareNumThreads;
}
}  // namespace MediaProcessor
