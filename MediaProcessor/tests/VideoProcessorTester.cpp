#include <gtest/gtest.h>

#include <filesystem>
#include <nlohmann/json.hpp>

#include "../src/ConfigManager.h"
#include "../src/VideoProcessor.h"
#include "testUtils.h"

namespace fs = std::filesystem;
namespace MediaProcessor::Tests {

fs::path testMediaPath = TEST_MEDIA_DIR;

// fixture
class AudioProcessorTester : public ::testing::Test {
   protected:
    fs::path testVideoPath;
    fs::path testAudioPath;
    fs::path testOutputDir;
    testUtils::TestConfigFile testConfigFile;

    void assertFileExists(fs::path path) {
        ASSERT_TRUE(fs::exists(path)) << path << " not found.";
    }

    void SetUp() override {
        fs::path currentPath = fs::current_path();
        fs::path rootPath = fs::current_path().parent_path().parent_path();
        testVideoPath = testMediaPath / "test_video.mkv";
        testAudioPath = testMediaPath / "test_audio.wav";

        // Check if the files exists
        assertFileExists(testVideoPath);
        assertFileExists(testAudioPath);

        // Make a directory for test output
        testOutputDir = currentPath / "test_output";
        fs::create_directories(testOutputDir);

        // configure to use 1 Thread
        testConfigFile.changeConfigOptions("use_thread_cap", true, "max_threads_if_capped", 1);
    }

    void TearDown() override {
        // Delete the testOutputDir
        fs::remove_all(testOutputDir);
    }
};

TEST_F(AudioProcessorTester, isolateVocalsFromTestVideo) {
    // Load the config
    ConfigManager &configManager = ConfigManager::getInstance();
    ASSERT_TRUE(configManager.loadConfig(testConfigFile.getFilePath()))
        << "Unable to Load TestConfigFile";

    fs::path testOutputVideoPath = testOutputDir / "test_output_video.mkv";
    VideoProcessor videoProcessor(testVideoPath, testAudioPath, testOutputVideoPath);

    // Test the mergeMedia function
    EXPECT_EQ(videoProcessor.mergeMedia(), true);

    // Check if the output file was created
    EXPECT_TRUE(fs::exists(testOutputVideoPath));

    // check if already processed video file and output video files are same
    testUtils::compareFilesByteByByte(testVideoPath, testOutputVideoPath);
}

}  // namespace MediaProcessor::Tests