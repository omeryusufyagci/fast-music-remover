#ifndef FFMPEGCONTROLLERBUILDER_H
#define FFMPEGCONTROLLERBUILDER_H

#include <filesystem>
#include <string>
#include <vector>

#include "CommandBuilder.h"
#include "ConfigManager.h"

namespace fs = std::filesystem;

namespace MediaProcessor {

struct FFmpegGlobalConfig {
    fs::path m_ffmpegPath;
    bool m_overwrite;
    bool m_useShortest;
    std::string m_strictness;

    FFmpegGlobalConfig()
        : m_ffmpegPath(ConfigManager::getInstance().getFFmpegPath()),
          m_overwrite(true),
          m_useShortest(true),
          m_strictness("experimental") {}
};

struct FFmpegAudioConfig {
    unsigned m_sampleRate;
    unsigned m_numChannels;
    std::string m_mapping;
    std::string m_codec;

    FFmpegAudioConfig()
        : m_sampleRate(48000),
          m_numChannels(1),
          m_mapping("1:a:0"),
          m_codec("pcm_s16le") {}
};

struct FFmpegVideoConfig {
    std::string m_codec;
    std::string m_mapping;

    FFmpegVideoConfig() : m_codec("copy"), m_mapping("0:v:0") {}
};

class FFmpegControllerBuilder {
   public:
    FFmpegControllerBuilder();

    FFmpegControllerBuilder(FFmpegGlobalConfig &globalConfig, FFmpegAudioConfig &audioConfig,
                            FFmpegVideoConfig &videoConfig);

    // Set new path for the FFmpeg binary
    void setFFmpegPath(fs::path path);

    // Allow overwriting destination files
    void setOverwrite(bool overwrite);

    // Set FFmpeg to finish encoding when the shortest stream ends
    void setShortest(bool shortest);

    // Set strictness to standards adherence - ex. "experimental"
    void setStrictness(std::string &mode);

    // Get path for FFmpeg binary
    fs::path getFFmpegPath();

    // Get whether overwrite is allowed
    bool getOverwrite();

    // Get whether "-shortest" flag is set
    bool getShortest();

    // Get current strictness
    std::string getStrictness();

    // Set audio sample rate - ex. 48000
    void setAudioSampleRate(unsigned rate);

    // Set number of audio channels - ex. 2
    void setNumAudioChannels(unsigned channels);

    // Set audio mapping
    void setAudioMapping(std::string &map);

    // Set audio codec - ex. "aac"
    void setAudioCodec(std::string &audioCodec);

    // Get audio sample rate
    unsigned getAudioSampleRate();

    // Get number of audio channels
    unsigned getNumAudioChannels();

    // Get audio mapping
    std::string getAudioMapping();

    // Get audio codec
    std::string getAudioCodec();

    // Set video codec - ex. "copy"
    void setVideoCodec(std::string &videoCodec);

    // Set video mapping
    void setVideoMapping(std::string &map);

    // Set video codec - ex. "copy"
    std::string getVideoCodec();

    // Set video mapping
    std::string getVideoMapping();

   private:
    FFmpegGlobalConfig m_ffmpegGlobalConfig;
    FFmpegAudioConfig m_ffmpegAudioConfig;
    FFmpegVideoConfig m_ffmpegVideoConfig;
};

}  // namespace MediaProcessor

#endif /* FFMPEGCONTROLLERBUILDER_H */
