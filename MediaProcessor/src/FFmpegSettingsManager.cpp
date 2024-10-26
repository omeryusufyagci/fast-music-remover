#include "FFmpegSettingsManager.h"

namespace MediaProcessor {

FFmpegSettingsManager::FFmpegSettingsManager() {
    m_audioCodecToString = {{AudioCodec::AAC, "aac"},
                            {AudioCodec::MP3, "mp3"},
                            {AudioCodec::FLAC, "flac"},
                            {AudioCodec::OPUS, "opus"},
                            {AudioCodec::UNKNOWN, "unknown"}};

    m_videoCodecToString = {{VideoCodec::H264, "libx264"},
                            {VideoCodec::H265, "libx265"},
                            {VideoCodec::VP8, "libvpx"},
                            {VideoCodec::VP9, "libvpx-vp9"},
                            {VideoCodec::UNKNOWN, "unknown"}};
}

// Global Setters
void FFmpegSettingsManager::setOverwrite(bool overwrite) {
    m_globalSettings.overwrite = overwrite;
}

// Global Getters
bool FFmpegSettingsManager::getOverwrite() const {
    return m_globalSettings.overwrite;
}

// Audio Setters
void FFmpegSettingsManager::setAudioCodec(AudioCodec codec) {
    m_audioSettings.codec = codec;
}

void FFmpegSettingsManager::setAudioSampleRate(int sampleRate) {
    m_audioSettings.sampleRate = sampleRate;
}

void FFmpegSettingsManager::setAudioChannels(int channels) {
    m_audioSettings.numChannels = channels;
}

// Audio Getters
AudioCodec FFmpegSettingsManager::getAudioCodec() const {
    return m_audioSettings.codec;
}

int FFmpegSettingsManager::getAudioSampleRate() const {
    return m_audioSettings.sampleRate;
}

int FFmpegSettingsManager::getAudioChannels() const {
    return m_audioSettings.numChannels;
}

// Video Setters
void FFmpegSettingsManager::setVideoCodec(VideoCodec codec) {
    m_videoSettings.codec = codec;
}

// Video Getters
VideoCodec FFmpegSettingsManager::getVideoCodec() const {
    return m_videoSettings.codec;
}

std::unordered_map<AudioCodec, std::string>& FFmpegSettingsManager::getAudioCodecAsString() {
    return m_audioCodecToString;
}

std::unordered_map<VideoCodec, std::string>& FFmpegSettingsManager::getVideoCodecAsString() {
    return m_videoCodecToString;
}

}  // namespace MediaProcessor
