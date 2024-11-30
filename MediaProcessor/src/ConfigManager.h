#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>

namespace fs = std::filesystem;
namespace MediaProcessor {

/**
 * @brief Manages configuration settings for the application.
 */
class ConfigManager {
   public:
    /**
     * @brief Retrieves the singleton instance of ConfigManager.
     */
    static ConfigManager& getInstance();

    /**
     * @brief Loads the configuration from a JSON file.
     *
     * @return true if the configuration is loaded successfully, false otherwise.
     */
    bool loadConfig(const fs::path& configFilePath);

    fs::path getDeepFilterPath() const;
    fs::path getDeepFilterTarballPath() const;
    fs::path getDeepFilterEncoderPath() const;
    fs::path getDeepFilterDecoderPath() const;
    fs::path getFFmpegPath() const;

    /**
     * @brief Validates and returns the attenuation value or throws!
     *
     * @returns the attenaution value if it's between [0.0f, 100.0f]
     *
     * @throws std::runtime_error if the value provided is not within [0.0f, 100.0f]
     */
    float getFilterAttenuationLimit() const;

    /**
     * @brief Gets the optimal number of threads for processing.
     *
     * @return The optimal thread count based on configuration and hardware.
     */
    unsigned int getOptimalThreadCount();

   private:
    /**
     * @brief Gets the number of threads specified in the configuration.
     */
    unsigned int getNumThreadsValue();

    /**
     * @brief Determines the number of threads to use based on configuration and hardware.
     *
     * @param configNumThreads Number of threads specified in the configuration.
     * @param hardwareNumThreads Number of threads supported by the hardware.
     * @return The number of threads to use.
     */
    unsigned int determineNumThreads(unsigned int configNumThreads,
                                     unsigned int hardwareNumThreads);

    /**
     * @brief Ensures the provided filter attenuation parameter is valid
     *
     * @throws std::runtime_error if the value provided is not within [0.0f, 100.0f]
     */
    void validateFilterAttenuationLimit(float candidate) const;

    template <typename T>
    T getConfigValue(const std::string& optionName) const;

    template <typename T>
    T getConfigValue(const std::string& optionName, const T& defaultValue) const;

    ConfigManager() = default;
    nlohmann::json m_config; /**< JSON object holding the configuration data. */
};

template <typename T>
T ConfigManager::getConfigValue(const std::string& optionName) const {
    if (!m_config.contains(optionName)) {
        throw std::runtime_error("Config option '" + optionName + "' not found.");
    }

    try {
        return m_config[optionName].get<T>();
    } catch (const nlohmann::json::exception& e) {
        throw std::runtime_error("Failed to retrieve config option '" + optionName +
                                 "': " + std::string(e.what()));
    }
}

template <typename T>
T ConfigManager::getConfigValue(const std::string& optionName, const T& defaultValue) const {
    return m_config.value(
        optionName,
        defaultValue);  // built-in .value() from the JSON lib handles the checks for us
}

}  // namespace MediaProcessor

#endif  // CONFIGMANAGER_H
