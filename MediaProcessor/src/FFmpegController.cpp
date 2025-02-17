#include "FFmpegController.h"

#include <iostream>

#include "FFmpegCommandBuilder.h"
#include "Utils.h"

namespace MediaProcessor {

FFmpegController::FFmpegController(const FFmpegConfigManager& ffmpegConfig)
    : m_ffmpegConfig(ffmpegConfig), m_ffmpegCmdBuilder(ffmpegConfig) {}

bool FFmpegController::extractAudio() {
    if (m_ffmpegConfig.getOverwrite()) {
        m_ffmpegCmdBuilder.addOverwrite();
    }
    m_ffmpegCmdBuilder.addInputFile()
        .addAudioSampleRate()
        .addAudioChannels()
        .addAudioCodec()
        .addOutputFile();

    return Utils::runCommand(m_ffmpegCmdBuilder.build());
}

std::vector<fs::path> FFmpegController::splitMedia(int numChunks, double overlapDuration) {
    std::vector<double> chunkStartTimes;
    std::vector<double> chunkDurations;
    std::vector<fs::path> chunkPathCol;
    populateChunkDurations(chunkStartTimes, chunkDurations, numChunks, overlapDuration);

    for (int i = 0; i < numChunks; ++i) {
        if (!generateChunkFile(i, chunkStartTimes[i], chunkDurations[i], chunkPathCol)) {
            throw std::runtime_error("Failed to split audio into chunks.");
        }
    }

    return chunkPathCol;
}

bool FFmpegController::generateChunkFile(const int chunkIndex, const double startTime,
                                         const double duration,
                                         std::vector<fs::path>& chunkPathCol) {
    fs::path chunkPath =
        m_ffmpegConfig.getOutputFilePath() / ("chunk_" + std::to_string(chunkIndex) + ".wav");

    // Set higher precision for chunk boundaries
    std::ostringstream ssStartTime, ssDuration;
    ssStartTime << std::fixed << std::setprecision(6) << startTime;
    ssDuration << std::fixed << std::setprecision(6) << duration;

    if (m_ffmpegConfig.getOverwrite()) {
        m_ffmpegCmdBuilder.addOverwrite();
    }
    m_ffmpegCmdBuilder.addStartTime(ssStartTime)
        .addDuration(ssDuration)
        .addInputFile()
        .addAudioSampleRate()
        .addAudioChannels()
        .addAudioCodec()
        .addChunkPath();

    chunkPathCol.push_back(chunkPath);

    return Utils::runCommand(m_ffmpegCmdBuilder.build());
}

bool FFmpegController::mergeChunks(const std::vector<fs::path>& processedChunkCol,
                                   double overlapDuration) {
    if (m_ffmpegConfig.getOverwrite()) {
        m_ffmpegCmdBuilder.addOverwrite();
    }

    for (const auto& chunkPath : processedChunkCol) {
        m_ffmpegCmdBuilder.addInputFile(chunkPath.string());
    }

    if (static_cast<int>(processedChunkCol.size()) >= 2) {
        m_ffmpegCmdBuilder.addFilterComplex(processedChunkCol, overlapDuration);
        m_ffmpegCmdBuilder.addStream("[outa]");
    }

    return Utils::runCommand(m_ffmpegCmdBuilder.build());
}

bool FFmpegController::mergeMedia(const fs::path& videoPath, const fs::path& audioPath) {
    if (m_ffmpegConfig.getOverwrite()) {
        m_ffmpegCmdBuilder.addOverwrite();
    }

    m_ffmpegCmdBuilder.addInputFile(videoPath)
        .addInputFile(audioPath)
        .addVideoCodec()
        .addAudioCodec()
        .addCodecStrictness()
        .addStream("0:v:0")
        .addStream("1:a:0")
        .addShortest()
        .addOutputFile();

    return Utils::runCommand(m_ffmpegCmdBuilder.build());
}

void FFmpegController::populateChunkDurations(std::vector<double>& startTimes,
                                              std::vector<double>& durations, int numChunks,
                                              double overlapDuration) {
    double totalDuration = Utils::getMediaDuration(m_ffmpegConfig.getInputFilePath());
    double chunkDuration = totalDuration / numChunks;

    for (int i = 0; i < numChunks; ++i) {
        double startTime = i * chunkDuration;
        double duration = chunkDuration + overlapDuration;

        // Handle duration for the last chunk
        if (startTime + duration > totalDuration) {
            duration = totalDuration - startTime;
        }

        startTimes.push_back(startTime);
        durations.push_back(duration);
    }
}

}  // namespace MediaProcessor
