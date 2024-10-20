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
class VideoProcessorTester : public ::testing::Test {
   protected:
    fs::path testVideoPath;
    fs::path testAudioPath;
    fs::path testVideoProcessedPath;
    fs::path testOutputDir;
    testUtils::TestConfigFile testConfigFile;

    void assertFileExists(fs::path path) {
        ASSERT_TRUE(fs::exists(path)) << path << " not found.";
    }

    void SetUp() override {
        fs::path currentPath = fs::current_path();

        testVideoPath = testMediaPath / "test_video.mkv";
        testAudioPath = testMediaPath / "test_audio_processed.wav";
        testVideoProcessedPath = testMediaPath / "test_video_processed.mp4";

        assertFileExists(testVideoPath);
        assertFileExists(testAudioPath);
        assertFileExists(testVideoProcessedPath);

        testOutputDir = currentPath / "test_output";
        fs::create_directories(testOutputDir);
    }

    void TearDown() override {
        fs::remove_all(testOutputDir);
    }
};

TEST_F(VideoProcessorTester, MergeMediaSuccessfully) {
    ConfigManager &configManager = ConfigManager::getInstance();
    ASSERT_TRUE(configManager.loadConfig(testConfigFile.getFilePath()))
        << "Unable to Load TestConfigFile";

    fs::path testOutputVideoPath = testOutputDir / "test_output_video.mp4";
    VideoProcessor videoProcessor(testVideoPath, testAudioPath, testOutputVideoPath);

    // Test the mergeMedia function
    EXPECT_EQ(videoProcessor.mergeMedia(), true);

    // Check if the output file was created
    EXPECT_TRUE(fs::exists(testOutputVideoPath));

    // check if already processed video file and output video files are same
    EXPECT_TRUE(testUtils::CompareFiles::compareFilesByteByByte(testVideoProcessedPath,
                                                                testOutputVideoPath));
}

}  // namespace MediaProcessor::Tests