#include <gtest/gtest.h>

#include <filesystem>

#include "../src/Utils.h"

namespace fs = std::filesystem;
namespace MediaProcessor::Tests {

TEST(UtilsTester, checkPreparedOutputPaths) {
    fs::path videoPath = "/Tests/Video.mp4";
    fs::path expectedVocalsPath = "/Tests/Video_isolated_audio.wav",
             expectedProcessedVideoPath = "/Tests/Video_processed_video.mp4";

    auto [outputVocalsPath, outputProcessedVideoPath] = Utils::prepareOutputPaths(videoPath);
    EXPECT_EQ(expectedVocalsPath, outputVocalsPath);
    EXPECT_EQ(expectedProcessedVideoPath, outputProcessedVideoPath);
}

TEST(UtilsTester, EnsureDirectoryExists) {
    fs::path tempPath = fs::temp_directory_path() / "test_dir";

    EXPECT_FALSE(fs::exists(tempPath));

    EXPECT_TRUE(Utils::ensureDirectoryExists(tempPath));
    EXPECT_TRUE(fs::exists(tempPath));

    EXPECT_FALSE(Utils::ensureDirectoryExists(tempPath));

    fs::remove_all(tempPath);
}

TEST(UtilsTester, TrimTrailingSpaces) {
    std::string inputWithTrailingSpace = "Hello, World! ";
    std::string inputWithoutTrailingSpace = "Hello, World!";

    EXPECT_EQ(inputWithoutTrailingSpace, Utils::trimTrailingSpace(inputWithTrailingSpace));
    EXPECT_EQ(inputWithoutTrailingSpace, Utils::trimTrailingSpace(inputWithoutTrailingSpace));
}

}  // namespace MediaProcessor::Tests