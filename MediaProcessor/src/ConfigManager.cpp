#include "ConfigManager.h"

#include <fmt/format.h>

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
        throw std::runtime_error("Error: Could not open " + configFilePath.string());
        return false;
    }
    try {
        config_file >> m_config;
    } catch (const std::exception& e) {
        throw std::runtime_error("Could not read config file: " + std::string(e.what()));
    }
    return true;
}

fs::path ConfigManager::getDeepFilterPath() const {
    return getConfigValue<std::string>("deep_filter_path");
}

fs::path ConfigManager::getDeepFilterTarballPath() const {
    return getConfigValue<std::string>("deep_filter_tarball_path");
}

fs::path ConfigManager::getDeepFilterEncoderPath() const {
    return getConfigValue<std::string>("deep_filter_encoder_path");
}

fs::path ConfigManager::getDeepFilterDecoderPath() const {
    return getConfigValue<std::string>("deep_filter_decoder_path");
}

fs::path ConfigManager::getFFmpegPath() const {
    return getConfigValue<std::string>("ffmpeg_path");
}

float ConfigManager::getFilterAttenuationLimit() const {
    auto candidateLimit = getConfigValue<float>("filter_attenuation_limit");
    validateFilterAttenuationLimit(candidateLimit);

    return candidateLimit;
}

void ConfigManager::validateFilterAttenuationLimit(float candidateLimit) const {
    if (not Utils::isWithinRange(candidateLimit, 0.0f, 100.0f)) {
        throw std::runtime_error(
            fmt::format("Filter attenuation limit {}"
                        " is not valid. Limit must be within [0.0, 100.0]",
                        candidateLimit));
    }
}

unsigned int ConfigManager::getOptimalThreadCount() {
    unsigned int configNumThreads = getNumThreadsValue();
    unsigned int hardwareNumThreads = HardwareUtils::getHardwareThreadCount();

    return determineNumThreads(configNumThreads, hardwareNumThreads);
}

unsigned int ConfigManager::getNumThreadsValue() {
    if (!getConfigValue<bool>("use_thread_cap")) {
        return 0;
    }
    return getConfigValue<unsigned int>("max_threads_if_capped");
}

unsigned int ConfigManager::determineNumThreads(unsigned int configNumThreads,
                                                unsigned int hardwareNumThreads) {
    return Utils::isWithinRange(configNumThreads, 1u, hardwareNumThreads) ? configNumThreads
                                                                          : hardwareNumThreads;
}
}  // namespace MediaProcessor
