#ifndef FFMPEGCONFIGMANAGER_H
#define FFMPEGCONFIGMANAGER_H

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "ConfigManager.h"

namespace fs = std::filesystem;

namespace MediaProcessor {

enum class AudioCodec { AAC, MP3, FLAC, OPUS, UNKNOWN };
enum class VideoCodec { H264, H265, VP8, VP9, UNKNOWN };
enum class CodecStrictness { VERY, STRICT, NORMAL, UNOFFICIAL, EXPERIMENTAL };

/**
 * @brief Manages FFmpeg-specific configurations and user settings
 *
 * Provides an interface for setting and retrieving configuration options,
 * including file paths, codecs, and other processing parameters.
 */
class FFmpegConfigManager {
   public:
    FFmpegConfigManager();

    // Global Setters
    void setOverwrite(bool overwrite);
    void setCodecStrictness(CodecStrictness strictness);
    void setInputFilePath(const fs::path inputFilePath);
    void setOutputFilePath(const fs::path outputFilePath);

    // Global Getters
    bool getOverwrite() const;
    CodecStrictness getCodecStrictness() const;
    fs::path getInputFilePath() const;
    fs::path getOutputFilePath() const;

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

    // Value Map Getters
    const std::unordered_map<AudioCodec, std::string>& getAudioCodecAsString() const;
    const std::unordered_map<VideoCodec, std::string>& getVideoCodecAsString() const;
    const std::unordered_map<CodecStrictness, std::string>& getCodecStrictnessAsString() const;

    friend class FFmpegController;

   protected:
    struct FFmpegGlobalSettings {
        bool overwrite;
        CodecStrictness strictness;
        fs::path inputFilePath;
        fs::path outputFilePath;

        FFmpegGlobalSettings() : overwrite(false), strictness(CodecStrictness::EXPERIMENTAL) {}
    } m_globalSettings;

    struct FFmpegAudioSettings {
        AudioCodec codec;
        int sampleRate;
        int numChannels;

        FFmpegAudioSettings() : codec(AudioCodec::AAC), sampleRate(48000), numChannels(2) {}
    } m_audioSettings;

    struct FFmpegVideoSettings {
        VideoCodec codec;

        FFmpegVideoSettings() : codec(VideoCodec::H264) {}
    } m_videoSettings;

    // Update Global, Audio, or Video Settings
    void updateSettings(const struct FFmpegGlobalSettings& globalSettings = FFmpegGlobalSettings(),
                        const struct FFmpegAudioSettings& audioSettings = FFmpegAudioSettings(),
                        const struct FFmpegVideoSettings& videoSettings = FFmpegVideoSettings());

   private:
    std::unordered_map<AudioCodec, std::string> m_audioCodecAsString;
    std::unordered_map<VideoCodec, std::string> m_videoCodecAsString;
    std::unordered_map<CodecStrictness, std::string> m_codecStrictnessAsString;
};

}  // namespace MediaProcessor

#endif  // FFMPEGCONFIGMANAGER_H
