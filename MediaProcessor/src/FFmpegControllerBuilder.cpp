#include "FFmpegControllerBuilder.h"

#include "FFmpegController.h"
#include "FFmpegSettingsManager.h"

namespace MediaProcessor {

FFmpegControllerBuilder::FFmpegControllerBuilder() : m_ffmpegSettings(FFmpegSettingsManager()) {}

FFmpegControllerBuilder::FFmpegControllerBuilder(FFmpegSettingsManager& ffmpegSettings)
    : m_ffmpegSettings(std::move(ffmpegSettings)) {}

FFmpegControllerBuilder& FFmpegControllerBuilder::setOverwrite(bool overwrite) {
    m_ffmpegSettings.setOverwrite(overwrite);

    return *this;
}

FFmpegControllerBuilder& FFmpegControllerBuilder::setAudioCodec(AudioCodec codec) {
    m_ffmpegSettings.setAudioCodec(codec);

    return *this;
}

FFmpegControllerBuilder& FFmpegControllerBuilder::setAudioSampleRate(int sampleRate) {
    m_ffmpegSettings.setAudioSampleRate(sampleRate);

    return *this;
}

FFmpegControllerBuilder& FFmpegControllerBuilder::setAudioChannels(int numChannels) {
    m_ffmpegSettings.setAudioChannels(numChannels);

    return *this;
}

FFmpegControllerBuilder& FFmpegControllerBuilder::setVideoCodec(VideoCodec codec) {
    m_ffmpegSettings.setVideoCodec(codec);

    return *this;
}

std::unique_ptr<FFmpegController> FFmpegControllerBuilder::build() const {
    return std::make_unique<FFmpegController>(std::move(m_ffmpegSettings));
}

}  // namespace MediaProcessor
