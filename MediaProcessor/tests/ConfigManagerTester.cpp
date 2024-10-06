#include <gtest/gtest.h>
#include "../src/ConfigManager.h"
#include <fstream>

using namespace MediaProcessor;

// Helper function to create a temporary config file for testing
std::string createTempConfigFile(const std::string &content) {
    std::string tempFilePath = "temp_config.json";
    std::ofstream tempFile(tempFilePath);
    if (tempFile.is_open()) {
        tempFile << content;
        tempFile.close();
    }
    return tempFilePath;
}

// fixture
class ConfigManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
        // Clean up temporary files after the tests
        remove("temp_config.json");
    }
};

TEST_F(ConfigManagerTest, LoadValidConfigFile) {
    // Create a temporary config file with valid JSON content
    std::string configContent = R"({
        "deep_filter_path": "MediaProcessor/res/deep-filter-0.5.6-x86_64-unknown-linux-musl",
        "downloads_dir": "downloads",
        "ffmpeg_path": "/usr/bin/ffmpeg",
        "upload_folder": "uploads",
        "limit_thread": false,
        "max_thread": 6
    })";

    std::string configFilePath = createTempConfigFile(configContent);

    // Load the config
    ConfigManager &configManager = ConfigManager::getInstance();
    bool loadSuccess = configManager.loadConfig(configFilePath);

    // Assertions
    ASSERT_TRUE(loadSuccess);
    ASSERT_EQ(configManager.getDeepFilterPath(), "MediaProcessor/res/deep-filter-0.5.6-x86_64-unknown-linux-musl");
    ASSERT_EQ(configManager.getFFmpegPath(), "/usr/bin/ffmpeg");
}