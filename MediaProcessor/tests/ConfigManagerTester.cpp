#include <gtest/gtest.h>

#include "../src/ConfigManager.h"
#include "ConfigFileCreator.h"

using namespace MediaProcessor;

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

    TempConfigFile tempConfigFile("tempConfig.json", jsonContent);

    // Load the config
    ConfigManager &configManager = ConfigManager::getInstance();
    bool loadSuccess = configManager.loadConfig(tempConfigFile.getFilePath());

    // tests
    EXPECT_TRUE(loadSuccess);
    EXPECT_EQ(configManager.getDeepFilterPath(),
              "MediaProcessor/res/deep-filter-0.5.6-x86_64-unknown-linux-musl");
    EXPECT_EQ(configManager.getFFmpegPath(), "/usr/bin/ffmpeg");
    EXPECT_EQ(configManager.getOptimalThreadCount(), 1);
}