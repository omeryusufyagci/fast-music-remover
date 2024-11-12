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
    m_globalSettings.inputFilePath = inputFilePath;
}

void FFmpegConfigManager::setOutputFilePath(const fs::path outputFilePath) {
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

void FFmpegConfigManager::setProcessedChunkCol(const std::vector<fs::path> processedChunkCol) {
    m_audioSettings.processedChunkCol = processedChunkCol;
}

void FFmpegConfigManager::setNumChunks(int numChunks) {
    m_audioSettings.numChunks = numChunks;
}

void FFmpegConfigManager::setAudioSampleRate(int sampleRate) {
    m_audioSettings.sampleRate = sampleRate;
}

void FFmpegConfigManager::setAudioChannels(int channels) {
    m_audioSettings.numChannels = channels;
}

void FFmpegConfigManager::setAudioStartTime(double startTime) {
    m_audioSettings.audioStartTime = startTime;
}

void FFmpegConfigManager::setAudioDuration(double duration) {
    m_audioSettings.audioDuration = duration;
}

void FFmpegConfigManager::setOverlapDuration(double overlapDuration) {
    m_audioSettings.overlapDuration = overlapDuration;
}

// Audio Getters
AudioCodec FFmpegConfigManager::getAudioCodec() const {
    return m_audioSettings.codec;
}

std::vector<fs::path> FFmpegConfigManager::getProcessedChunkCol() const {
    return m_audioSettings.processedChunkCol;
}

int FFmpegConfigManager::getNumChunks() const {
    return m_audioSettings.numChunks;
}

int FFmpegConfigManager::getAudioSampleRate() const {
    return m_audioSettings.sampleRate;
}

int FFmpegConfigManager::getAudioChannels() const {
    return m_audioSettings.numChannels;
}

double FFmpegConfigManager::getAudioStartTime() const {
    return m_audioSettings.audioStartTime;
}

double FFmpegConfigManager::getAudioDuration() const {
    return m_audioSettings.audioDuration;
}

double FFmpegConfigManager::getOverlapDuration() const {
    return m_audioSettings.overlapDuration;
}

// Video Setters
void FFmpegConfigManager::setVideoCodec(VideoCodec codec) {
    m_videoSettings.codec = codec;
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
void FFmpegConfigManager::updateSettings(
    const struct FFmpegGlobalSettings& globalSettings = FFmpegGlobalSettings(),
    const struct FFmpegAudioSettings& audioSettings = FFmpegAudioSettings(),
    const struct FFmpegVideoSettings& videoSettings = FFmpegVideoSettings()) {
    m_globalSettings = globalSettings;
    m_audioSettings = audioSettings;
    m_videoSettings = videoSettings;
}
}  // namespace MediaProcessor
