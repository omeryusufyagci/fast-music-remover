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

    ConfigManager() = default;
    nlohmann::json m_config; /**< JSON object holding the configuration data. */
};

}  // namespace MediaProcessor

#endif  // CONFIGMANAGER_H
