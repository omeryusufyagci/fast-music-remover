#include "FFmpegCommandBuilder.h"

#include "ConfigManager.h"
#include "FFmpegConfigManager.h"
#include "Utils.h"

namespace MediaProcessor {

FFmpegCommandBuilder::FFmpegCommandBuilder(const FFmpegConfigManager& ffmpegConfig)
    : m_ffmpegConfig(ffmpegConfig) {
    addArgument(ConfigManager::getInstance().getFFmpegPath());
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addOverwrite() {
    addFlag("-y");

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addInputFile() {
    addFlag("-i", m_ffmpegConfig.getInputFilePath());

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addInputFile(const fs::path& inputFilePath) {
    addFlag("-i", inputFilePath);

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addOutputFile() {
    addArgument(m_ffmpegConfig.getOutputFilePath());

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addAudioCodec() {
    addFlag("-c:a", Utils::enumToString<AudioCodec>(m_ffmpegConfig.getAudioCodec(),
                                                    m_ffmpegConfig.getAudioCodecAsString()));

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addAudioSampleRate() {
    addFlag("-ar", std::to_string(m_ffmpegConfig.getAudioSampleRate()));

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addAudioChannels() {
    addFlag("-ac", std::to_string(m_ffmpegConfig.getAudioChannels()));

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addStartTime(const std::ostringstream& startTime) {
    addFlag("-ss", startTime.str());

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addDuration(const std::ostringstream& duration) {
    addFlag("-t", duration.str());

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addVideoCodec() {
    addFlag("-c:v", Utils::enumToString<VideoCodec>(m_ffmpegConfig.getVideoCodec(),
                                                    m_ffmpegConfig.getVideoCodecAsString()));

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addChunkPath() {
    addArgument(m_ffmpegConfig.getOutputFilePath());

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addCodecStrictness() {
    addFlag("-strict",
            Utils::enumToString<CodecStrictness>(m_ffmpegConfig.getCodecStrictness(),
                                                 m_ffmpegConfig.getCodecStrictnessAsString()));

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addStream(const std::string map) {
    addFlag("-map", map);

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addShortest() {
    addFlag("-shortest");

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addFilterComplex(
    const std::vector<fs::path> processedChunkCol, double overlapDuration) {
    addFlag("-filter_complex", buildFilterComplex(processedChunkCol, overlapDuration));

    return *this;
}

std::string FFmpegCommandBuilder::buildCrossfade(int chunkColSize, double overlapDuration) const {
    std::string filterComplex;
    int filterIndex = 0;

    for (int i = 0; i < chunkColSize - 1; ++i) {
        if (i == 0) {
            // Generate a `crossfade` for the first chunk pair (0 and 1)
            filterComplex += "[" + std::to_string(i) + ":a][" + std::to_string(i + 1) +
                             ":a]acrossfade=d=" + std::to_string(overlapDuration) +
                             ":c1=tri:c2=tri[a" + std::to_string(filterIndex) + "]; ";
        } else {
            // For the rest, use the result of the previous crossfade (a<filterIndex-1>)
            // and apply a new crossfade with the next chunk (chunk i+1)
            filterComplex += "[a" + std::to_string(filterIndex - 1) + "][" + std::to_string(i + 1) +
                             ":a]acrossfade=d=" + std::to_string(overlapDuration) +
                             ":c1=tri:c2=tri[a" + std::to_string(filterIndex) + "]; ";
        }
        filterIndex++;
    }

    // Merge the output of the last crossfade into a final output audio stream
    filterComplex += "[a" + std::to_string(filterIndex - 1) + "]amerge=inputs=1[outa]";
    return filterComplex;
}

std::string FFmpegCommandBuilder::buildFilterComplex(const std::vector<fs::path> processedChunkCol,
                                                     double overlapDuration) const {
    // Build filter complex, i.e. a set of instructions for FFmpeg (called filter graph)
    std::string filterComplex =
        buildCrossfade(static_cast<int>(processedChunkCol.size()), overlapDuration);
    return filterComplex;
}

}  // namespace MediaProcessor
