#include <gtest/gtest.h>

#include <filesystem>
#include <nlohmann/json.hpp>

#include "../src/AudioProcessor.h"
#include "../src/FFmpegController.h"
#include "../src/Utils.h"
#include "TestUtils.h"

namespace fs = std::filesystem;
namespace MediaProcessor::Tests {

fs::path testMediaPath = TEST_MEDIA_DIR;

class FFmpegControllerTester : public ::testing::Test {
   protected:
    fs::path testAudioPath;
    fs::path testVideoPath;
    fs::path testChunksPath;
    fs::path testAudioProcessedPath;
    fs::path testVideoProcessedPath;
    fs::path testChunksProcessedPath;
    fs::path testOutputDir;
    int testNumChunks;
    std::vector<fs::path> testProcessedChunkCol;
    TestUtils::TestConfigFile testConfigFile;
    FFmpegConfigManager ffmpegConfig;

    FFmpegControllerTester() : ffmpegConfig(FFmpegConfigManager()) {}

    void assertFileExists(fs::path path) {
        ASSERT_TRUE(fs::exists(path)) << path << " not found.";
    }

    void SetUp() override {
        fs::path currentPath = fs::current_path();

        testAudioPath = testMediaPath / "test_audio.wav";
        testVideoPath = testMediaPath / "test_video.mkv";
        testChunksPath = testMediaPath / "test_chunks";
        testNumChunks = 4;
        testAudioProcessedPath = testMediaPath / "test_audio_processed.wav";
        testVideoProcessedPath = testMediaPath / "test_video_processed.mp4";
        testChunksProcessedPath = testMediaPath / "test_chunks_processed";

        assertFileExists(testAudioPath);
        assertFileExists(testVideoPath);
        assertFileExists(testAudioProcessedPath);
        assertFileExists(testVideoProcessedPath);

        testOutputDir = currentPath / "test_output";
        fs::create_directories(testOutputDir);

        testConfigFile.changeConfigOptions("use_thread_cap", true, "max_threads_if_capped", 1);
    }

    void TearDown() override {
        fs::remove_all(testOutputDir);
    }
};

// ClassName_MethodName_StateUnderTest_ExpectedBehavior gtest std naming convention
TEST_F(FFmpegControllerTester, ExtractAudio_ExtractsAudioCorrectly) {
    fs::path testAudioOutputPath = testOutputDir / "test_output_audio.wav";

    ffmpegConfig.setOverwrite(true);
    ffmpegConfig.setInputFilePath(testVideoPath);
    ffmpegConfig.setAudioSampleRate(48000);
    ffmpegConfig.setAudioChannels(1);
    ffmpegConfig.setAudioCodec(AudioCodec::AAC);
    ffmpegConfig.setOutputFilePath(testAudioOutputPath);

    FFmpegController controller = FFmpegController(ffmpegConfig);

    EXPECT_EQ(controller.extractAudio(), true);

    EXPECT_TRUE(fs::exists(testAudioOutputPath));

    EXPECT_TRUE(TestUtils::CompareFiles::compareAudioFiles(testAudioOutputPath, testAudioPath));
}

TEST_F(FFmpegControllerTester, SplitMedia_MediaSplitCorrectly) {
    fs::path testChunkOutputPath = testOutputDir / "test_output_chunks";

    ffmpegConfig.setOverwrite(true);
    ffmpegConfig.setInputFilePath(testAudioPath);
    ffmpegConfig.setAudioSampleRate(48000);
    ffmpegConfig.setAudioChannels(1);
    ffmpegConfig.setAudioCodec(AudioCodec::AAC);
    ffmpegConfig.setOutputFilePath(testChunkOutputPath);

    FFmpegController controller = FFmpegController(ffmpegConfig);
    testProcessedChunkCol = controller.splitMedia(testNumChunks, DEFAULT_OVERLAP_DURATION);

    EXPECT_TRUE(fs::exists(testChunkOutputPath));

    for (int i = 0; i < testNumChunks; i++) {
        fs::path chunk = testChunkOutputPath / ("chunk_" + std::to_string(i) + ".wav");
        EXPECT_TRUE(fs::exists(chunk));
        EXPECT_TRUE(TestUtils::CompareFiles::compareAudioFiles(
            chunk, testChunksPath / testProcessedChunkCol[i].filename()));
    }
}

TEST_F(FFmpegControllerTester, MergeChunks_ChunksMergedCorrectly) {
    fs::path testChunkOutputPath = testOutputDir / "test_chunks_processed";

    ffmpegConfig.setOverwrite(true);
    ffmpegConfig.setOutputFilePath(testChunkOutputPath);
    ffmpegConfig.setAudioCodec(AudioCodec::AAC);
    ffmpegConfig.setAudioSampleRate(48000);

    for (int i = 0; i < testNumChunks; i++) {
        fs::path chunk = "chunk_" + std::to_string(i) + ".wav";
        EXPECT_TRUE(fs::exists(testChunkOutputPath / chunk));
        EXPECT_TRUE(TestUtils::CompareFiles::compareAudioFiles(
            chunk, testChunksProcessedPath / testProcessedChunkCol[i].filename()));
    }
}

TEST_F(FFmpegControllerTester, MergeMedia_MediaMergedCorrectly) {
    fs::path testVideoOutputPath = testOutputDir / "test_output_video.mp4";

    ffmpegConfig.setOverwrite(true);
    ffmpegConfig.setVideoCodec(VideoCodec::COPY);
    ffmpegConfig.setAudioCodec(AudioCodec::AAC);
    ffmpegConfig.setCodecStrictness(CodecStrictness::EXPERIMENTAL);
    ffmpegConfig.setOutputFilePath(testVideoOutputPath);

    FFmpegController controller = FFmpegController(ffmpegConfig);

    EXPECT_EQ(controller.mergeMedia(testVideoPath, testAudioPath), true);

    EXPECT_TRUE(fs::exists(testVideoOutputPath));

    EXPECT_TRUE(
        TestUtils::CompareFiles::compareAudioFiles(testVideoOutputPath, testVideoProcessedPath));
}

}  // namespace MediaProcessor::Tests
