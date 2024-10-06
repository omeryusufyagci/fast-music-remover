#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <nlohmann/json.hpp>
#include <string>

namespace MediaProcessor {

class ConfigManager {
   public:
    static ConfigManager &getInstance();

    bool loadConfig(const std::string &configFilePath);
    std::string getDeepFilterPath() const;
    std::string getFFmpegPath() const;
    unsigned int getOptimalThreadCount();

   private:
    unsigned int getNumThreadsValue();
    unsigned int determineNumThreads(unsigned int configNumThreads, unsigned int hardwareNumThreads);

    ConfigManager() = default;
    nlohmann::json m_config;
};

}  // namespace MediaProcessor

#endif  // CONFIGMANAGER_H
