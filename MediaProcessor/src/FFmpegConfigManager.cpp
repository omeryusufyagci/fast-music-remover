#include "FFmpegConfigManager.h"

namespace MediaProcessor {

FFmpegConfigManager::FFmpegConfigManager() {
    m_audioCodecAsString = {{AudioCodec::AAC, "aac"},
                            {AudioCodec::MP3, "mp3"},
                            {AudioCodec::FLAC, "flac"},
                            {AudioCodec::OPUS, "opus"},
                            {AudioCodec::UNKNOWN, "unknown"}};

    m_videoCodecAsString = {{VideoCodec::H264, "libx264"},
                            {VideoCodec::H265, "libx265"},
                            {VideoCodec::VP8, "libvpx"},
                            {VideoCodec::VP9, "libvpx-vp9"},
                            {VideoCodec::UNKNOWN, "unknown"}};
}

// Global Setters
void FFmpegConfigManager::setOverwrite(bool overwrite) {
    m_globalSettings.overwrite = overwrite;
}

void FFmpegConfigManager::setInputFilePath(const fs::path inputFilePath) {
    m_globalSettings.inputFilePath = inputFilePath;
}

void FFmpegConfigManager::setOutputFilePath(const fs::path outputFilePath) {
    m_globalSettings.outputFilePath = outputFilePath;
}

// Global Getters
bool FFmpegConfigManager::getOverwrite() const {
    return m_globalSettings.overwrite;
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
}

// Video Getters
VideoCodec FFmpegConfigManager::getVideoCodec() const {
    return m_videoSettings.codec;
}

const std::unordered_map<AudioCodec, std::string>& FFmpegConfigManager::getAudioCodecAsString()
    const {
    return m_audioCodecAsString;
}

const std::unordered_map<VideoCodec, std::string>& FFmpegConfigManager::getVideoCodecAsString()
    const {
    return m_videoCodecAsString;
}

}  // namespace MediaProcessor
