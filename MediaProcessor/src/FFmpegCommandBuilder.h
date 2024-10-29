#ifndef FFMPEGCOMMANDBUILDER_H
#define FFMPEGCOMMANDBUILDER_H

#include <filesystem>
#include <sstream>

#include "CommandBuilder.h"
#include "FFmpegConfigManager.h"

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
     * @param ffmpegConfig A reference to an instance of FFmpegConfigManager
     */
    FFmpegCommandBuilder(const FFmpegConfigManager& ffmpegConfig);

    /**
     * @brief Adds overwrite flag
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegCommandBuilder& addOverwrite();

    /**
     * @brief Adds input flag and the input file path from FFmpegConfigManager
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegCommandBuilder& addInputFile();

    /**
     * @brief Adds input flag and the given input file path
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegCommandBuilder& addInputFile(const fs::path& inputFilePath);
    /**
     * @brief Adds output file path
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegCommandBuilder& addOutputFile();

    /**
     * @brief Adds audio codec flag and codec type
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegCommandBuilder& addAudioCodec();

    /**
     * @brief Adds audio sample rate flag and sample rate
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegCommandBuilder& addAudioSampleRate();

    /**
     * @brief Adds audio channels flag and number of channels
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegCommandBuilder& addAudioChannels();

    /**
     * @brief Adds position flag and the given start time
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegCommandBuilder& addStartTime(const std::ostringstream& startTime);

    /**
     * @brief Adds duration flag and the given duration
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegCommandBuilder& addDuration(const std::ostringstream& duration);
    /**

     * @brief Adds video codec flag and codec type
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegCommandBuilder& addVideoCodec();

    /**
     * @brief Adds the chunk path
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegCommandBuilder& addChunkPath();

    /**
     * @brief Adds strictness flag with "experimental" option
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegCommandBuilder& addStrictExperimental();

    /**
     * @brief Adds the stream with the given mapping
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegCommandBuilder& addStream(const std::string map);

    /**
     * @brief Adds the -shortest flag
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegCommandBuilder& addShortest();

    /**
     * @brief Adds the -shortest flag
     *
     * @return A reference to the current object for method chaining
     */
    FFmpegCommandBuilder& addFilterComplex(const std::vector<fs::path> processedChunkCol,
                                           double overlapDuration);

   private:
    const FFmpegConfigManager& m_ffmpegConfig;

    std::string buildCrossfade(int chunkColSize, double overlapDuration) const;
    std::string buildFilterComplex(const std::vector<fs::path> processedChunkCol,
                                   double overlapDuration) const;
};

}  // namespace MediaProcessor

#endif /* FFMPEGCOMMANDBUILDER_H */
