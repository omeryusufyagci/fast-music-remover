#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>

namespace fs = std::filesystem;
namespace MediaProcessor {

class ConfigManager {
   public:
    static ConfigManager& getInstance();

    bool loadConfig(const fs::path& configFilePath);

    fs::path getDeepFilterPath() const;
    fs::path getDeepFilterTarballPath() const;
    fs::path getDeepFilterEncoderPath() const;
    fs::path getDeepFilterDecoderPath() const;

    fs::path getFFmpegPath() const;

    unsigned int getOptimalThreadCount();

   private:
    unsigned int getNumThreadsValue();
    unsigned int determineNumThreads(unsigned int configNumThreads,
                                     unsigned int hardwareNumThreads);

    ConfigManager() = default;
    nlohmann::json m_config;
};

}  // namespace MediaProcessor

#endif  // CONFIGMANAGER_H
