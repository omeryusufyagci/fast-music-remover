#include <gtest/gtest.h>

#include <fstream>

#include "../src/ConfigManager.h"
#include "TestUtils.h"

namespace fs = std::filesystem;
namespace MediaProcessor::Tests {

class ConfigManagerTest : public ::testing::Test {
   protected:
    ConfigManager& configManager;
    TestUtils::TestConfigFile testConfigFile;

    ConfigManagerTest() : configManager(ConfigManager::getInstance()) {}
};

// ClassName_MethodName_StateUnderTest_ExpectedBehavior gtest std naming convention
TEST_F(ConfigManagerTest, LoadValidConfigFile_Succeeds) {
    // Generate a temporary config file with valid JSON object
    nlohmann::json jsonObject = {
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
        {"max_threads_if_capped", 1},
        {"filter_attenuation_limit", 100.0f},
    };

    testConfigFile.generateConfigFile("testConfig.json", jsonObject);

    bool loadSuccess = configManager.loadConfig(testConfigFile.getFilePath());

    EXPECT_TRUE(loadSuccess);
    EXPECT_EQ(configManager.getDeepFilterPath(), jsonObject["deep_filter_path"].get<std::string>());
    EXPECT_EQ(configManager.getDeepFilterTarballPath(),
              jsonObject["deep_filter_tarball_path"].get<std::string>());
    EXPECT_EQ(configManager.getDeepFilterEncoderPath(),
              jsonObject["deep_filter_encoder_path"].get<std::string>());
    EXPECT_EQ(configManager.getDeepFilterDecoderPath(),
              jsonObject["deep_filter_decoder_path"].get<std::string>());
    EXPECT_EQ(configManager.getFFmpegPath(), jsonObject["ffmpeg_path"].get<std::string>());
    EXPECT_EQ(configManager.getOptimalThreadCount(),
              jsonObject["max_threads_if_capped"].get<unsigned int>());
    EXPECT_EQ(configManager.getFilterAttenuationLimit(),
              jsonObject["filter_attenuation_limit"].get<float>());
}

TEST_F(ConfigManagerTest, LoadInvalidConfigFile) {
    fs::path invalidConfigPath = "invalid_config.json";

    // Generate an invalid config file for testing
    std::ofstream(invalidConfigPath) << "not a json";

    EXPECT_THROW(configManager.loadConfig(invalidConfigPath), std::runtime_error);
}

TEST_F(ConfigManagerTest, LoadInvalidConfigOptions) {
    // Generate a temporary config file with valid JSON object with invalid options
    nlohmann::json jsonObject = {{"deep_filter_path", false},
                                 {"deep_filter_tarball_path", true},
                                 {"deep_filter_encoder_path", 1.0},
                                 {"deep_filter_decoder_path", -1},
                                 {"ffmpeg_path", false},
                                 {"downloads_path", false},
                                 {"uploads_path", false},
                                 {"use_thread_cap", "true"},
                                 {"max_threads_if_capped", -1},
                                 {"filter_attenuation_limit", 100.0f}};

    testConfigFile.generateConfigFile("testConfig.json", jsonObject);

    bool loadSuccess = configManager.loadConfig(testConfigFile.getFilePath());

    EXPECT_TRUE(loadSuccess);
    EXPECT_THROW(configManager.getDeepFilterPath(), std::runtime_error);
    EXPECT_THROW(configManager.getDeepFilterTarballPath(), std::runtime_error);
    EXPECT_THROW(configManager.getDeepFilterEncoderPath(), std::runtime_error);
    EXPECT_THROW(configManager.getDeepFilterDecoderPath(), std::runtime_error);
    EXPECT_THROW(configManager.getFFmpegPath(), std::runtime_error);
    EXPECT_THROW(configManager.getOptimalThreadCount(), std::runtime_error);
}

}  // namespace MediaProcessor::Tests