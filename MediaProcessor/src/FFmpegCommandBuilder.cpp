#include "FFmpegCommandBuilder.h"

#include "ConfigManager.h"
#include "FFmpegSettingsManager.h"
#include "Utils.h"

namespace MediaProcessor {

FFmpegCommandBuilder::FFmpegCommandBuilder(FFmpegSettingsManager& ffmpegSettings)
    : m_ffmpegSettings(ffmpegSettings),
      m_ffmpegPath(ConfigManager::getInstance().getFFmpegPath()) {}

FFmpegCommandBuilder& FFmpegCommandBuilder::addOverwrite() {
    addFlag("-y");

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addInputFile(const fs::path& inputFile) {
    m_inputFile = inputFile;
    addFlag("-i", inputFile.string());

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addOutputFile(const fs::path& outputFile) {
    m_outputFile = outputFile;
    addArgument(outputFile.string());

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addAudioCodec() {
    std::string codec = Utils::enumToString<AudioCodec>(m_ffmpegSettings.getAudioCodec(),
                                                        m_ffmpegSettings.getAudioCodecAsString());

    addFlag("-c:a", codec);

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addAudioSampleRate() {
    int sampleRate = m_ffmpegSettings.getAudioSampleRate();
    addFlag("-ar", std::to_string(sampleRate));

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addAudioChannels() {
    int channels = m_ffmpegSettings.getAudioChannels();
    addFlag("-ac", std::to_string(channels));

    return *this;
}

FFmpegCommandBuilder& FFmpegCommandBuilder::addVideoCodec() {
    std::string codec = Utils::enumToString<VideoCodec>(m_ffmpegSettings.getVideoCodec(),
                                                        m_ffmpegSettings.getVideoCodecAsString());

    addFlag("-c:v", codec);

    return *this;
}

std::string FFmpegCommandBuilder::build() const {
    if (m_inputFile.empty()) {
        throw std::runtime_error("Input file path must be specified.");
    }

    if (m_outputFile.empty()) {
        throw std::runtime_error("Output file path must be specified.");
    }

    return CommandBuilder::build();
}

}  // namespace MediaProcessor
