#include "FFmpegControllerBuilder.h"

#include <cmath>

namespace MediaProcessor {

FFmpegControllerBuilder::FFmpegControllerBuilder()
    : m_ffmpegGlobalConfig(), m_ffmpegAudioConfig(), m_ffmpegVideoConfig() {}

FFmpegControllerBuilder::FFmpegControllerBuilder(FFmpegGlobalConfig &globalConfig,
                                                 FFmpegAudioConfig &audioConfig,
                                                 FFmpegVideoConfig &videoConfig)
    : m_ffmpegGlobalConfig(globalConfig),
      m_ffmpegAudioConfig(audioConfig),
      m_ffmpegVideoConfig(videoConfig) {}

/* FFmpegGlobalConfig getters & setters */

void FFmpegControllerBuilder::setFFmpegPath(fs::path path) {
    m_ffmpegGlobalConfig.m_ffmpegPath = path;
}

void FFmpegControllerBuilder::setOverwrite(bool overwrite) {
    m_ffmpegGlobalConfig.m_overwrite = overwrite;
}

void FFmpegControllerBuilder::setShortest(bool useShortest) {
    m_ffmpegGlobalConfig.m_useShortest = useShortest;
}

void FFmpegControllerBuilder::setStrictness(std::string &mode) {
    m_ffmpegGlobalConfig.m_strictness = mode;
}

fs::path FFmpegControllerBuilder::getFFmpegPath() {
    return m_ffmpegGlobalConfig.m_ffmpegPath;
}

bool FFmpegControllerBuilder::getOverwrite() {
    return m_ffmpegGlobalConfig.m_overwrite;
}

bool FFmpegControllerBuilder::getShortest() {
    return m_ffmpegGlobalConfig.m_useShortest;
}

std::string FFmpegControllerBuilder::getStrictness() {
    return m_ffmpegGlobalConfig.m_strictness;
}

/* FFmpegAudioConfig getters & setters */

void FFmpegControllerBuilder::setAudioSampleRate(unsigned rate) {
    m_ffmpegAudioConfig.m_sampleRate = rate;
}

void FFmpegControllerBuilder::setNumAudioChannels(unsigned channels) {
    m_ffmpegAudioConfig.m_numChannels = channels;
}

void FFmpegControllerBuilder::setAudioMapping(std::string &map) {
    m_ffmpegAudioConfig.m_mapping = map;
}

void FFmpegControllerBuilder::setAudioCodec(std::string &codec) {
    m_ffmpegAudioConfig.m_codec = codec;
}

unsigned FFmpegControllerBuilder::getAudioSampleRate() {
    return m_ffmpegAudioConfig.m_sampleRate;
}

unsigned FFmpegControllerBuilder::getNumAudioChannels() {
    return m_ffmpegAudioConfig.m_numChannels;
}

std::string FFmpegControllerBuilder::getAudioMapping() {
    return m_ffmpegAudioConfig.m_mapping;
}

std::string FFmpegControllerBuilder::getAudioCodec() {
    return m_ffmpegAudioConfig.m_codec;
}

/* FFmpegVideoConfig getters & setters */

void FFmpegControllerBuilder::setVideoCodec(std::string &codec) {
    m_ffmpegVideoConfig.m_codec = codec;
}

void FFmpegControllerBuilder::setVideoMapping(std::string &map) {
    m_ffmpegVideoConfig.m_mapping = map;
}

std::string FFmpegControllerBuilder::getVideoCodec() {
    return m_ffmpegVideoConfig.m_codec;
}

std::string FFmpegControllerBuilder::getVideoMapping() {
    return m_ffmpegVideoConfig.m_mapping;
}

}  // namespace MediaProcessor
