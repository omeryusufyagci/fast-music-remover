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

bool FFmpegController::generateChunkFile() {
    fs::path chunkPath = m_ffmpegConfig.getChunksPath() /
                         ("chunk_" + std::to_string(m_ffmpegConfig.getChunkIndex()) + ".wav");

    // Set higher precision for chunk boundaries
    std::ostringstream ssStartTime, ssDuration;
    ssStartTime << std::fixed << std::setprecision(6) << m_ffmpegConfig.getAudioStartTime();
    ssDuration << std::fixed << std::setprecision(6) << m_ffmpegConfig.getAudioDuration();

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

    return Utils::runCommand(m_ffmpegCmdBuilder.build());
}

bool FFmpegController::mergeChunks() {
    if (m_ffmpegConfig.getOverwrite()) {
        m_ffmpegCmdBuilder.addOverwrite();
    }

    const std::vector<fs::path> processedChunkCol = m_ffmpegConfig.getProcessedChunkCol();
    for (const auto& chunkPath : processedChunkCol) {
        m_ffmpegCmdBuilder.addInputFile(chunkPath.string());
    }

    if (static_cast<int>(processedChunkCol.size()) >= 2) {
        m_ffmpegCmdBuilder.addFilterComplex(processedChunkCol);
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

}  // namespace MediaProcessor
