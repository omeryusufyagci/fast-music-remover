#include <gtest/gtest.h>

#include <fstream>

#include "../src/ConfigManager.h"
#include "testUtils.h"

namespace fs = std::filesystem;
namespace MediaProcessor::Tests {

// Test fixture
class ConfigManagerTest : public ::testing::Test {
   protected:
    ConfigManager& configManager;
    testUtils::TestConfigFile testConfigFile;

    ConfigManagerTest() : configManager(ConfigManager::getInstance()) {}
};

TEST_F(ConfigManagerTest, LoadValidConfigFile) {
    // Create a temporary config file with valid JSON content
    nlohmann::json jsonContent = {
        {"deep_filter_path", "MediaProcessor/res/deep-filter-0.5.6-x86_64-unknown-linux-musl"},
        {"deep_filter_tarball_path", "MediaProcessor/res/DeepFilterNet3_ll_onnx.tar.gz"},
        {"deep_filter_encoder_path",
         "MediaProcessor/res/DeepFilterNet3_ll_onnx/tmp/export/enc.onnx"},
        {"deep_filter_decoder_path",
         "MediaProcessor/res/DeepFilterNet3_ll_onnx/tmp/export/df_dec.onnx"},
        {"ffmpeg_path", "/usr/bin/ffmpeg"},
        {"downloads_path", "downloads"},
        {"uploads_path", "uploads"},
        {"use_thread_cap", true},
        {"max_threads_if_capped", 1}};

    testConfigFile.createConfigFile("testConfig.json", jsonContent);

    // Load the config
    bool loadSuccess = configManager.loadConfig(testConfigFile.getFilePath());

    // tests
    EXPECT_TRUE(loadSuccess);
    EXPECT_EQ(configManager.getDeepFilterPath(),
              jsonContent["deep_filter_path"].get<std::string>());
    EXPECT_EQ(configManager.getDeepFilterTarballPath(),
              jsonContent["deep_filter_tarball_path"].get<std::string>());
    EXPECT_EQ(configManager.getDeepFilterEncoderPath(),
              jsonContent["deep_filter_encoder_path"].get<std::string>());
    EXPECT_EQ(configManager.getDeepFilterDecoderPath(),
              jsonContent["deep_filter_decoder_path"].get<std::string>());
    EXPECT_EQ(configManager.getFFmpegPath(), jsonContent["ffmpeg_path"].get<std::string>());
    EXPECT_EQ(configManager.getOptimalThreadCount(),
              jsonContent["max_threads_if_capped"].get<unsigned int>());
}

TEST_F(ConfigManagerTest, LoadInvalidConfigFile) {
    fs::path invalidConfigPath = "invalid_config.json";

    // Create an invalid config file for testing
    std::ofstream(invalidConfigPath) << "not a json";

    // Load the config
    EXPECT_THROW(configManager.loadConfig(invalidConfigPath), std::runtime_error);
}

TEST_F(ConfigManagerTest, LoadInvalidConfigOptions) {
    // Create a temporary config file with valid JSON content but invalid options
    nlohmann::json jsonContent = {{"deep_filter_path", false},
                                  {"deep_filter_tarball_path", true},
                                  {"deep_filter_encoder_path", 1.0},
                                  {"deep_filter_decoder_path", -1},
                                  {"ffmpeg_path", false},
                                  {"downloads_path", false},
                                  {"uploads_path", false},
                                  {"use_thread_cap", "true"},
                                  {"max_threads_if_capped", -1}};

    testConfigFile.createConfigFile("testConfig.json", jsonContent);

    // Load the config
    bool loadSuccess = configManager.loadConfig(testConfigFile.getFilePath());

    // tests
    EXPECT_TRUE(loadSuccess);
    EXPECT_THROW(configManager.getDeepFilterPath(), std::runtime_error);
    EXPECT_THROW(configManager.getDeepFilterTarballPath(), std::runtime_error);
    EXPECT_THROW(configManager.getDeepFilterEncoderPath(), std::runtime_error);
    EXPECT_THROW(configManager.getDeepFilterDecoderPath(), std::runtime_error);
    EXPECT_THROW(configManager.getFFmpegPath(), std::runtime_error);
    EXPECT_THROW(configManager.getOptimalThreadCount(), std::runtime_error);
}

}  // namespace MediaProcessor::Tests