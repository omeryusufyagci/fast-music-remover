#include "FFmpegConfigManager.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace MediaProcessor {

FFmpegConfigManager::FFmpegConfigManager() {
    m_audioCodecAsString = {{AudioCodec::AAC, "aac"},
                            {AudioCodec::MP3, "mp3"},
                            {AudioCodec::FLAC, "flac"},
                            {AudioCodec::OPUS, "opus"},
                            {AudioCodec::UNKNOWN, "unknown"}};

    m_videoCodecAsString = {{VideoCodec::H264, "libx264"}, {VideoCodec::H265, "libx265"},
                            {VideoCodec::VP8, "libvpx"},   {VideoCodec::VP9, "libvpx-vp9"},
                            {VideoCodec::COPY, "copy"},    {VideoCodec::UNKNOWN, "unknown"}};

    m_codecStrictnessAsString = {{CodecStrictness::VERY, "very"},
                                 {CodecStrictness::STRICT, "strict"},
                                 {CodecStrictness::NORMAL, "normal"},
                                 {CodecStrictness::UNOFFICIAL, "unofficial"},
                                 {CodecStrictness::EXPERIMENTAL, "experimental"}};
}

// Global Setters
void FFmpegConfigManager::setOverwrite(bool overwrite) {
    m_globalSettings.overwrite = overwrite;
}

void FFmpegConfigManager::setCodecStrictness(CodecStrictness strictness) {
    m_globalSettings.strictness = strictness;
}

void FFmpegConfigManager::setInputFilePath(const fs::path inputFilePath) {
    fs::perms p = fs::status(inputFilePath).permissions();
    if ((p & fs::perms::owner_read) != fs::perms::owner_read) {
        throw std::runtime_error("Error: Permission denied when reading input file");
    }
    m_globalSettings.inputFilePath = inputFilePath;
}

void FFmpegConfigManager::setOutputFilePath(const fs::path outputFilePath) {
    Utils::ensureDirectoryExists(outputFilePath);
    m_globalSettings.outputFilePath = outputFilePath;
}

// Global Getters
bool FFmpegConfigManager::getOverwrite() const {
    return m_globalSettings.overwrite;
}

CodecStrictness FFmpegConfigManager::getCodecStrictness() const {
    return m_globalSettings.strictness;
}

fs::path FFmpegConfigManager::getInputFilePath() const {
    return m_globalSettings.inputFilePath;
}
fs::path FFmpegConfigManager::getOutputFilePath() const {
    return m_globalSettings.outputFilePath;
}

// Audio Setters
void FFmpegConfigManager::setAudioCodec(AudioCodec codec) {
    m_audioSettings.codec = codec;
}

void FFmpegConfigManager::setAudioSampleRate(int sampleRate) {
    m_audioSettings.sampleRate = sampleRate;
}

void FFmpegConfigManager::setAudioChannels(int channels) {
    m_audioSettings.numChannels = channels;
}

// Audio Getters
AudioCodec FFmpegConfigManager::getAudioCodec() const {
    return m_audioSettings.codec;
}

int FFmpegConfigManager::getAudioSampleRate() const {
    return m_audioSettings.sampleRate;
}

int FFmpegConfigManager::getAudioChannels() const {
    return m_audioSettings.numChannels;
}

// Video Setters
void FFmpegConfigManager::setVideoCodec(VideoCodec codec) {
    m_videoSettings.codec = codec;
    if (codec != VideoCodec::COPY) {
        std::cerr << "Warning: Consider setting the video codec to COPY for higher quality and "
                     "faster conversion times"
                  << std::endl;
    }
}

// Video Getters
VideoCodec FFmpegConfigManager::getVideoCodec() const {
    return m_videoSettings.codec;
}

// Value Map as String Getters
const std::unordered_map<AudioCodec, std::string>& FFmpegConfigManager::getAudioCodecAsString()
    const {
    return m_audioCodecAsString;
}

const std::unordered_map<VideoCodec, std::string>& FFmpegConfigManager::getVideoCodecAsString()
    const {
    return m_videoCodecAsString;
}

const std::unordered_map<CodecStrictness, std::string>&
FFmpegConfigManager::getCodecStrictnessAsString() const {
    return m_codecStrictnessAsString;
}

// Update Settings
void FFmpegConfigManager::updateSettings(const struct FFmpegGlobalSettings& globalSettings,
                                         const struct FFmpegAudioSettings& audioSettings,
                                         const struct FFmpegVideoSettings& videoSettings) {
    m_globalSettings = globalSettings;
    m_audioSettings = audioSettings;
    m_videoSettings = videoSettings;
}

// Validate Settings
template <typename T>
void FFmpegConfigManager::validateOption(const T value, const T min, const T max) {
    if (((value - min) | (max - value)) < 0) {
        std::string codec =
            Utils::enumToString<AudioCodec>(getAudioCodec(), getAudioCodecAsString());
        throw std::runtime_error("Invalid Audio Option \"" + std::to_string(value) +
                                 "\" : FFmpeg accepts values between " + std::to_string(min) +
                                 " and " + std::to_string(max) + " for " + codec);
    }
}

template <typename T>
void FFmpegConfigManager::validateOption(const T value, const std::unordered_set<T>& options) {
    if (std::find(options.begin(), options.end(), value) == options.end()) {
        std::string codec =
            Utils::enumToString<AudioCodec>(getAudioCodec(), getAudioCodecAsString());
        std::ostringstream ssError;
        ssError << "Invalid " + codec + " sample rate: FFmpeg supports ";
        for (const T& opt : options) {
            ssError << opt << " ";
        }
        throw std::runtime_error(ssError.str());
    }
}

// Validate Config
void FFmpegConfigManager::validateAudio() {
    switch (m_audioSettings.codec) {
        case AudioCodec::AAC: {
            auto& validOpts = m_validAudioOptions.AAC;
            validateOption(m_audioSettings.numChannels, validOpts.minChannels,
                           validOpts.maxChannels);
            validateOption(m_audioSettings.sampleRate, validOpts.sampleRates);
            break;
        }
        case AudioCodec::MP3: {
            auto& validOpts = m_validAudioOptions.MP3;
            validateOption(m_audioSettings.numChannels, validOpts.minChannels,
                           validOpts.maxChannels);
            validateOption(m_audioSettings.sampleRate, validOpts.sampleRates);
            break;
        }
        case AudioCodec::FLAC: {
            auto& validOpts = m_validAudioOptions.FLAC;
            validateOption(m_audioSettings.numChannels, validOpts.minChannels,
                           validOpts.maxChannels);
            validateOption(m_audioSettings.sampleRate, validOpts.minSampleRate,
                           validOpts.maxSampleRate);
            break;
        }
        case AudioCodec::OPUS: {
            auto& validOpts = m_validAudioOptions.Opus;
            validateOption(m_audioSettings.numChannels, validOpts.minChannels,
                           validOpts.maxChannels);
            validateOption(m_audioSettings.sampleRate, validOpts.sampleRates);
            break;
        }
        case AudioCodec::UNKNOWN: {
            setAudioCodec(AudioCodec::AAC);
            setAudioChannels(2);
            setAudioSampleRate(48000);
            std::cerr << "Warning: Audio codec cannot be UNKNOWN, default to AAC @ 48000 Hz with 2 "
                         "channels"
                      << std::endl;
            break;
        }
        default:
            return;
    }
}

}  // namespace MediaProcessor
