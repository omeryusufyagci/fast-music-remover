#ifndef FFMPEGCONFIGMANAGER_H
#define FFMPEGCONFIGMANAGER_H

#include <filesystem>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ConfigManager.h"
#include "Utils.h"

namespace fs = std::filesystem;

namespace MediaProcessor {

enum class AudioCodec { AAC, MP3, FLAC, OPUS, UNKNOWN };
enum class VideoCodec { H264, H265, VP8, VP9, COPY, UNKNOWN };
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

        FFmpegVideoSettings() : codec(VideoCodec::COPY) {}
    } m_videoSettings;

    // Update Global, Audio, or Video Settings
    void updateSettings(const struct FFmpegGlobalSettings& globalSettings = FFmpegGlobalSettings(),
                        const struct FFmpegAudioSettings& audioSettings = FFmpegAudioSettings(),
                        const struct FFmpegVideoSettings& videoSettings = FFmpegVideoSettings());

   private:
    std::unordered_map<AudioCodec, std::string> m_audioCodecAsString;
    std::unordered_map<VideoCodec, std::string> m_videoCodecAsString;
    std::unordered_map<CodecStrictness, std::string> m_codecStrictnessAsString;

    struct ValidAudioOptions {
        struct ValidOptionsAAC {
            inline static const int minChannels = 1;
            inline static const int maxChannels = 8;
            inline static const int minBitrate = 96000;
            inline static const int maxBitrate = 512000;
            inline static const std::unordered_set<int> sampleRates = {
                7350,  8000,  11025, 12000, 16000, 22050, 24000,
                32000, 44100, 48000, 64000, 88200, 96000};
        } AAC;

        struct ValidOptionsMP3 {
            inline static const int minChannels = 1;
            inline static const int maxChannels = 2;
            inline static const int minBitrate = 128000;
            inline static const int maxBitrate = 320000;
            inline static const std::unordered_set<int> sampleRates = {
                8000, 12000, 11025, 16000, 24000, 22050, 32000, 44100, 48000};
        } MP3;

        struct ValidOptionsFLAC {
            inline static const int minChannels = 1;
            inline static const int maxChannels = 8;
            inline static const int minSampleRate = 0;
            inline static const int maxSampleRate = 1048575;
        } FLAC;

        struct ValidOptionsOpus {
            inline static const int minChannels = 1;
            inline static const int maxChannels = 8;
            inline static const int minBitrate = 24000;
            inline static const int maxBitrate = 512000;
            inline static const std::unordered_set<int> sampleRates = {8000, 12000, 16000, 24000,
                                                                       48000};
        } Opus;

    } inline static const m_validAudioOptions;

    struct FFmpegValidSettingsAAC {
        inline static const std::pair<int, int> minMaxChannels = {1, 8};
        inline static const std::pair<int, int> minMaxBitRate = {96000, 512000};
        inline static const std::unordered_set<int> sampleRates = {
            7350,  8000,  11025, 12000, 16000, 22050, 24000,
            32000, 44100, 48000, 64000, 88200, 96000};
    } m_validSettingsAAC;

    struct FFmpegValidSettingsMP3 {
        inline static const std::pair<int, int> minMaxChannels = {1, 2};
        inline static const std::pair<int, int> minMaxBitRate = {128000, 320000};
        inline static const std::unordered_set<int> sampleRates = {
            8000, 12000, 11025, 16000, 24000, 22050, 32000, 44100, 48000};
    } m_validSettingsMP3;

    struct FFmpegValidSettingsFLAC {
        inline static const std::pair<int, int> minMaxChannels = {1, 8};
        inline static const std::pair<int, int> minMaxSampleRates = {0, 1048575};
    } m_validSettingsFLAC;

    struct FFmpegValidSettingsOpus {
        inline static const std::pair<int, int> minMaxChannels = {1, 8};
        inline static const std::pair<int, int> minMaxBitRate = {64000, 512000};
        inline static const std::unordered_set<int> sampleRates = {8000, 12000, 16000, 24000,
                                                                   48000};

    } m_validSettingsOpus;

    template <typename T>
    void validateOption(const T optionValue, const std::unordered_set<T>& validOptions);

    template <typename T>
    void validateOption(const T optionValue, const T min, const T max);

    void validateAudio();
};

}  // namespace MediaProcessor

#endif  // FFMPEGCONFIGMANAGER_H
