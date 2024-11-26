#ifndef FFMPEGCONFIGMANAGER_H
#define FFMPEGCONFIGMANAGER_H

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "ConfigManager.h"

namespace fs = std::filesystem;

namespace MediaProcessor {
constexpr double DEFAULT_OVERLAP_DURATION = 0.5;

enum class AudioCodec { AAC, MP3, FLAC, OPUS, COPY, UNKNOWN };
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
    struct FFmpegGlobalSettings {
        bool overwrite = false;
        CodecStrictness strictness = CodecStrictness::EXPERIMENTAL;
        fs::path inputFilePath;
        fs::path outputFilePath;
    } m_globalSettings;

    struct FFmpegAudioSettings {
        AudioCodec codec = AudioCodec::AAC;
        fs::path chunksPath;
        std::vector<fs::path> processedChunkCol;
        int numChunks;
        int sampleRate = 48000;
        int numChannels = 2;
        double overlapDuration = DEFAULT_OVERLAP_DURATION;
    } m_audioSettings;

    struct FFmpegVideoSettings {
        VideoCodec codec = VideoCodec::H264;
    } m_videoSettings;

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
    void setProcessedChunkCol(const std::vector<fs::path> processedChunkCol);
    void setNumChunks(int numChunks);
    void setAudioSampleRate(int sampleRate);
    void setAudioChannels(int channels);
    void setOverlapDuration(double overlapDuration);

    // Audio Getters
    AudioCodec getAudioCodec() const;
    std::vector<fs::path> getProcessedChunkCol() const;
    int getNumChunks() const;
    int getAudioSampleRate() const;
    int getAudioChannels() const;
    double getOverlapDuration() const;

    // Video Setters
    void setVideoCodec(VideoCodec codec);

    // Video Getters
    VideoCodec getVideoCodec() const;

    // Value Map Getters
    const std::unordered_map<AudioCodec, std::string>& getAudioCodecAsString() const;
    const std::unordered_map<VideoCodec, std::string>& getVideoCodecAsString() const;
    const std::unordered_map<CodecStrictness, std::string>& getCodecStrictnessAsString() const;

    // Update Global, Audio, or Video Settings
    void updateSettings(const struct FFmpegGlobalSettings& globalSettings,
                        const struct FFmpegAudioSettings& audioSettings,
                        const struct FFmpegVideoSettings& videoSettings);

   private:
    std::unordered_map<AudioCodec, std::string> m_audioCodecAsString;
    std::unordered_map<VideoCodec, std::string> m_videoCodecAsString;
    std::unordered_map<CodecStrictness, std::string> m_codecStrictnessAsString;
};

}  // namespace MediaProcessor

#endif  // FFMPEGCONFIGMANAGER_H
