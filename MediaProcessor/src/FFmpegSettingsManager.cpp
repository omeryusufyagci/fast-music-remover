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

FFmpegSettingsManager& FFmpegSettingsManager::getInstance() {
    static FFmpegSettingsManager instance;
    return instance;
}

// Global Setters
void FFmpegSettingsManager::setOverwrite(bool overwrite) {
    m_globalSettings.overwrite = overwrite;
}

void FFmpegSettingsManager::setInputFile(const std::string& inputFile) {
    m_globalSettings.inputFile = inputFile;
}

void FFmpegSettingsManager::setOutputFile(const std::string& outputFile) {
    m_globalSettings.outputFile = outputFile;
}

// Global Getters
bool FFmpegSettingsManager::getOverwrite() const {
    return m_globalSettings.overwrite;
}

std::string FFmpegSettingsManager::getInputFile() const {
    return m_globalSettings.inputFile;
}

std::string FFmpegSettingsManager::getOutputFile() const {
    return m_globalSettings.outputFile;
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

template <typename T>
std::string FFmpegSettingsManager::enumToString(
    const T& value, const std::unordered_map<T, std::string>& valueMap) const {
    auto it = valueMap.find(value);
    return (it != valueMap.end()) ? it->second : "unknown";
}

// Explicit instantiations ensure the compiler generates the template for a type
template std::string FFmpegSettingsManager::enumToString<AudioCodec>(
    const AudioCodec& codec, const std::unordered_map<AudioCodec, std::string>& codecMap) const;
template std::string FFmpegSettingsManager::enumToString<VideoCodec>(
    const VideoCodec& codec, const std::unordered_map<VideoCodec, std::string>& codecMap) const;

}  // namespace MediaProcessor
