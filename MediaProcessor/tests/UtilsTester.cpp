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

}  // namespace MediaProcessor::Tests