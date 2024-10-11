#include <gtest/gtest.h>

#include <filesystem>

#include "../src/Utils.h"

namespace MediaProcessor::UnitTesting {

// fixture
class UtilsTester : public ::testing::Test {
   protected:
    void SetUp() override {
        // check for test directory
    }
    void TearDown() override {}
};

TEST_F(UtilsTester, checkPreparedOutputPaths) {
    std::filesystem::path videoPath = "/Tests/Video.mp4",
                          expectedVocalsPath = "/Tests/Video_isolated_audio.wav",
                          expectedProcessedVideoPath = "/Tests/Video_processed_video.mp4";

    auto [outputVocalsPath, outputProcessedVideoPath] = Utils::prepareOutputPaths(videoPath);
    EXPECT_EQ(expectedVocalsPath, outputVocalsPath);
    EXPECT_EQ(expectedProcessedVideoPath, outputProcessedVideoPath);
}

}  // namespace MediaProcessor::UnitTesting