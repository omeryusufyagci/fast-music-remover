#include "FFmpegCommandBuilder.h"

#include "ConfigManager.h"
#include "FFmpegConfigManager.h"
#include "Utils.h"

namespace MediaProcessor {

FFmpegCommandBuilder::FFmpegCommandBuilder(FFmpegConfigManager& ffmpegConfig)
    : m_ffmpegConfig(ffmpegConfig), m_ffmpegPath(ConfigManager::getInstance().getFFmpegPath()) {}

FFmpegCommandBuilder& FFmpegCommandBuilder::addOverwrite() {
    addFlag("-y");

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addInputFile(const fs::path& inputFile) {
    m_inputFilePath = inputFile;
    addFlag("-i", inputFile.string());

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addOutputFile(const fs::path& outputFile) {
    m_outputFilePath = outputFile;
    addArgument(outputFile.string());

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addAudioCodec() {
    std::string codec = Utils::enumToString<AudioCodec>(m_ffmpegConfig.getAudioCodec(),
                                                        m_ffmpegConfig.getAudioCodecAsString());

    addFlag("-c:a", codec);

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addAudioSampleRate() {
    int sampleRate = m_ffmpegConfig.getAudioSampleRate();
    addFlag("-ar", std::to_string(sampleRate));

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addAudioChannels() {
    int channels = m_ffmpegConfig.getAudioChannels();
    addFlag("-ac", std::to_string(channels));

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addVideoCodec() {
    std::string codec = Utils::enumToString<VideoCodec>(m_ffmpegConfig.getVideoCodec(),
                                                        m_ffmpegConfig.getVideoCodecAsString());

    addFlag("-c:v", codec);

    return *this;
}

std::string FFmpegCommandBuilder::build() const {
    if (m_inputFilePath.empty()) {
        throw std::runtime_error("Input file path must be specified.");
    }

    if (m_outputFilePath.empty()) {
        throw std::runtime_error("Output file path must be specified.");
    }

    return CommandBuilder::build();
}

}  // namespace MediaProcessor
