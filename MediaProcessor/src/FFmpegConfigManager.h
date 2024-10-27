#ifndef FFMPEGCONFIGMANAGER_H
#define FFMPEGCONFIGMANAGER_H

#include <filesystem>
#include <string>
#include <unordered_map>

namespace fs = std::filesystem;

namespace MediaProcessor {

enum class AudioCodec { AAC, MP3, FLAC, OPUS, UNKNOWN };
enum class VideoCodec { H264, H265, VP8, VP9, UNKNOWN };

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
    void setInputFilePath(const fs::path inputFilePath);
    void setOutputFilePath(const fs::path outputFilePath);

    // Global Getters
    bool getOverwrite() const;
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
    std::unordered_map<AudioCodec, std::string>& getAudioCodecAsString();
    std::unordered_map<VideoCodec, std::string>& getVideoCodecAsString();

   private:
    std::unordered_map<AudioCodec, std::string> m_audioCodecAsString;
    std::unordered_map<VideoCodec, std::string> m_videoCodecAsString;

    struct FFmpegGlobalSettings {
        bool overwrite = false;
        fs::path inputFilePath;
        fs::path outputFilePath;
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

#endif  // FFMPEGCONFIGMANAGER_H
