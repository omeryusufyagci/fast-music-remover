#include <gtest/gtest.h>

#include <filesystem>

#include "../src/Utils.h"

namespace fs = std::filesystem;
namespace MediaProcessor::Tests {

// fixture
class UtilsTester : public ::testing::Test {
   protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(UtilsTester, checkPreparedOutputPaths) {
    fs::path videoPath = "/Tests/Video.mp4";
    fs::path expectedVocalsPath = "/Tests/Video_isolated_audio.wav",
             expectedProcessedVideoPath = "/Tests/Video_processed_video.mp4";

    auto [outputVocalsPath, outputProcessedVideoPath] = Utils::prepareOutputPaths(videoPath);
    EXPECT_EQ(expectedVocalsPath, outputVocalsPath);
    EXPECT_EQ(expectedProcessedVideoPath, outputProcessedVideoPath);
}

TEST_F(UtilsTester, EnsureDirectoryExists) {
    fs::path tempPath = "test_dir";
    EXPECT_FALSE(fs::exists(tempPath));

    // Call ensureDirectoryExists
    bool result = Utils::ensureDirectoryExists(tempPath);

    EXPECT_TRUE(fs::exists(tempPath));
    EXPECT_TRUE(result);

    EXPECT_FALSE(Utils::ensureDirectoryExists(tempPath));

    fs::remove_all(tempPath);
}

TEST_F(UtilsTester, TrimTrailingSpaces) {
    std::string inputWithTrailingSpace = "Hello, World! ";
    std::string inputWithoutTrailingSpace = "Hello, World!";

    EXPECT_EQ(inputWithoutTrailingSpace, Utils::trimTrailingSpace(inputWithTrailingSpace));
    EXPECT_EQ(inputWithoutTrailingSpace, Utils::trimTrailingSpace(inputWithoutTrailingSpace));
}

}  // namespace MediaProcessor::Tests