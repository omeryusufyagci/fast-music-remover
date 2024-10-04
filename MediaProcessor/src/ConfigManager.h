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

   private:
    ConfigManager() = default;
    std::string getConfigValue(const std::string &key) const;
    nlohmann::json m_config;
};

}  // namespace MediaProcessor

#endif  // CONFIGMANAGER_H
