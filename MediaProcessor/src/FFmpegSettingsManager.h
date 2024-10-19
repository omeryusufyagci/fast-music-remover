#ifndef FFMPEGSETTINGSMANAGER_H
#define FFMPEGSETTINGSMANAGER_H

#include <string>
#include <unordered_map>

#include "ConfigManager.h"

namespace MediaProcessor {

enum class AudioCodec { AAC, MP3, FLAC, OPUS, UNKNOWN };
enum class VideoCodec { H264, H265, VP8, VP9, UNKNOWN };

/**
 * @brief Manages FFmpeg-specific global, audio, and video settings.
 *
 * The FFmpegSettingsManager is a singleton that provides interfaces to set
 * and get settings for global, audio, and video configurations.
 *
 * Note: This class does not manage configuration data directly. Configurations
 * should be retrieved via the `ConfigManager`.
 */
class FFmpegSettingsManager {
   public:
    /**
     * @brief Retrieves the singleton instance of FFmpegSettingsManager.
     */
    static FFmpegSettingsManager& getInstance();

    // Global Setters
    void setOverwrite(bool overwrite);
    void setInputFile(const std::string& inputFile);
    void setOutputFile(const std::string& outputFile);

    // Global Getters
    bool getOverwrite() const;
    std::string getInputFile() const;
    std::string getOutputFile() const;

    // Audio Setters
    void setAudioCodec(AudioCodec codec);
    void setAudioSampleRate(int sampleRate);
    void setAudioChannels(int channels);

    // Audio Getters
    AudioCodec getAudioCodec() const;
    int getAudioSampleRate() const;
    int getAudioChannels() const;

    // Video Setters
    void setVideoCodec(VideoCodec codec);

    // Video Getters
    VideoCodec getVideoCodec() const;

    /**
     * @brief Converts an enum value to its corresponding string representation.
     *
     * @tparam T Enum type.
     * @param value The enum value.
     * @param valueMap Map of enum values to their string representations.
     * @return The string representation of the enum value.
     */
    template <typename T>
    std::string enumToString(const T& value,
                             const std::unordered_map<T, std::string>& valueMap) const;

    /**
     * @brief Retrieves the path to the FFmpeg executable.
     *
     * @return The FFmpeg path as a string.
     */
    std::string getFFmpegPath() const {
        return ConfigManager::getInstance().getFFmpegPath().string();
    }

   private:
    FFmpegSettingsManager();

    std::unordered_map<AudioCodec, std::string> m_audioCodecToString;
    std::unordered_map<VideoCodec, std::string> m_videoCodecToString;

    struct FFmpegGlobalSettings {
        bool overwrite = false;
        std::string inputFile;
        std::string outputFile;
    } m_globalSettings;

    struct FFmpegAudioSettings {
        AudioCodec codec = AudioCodec::AAC;
        int sampleRate = 48000;
        int numChannels = 2;
    } m_audioSettings;

    struct FFmpegVideoSettings {
        VideoCodec codec = VideoCodec::H264;
    } m_videoSettings;
};

}  // namespace MediaProcessor

#endif  // FFMPEGSETTINGSMANAGER_H
