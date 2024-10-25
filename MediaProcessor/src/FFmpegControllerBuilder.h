#ifndef FFMPEGCONTROLLERBUILDER_H
#define FFMPEGCONTROLLERBUILDER_H

#include <filesystem>
#include <string>
#include <vector>

#include "CommandBuilder.h"
#include "ConfigManager.h"
#include "FFmpegController.h"
#include "FFmpegSettingsManager.h"

namespace MediaProcessor {

/**
 * @brief Builds FFmpegController and FFmpegSettingsManager objects
 *
 * Provides an interface for setting FFmpeg-specific configuration options with
 * FFmpegSettingsManager and builds a FFmpegController with the given settings
 */
class FFmpegControllerBuilder {
   public:
    FFmpegControllerBuilder();

    /**
     * @brief Initializes the FFmpegControllerBuilder with FFmpegSettingsManager
     *
     * This constructor will only accept references to FFmpegSettingsManager
     * objects. Copy initialization is disallowed, and FFmpegControllerManger
     * takes ownership of the reference.
     */
    explicit FFmpegControllerBuilder(FFmpegSettingsManager& ffmpegSettings);

    /**
     * @brief Sets the overwrite flag
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegControllerBuilder& setOverwrite(bool overwrite);

    /**
     * @brief Sets the audio codec
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegControllerBuilder& setAudioCodec(AudioCodec codec);

    /**
     * @brief Sets the audio sample rate
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegControllerBuilder& setAudioSampleRate(int sampleRate);

    /**
     * @brief Sets the number of audio channels
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegControllerBuilder& setAudioChannels(int numChannels);

    /**
     * @brief Sets the video codec
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegControllerBuilder& setVideoCodec(VideoCodec codec);

    /**
     * @brief Builds a FFmpegController
     *
     * @return A unique pointer to an instance FFmpegController
     */
    std::unique_ptr<FFmpegController> build() const;

   private:
    FFmpegSettingsManager m_ffmpegSettings;
};

}  // namespace MediaProcessor

#endif /* FFMPEGCONTROLLERBUILDER_H */
