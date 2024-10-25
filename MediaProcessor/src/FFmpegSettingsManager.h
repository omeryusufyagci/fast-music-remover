#ifndef FFMPEGSETTINGSMANAGER_H
#define FFMPEGSETTINGSMANAGER_H

#include <string>
#include <unordered_map>

namespace MediaProcessor {

enum class AudioCodec { AAC, MP3, FLAC, OPUS, UNKNOWN };
enum class VideoCodec { H264, H265, VP8, VP9, UNKNOWN };

/**
 * @brief Manages settings for FFmpeg-specific global, audio, and video settings.
 *
 * Provides an interface for setting and retrieving configuration options,
 * including file paths, codecs, and other processing parameters.
 */
class FFmpegSettingsManager {
   public:
    FFmpegSettingsManager();

    // Global Setters
    void setOverwrite(bool overwrite);

    // Global Getters
    bool getOverwrite() const;

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
