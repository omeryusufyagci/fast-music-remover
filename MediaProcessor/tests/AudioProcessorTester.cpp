#include <gtest/gtest.h>

#include <filesystem>
#include <nlohmann/json.hpp>

#include "../src/AudioProcessor.h"
#include "../src/ConfigManager.h"
#include "testUtils.h"

namespace fs = std::filesystem;
namespace MediaProcessor::Tests {

fs::path testMediaPath = TEST_MEDIA_DIR;

// fixture
class AudioProcessorTester : public ::testing::Test {
   protected:
    fs::path testVideoPath;
    fs::path testAudioProcessedPath;
    fs::path testOutputDir;
    testUtils::TestConfigFile testConfigFile;

    void assertFileExists(fs::path path) {
        ASSERT_TRUE(fs::exists(path)) << path << " not found.";
    }

    void SetUp() override {
        fs::path currentPath = fs::current_path();
        fs::path rootPath = currentPath.parent_path().parent_path();
        testVideoPath = testMediaPath / "test_video.mkv";
        testAudioProcessedPath = testMediaPath / "test_audio_processed.wav";

        assertFileExists(testVideoPath);
        assertFileExists(testAudioProcessedPath);

        testOutputDir = currentPath / "test_output";
        fs::create_directories(testOutputDir);

        testConfigFile.changeConfigOptions("use_thread_cap", true, "max_threads_if_capped", 1);
    }

    void TearDown() override {
        fs::remove_all(testOutputDir);
    }
};

TEST_F(AudioProcessorTester, isolateVocalsFromTestVideoSuccessfully) {
    ConfigManager &configManager = ConfigManager::getInstance();
    ASSERT_TRUE(configManager.loadConfig(testConfigFile.getFilePath()))
        << "Unable to Load TestConfigFile";

    fs::path testAudioOutputPath = testOutputDir / "test_output_audio.wav";
    AudioProcessor audioProcessor(testVideoPath, testAudioOutputPath);

    // Test the isolateVocals function
    EXPECT_EQ(audioProcessor.isolateVocals(), true);

    // Check if the output file was created
    EXPECT_TRUE(fs::exists(testAudioOutputPath));

    // check if output file and already processed files are same
    EXPECT_TRUE(testUtils::compareAudioFiles(testAudioOutputPath, testAudioProcessedPath));
}

}  // namespace MediaProcessor::Tests
