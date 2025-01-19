#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include "../src/ConfigManager.h"
#include "../src/Utils.h"
#include "../src/VideoProcessor.h"
#include "TestUtils.h"

namespace fs = std::filesystem;
namespace MediaProcessor::Tests {

fs::path testMediaPath = TEST_MEDIA_DIR;

class VideoProcessorTester : public ::testing::Test {
   protected:
    fs::path testVideoPath;
    fs::path testAudioPath;
    fs::path testOutputDir;
    TestUtils::TestConfigFile testConfigFile;
    ConfigManager& configManager;

    VideoProcessorTester() : configManager(ConfigManager::getInstance()) {}

    void assertFileExists(const fs::path& path) {
        ASSERT_TRUE(fs::exists(path)) << path << " not found.";
    }

    void SetUp() override {
        testMediaPath.make_preferred();
        testVideoPath = testMediaPath / "test_video.mkv";
        testAudioPath = testMediaPath / "test_audio_processed.wav";

        assertFileExists(testVideoPath);
        assertFileExists(testAudioPath);

        testOutputDir = fs::current_path() / "test_output";
        fs::create_directories(testOutputDir);
    }

    void TearDown() override {
        fs::remove_all(testOutputDir);
    }
};

// ClassName_MethodName_StateUnderTest_ExpectedBehavior gtest std naming convention
TEST_F(VideoProcessorTester, MergeMedia_MergesAudioAndVideoCorrectly) {
    /**
     * FIXME: currently only checking for duration here as the filter function
     * is already being checked within the audio tester.
     * Eventually we need check for sensible metrics here.
     */
    ConfigManager& configManager = ConfigManager::getInstance();
    ASSERT_TRUE(configManager.loadConfig(testConfigFile.getFilePath()))
        << "Unable to Load TestConfigFile";

    fs::path testOutputVideoPath = testOutputDir / "test_output_video.mp4";
    VideoProcessor videoProcessor(testVideoPath, testAudioPath, testOutputVideoPath);

    EXPECT_EQ(videoProcessor.mergeMedia(), true);
    EXPECT_TRUE(fs::exists(testOutputVideoPath));

    double originalDuration = MediaProcessor::Utils::getMediaDuration(testVideoPath);
    double outputDuration = MediaProcessor::Utils::getMediaDuration(testOutputVideoPath);
    EXPECT_NEAR(originalDuration, outputDuration, 0.5)
        << "Duration of the merged video differs significantly from the original.";
}

}  // namespace MediaProcessor::Tests
