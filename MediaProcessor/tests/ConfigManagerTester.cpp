#include <gtest/gtest.h>

#include "../src/ConfigManager.h"
#include "ConfigFileCreator.h"

namespace MediaProcessor::Tests {

// fixture
class ConfigManagerTest : public ::testing::Test {
   protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ConfigManagerTest, LoadValidConfigFile) {
    // Create a temporary config file with valid JSON content
    nlohmann::json jsonContent = {
        {"deep_filter_path", "MediaProcessor/res/deep-filter-0.5.6-x86_64-unknown-linux-musl"},
        {"downloads_dir", "downloads"},
        {"ffmpeg_path", "/usr/bin/ffmpeg"},
        {"upload_folder", "uploads"},
        {"use_thread_cap", true},
        {"max_threads_if_capped", 1}};

    TestConfigFile testConfigFile("testConfig.json", jsonContent);

    // Load the config
    ConfigManager &configManager = ConfigManager::getInstance();
    bool loadSuccess = configManager.loadConfig(testConfigFile.getFilePath());

    // tests
    EXPECT_TRUE(loadSuccess);
    EXPECT_EQ(configManager.getDeepFilterPath(),
              jsonContent["deep_filter_path"].get<std::string>());
    EXPECT_EQ(configManager.getFFmpegPath(), jsonContent["ffmpeg_path"].get<std::string>());
    EXPECT_EQ(configManager.getOptimalThreadCount(),
              jsonContent["max_threads_if_capped"].get<unsigned int>());
}

}  // namespace MediaProcessor::Tests