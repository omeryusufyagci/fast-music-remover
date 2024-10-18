#include <gtest/gtest.h>

#include <filesystem>
#include <nlohmann/json.hpp>

#include "../src/ConfigManager.h"
#include "../src/VideoProcessor.h"
#include "ConfigFileCreator.h"

namespace MediaProcessor::Tests {

std::filesystem::path testMediaPath = TEST_MEDIA_DIR;

// fixture
class AudioProcessorTester : public ::testing::Test {
   protected:
    std::filesystem::path testVideoPath;
    std::filesystem::path testAudioPath;
    std::filesystem::path testOutputDir;
    TestConfigFile testConfigFile;

    void SetUp() override {
        std::filesystem::path currentPath = std::filesystem::current_path();
        testVideoPath = testMediaPath / "test_video.mkv";
        testAudioPath = testMediaPath / "test_audio.wav";

        // Check if the files exists
        ASSERT_TRUE(std::filesystem::exists(testVideoPath))
            << testVideoPath.string() << " not found at " + testVideoPath.string();
        ASSERT_TRUE(std::filesystem::exists(testAudioPath))
            << testAudioPath.string() << " not found at " + testAudioPath.string();

        // Make a directory for test output
        testOutputDir = currentPath / "test_output";
        std::filesystem::create_directories(testOutputDir);

        // Initialize the TestConfigFile
        nlohmann::json jsonContent = {
            {"deep_filter_path",
             (testMediaPath / "../../res/deep-filter-0.5.6-x86_64-unknown-linux-musl").string()},
            {"downloads_dir", "downloads"},
            {"ffmpeg_path", "/usr/bin/ffmpeg"},
            {"upload_folder", "uploads"},
            {"use_thread_cap", false},
            {"max_threads_if_capped", 6}};

        testConfigFile.createTestConfigFile(currentPath / "testConfig.json", jsonContent);
    }

    void TearDown() override {
        // Delete the testOutputDir
        std::filesystem::remove_all(testOutputDir);
    }
};

TEST_F(AudioProcessorTester, isolateVocalsFromTestVideo) {
    // Load the config
    ConfigManager &configManager = ConfigManager::getInstance();
    ASSERT_TRUE(configManager.loadConfig(testConfigFile.getFilePath()))
        << "Unable to Load TestConfigFile";

    std::filesystem::path testOutputVideoPath = testOutputDir / "test_output_video.mp4";
    VideoProcessor videoProcessor(testVideoPath, testAudioPath, testOutputVideoPath);

    // Test the mergeMedia function
    EXPECT_EQ(videoProcessor.mergeMedia(), true);

    // Check if the output file was created
    EXPECT_TRUE(std::filesystem::exists(testOutputVideoPath));
}

}  // namespace MediaProcessor::Tests