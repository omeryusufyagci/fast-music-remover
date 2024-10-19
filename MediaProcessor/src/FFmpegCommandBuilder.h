#ifndef FFMPEGCOMMANDBUILDER_H
#define FFMPEGCOMMANDBUILDER_H

#include <filesystem>

#include "CommandBuilder.h"
#include "FFmpegSettingsManager.h"

namespace fs = std::filesystem;

namespace MediaProcessor {

/**
 * @brief Builds individual FFmpeg commands
 */
class FFmpegCommandBuilder : public CommandBuilder {
   public:
    /**
     * @brief Initializes the FFmpegCommandBuilder and adds the FFmpeg binary
     *        path as the first argument
     *
     * @param ffmpegSettings A reference to an instance of FFmpegSettingsManager
     */
    FFmpegCommandBuilder(FFmpegSettingsManager& ffmpegSettings);

    /**
     * @brief Adds overwrite flag
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegCommandBuilder& addOverwrite();

    /**
     * @brief Adds input flag and input file path
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegCommandBuilder& addInputFile(const fs::path& inputFile);

    /**
     * @brief Adds output file path
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegCommandBuilder& addOutputFile(const fs::path& outputFile);

    /**
     * @brief Adds audio codec flag and codec type
     *
     * @return A reference to the current object for method chaining
     *
     * @throws std::runtime_error If the audio codec is unknown
     */
    FFmpegCommandBuilder& addAudioCodec();

    /**
     * @brief Adds audio sample rate flag and sample rate
     *
     * @return A reference to the current object for method chaining
     *
     * @throws std::runtime_error If the rate is too low (rate <= 40000)
     */
    FFmpegCommandBuilder& addAudioSampleRate();

    /**
     * @brief Adds audio channels flag and number of channels
     *
     * @return A reference to the current object for method chaining
     *
     * @throws std::runtime_error If number of channels is less than 1
     */
    FFmpegCommandBuilder& addAudioChannels();

    /**
     * @brief Adds video codec flag and codec type
     *
     * @return A reference to the current object for method chaining
     *
     * @throws std::runtime_error If the video codec is unknown
     */
    FFmpegCommandBuilder& addVideoCodec();

    /**
     * @brief Constructs the command string from the added arguments and flags
     *
     * @return The FFmpeg command as a string
     *
     * @throws std::runtime_error If input or output file path is missing
     */
    std::string build() const override;

   private:
    FFmpegSettingsManager& m_ffmpegSettings;

    fs::path m_ffmpegPath;
    fs::path m_inputFile;
    fs::path m_outputFile;
};

}  // namespace MediaProcessor

#endif /* FFMPEGCOMMANDBUILDER_H */
