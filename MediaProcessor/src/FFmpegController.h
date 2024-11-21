#ifndef FFMPEGCONTROLLER_H
#define FFMPEGCONTROLLER_H

#include <vector>

#include "FFmpegCommandBuilder.h"
#include "FFmpegConfigManager.h"

namespace MediaProcessor {

/**
 * @brief An interface to orchestrate FFmpeg operations
 */
class FFmpegController {
   public:
    /**
     * @brief Constructs a FFmpegController
     *
     * This constructor instantiates FFmpegController and initializes it with
     * the given reference to a FFmpegConfigManager object.
     *
     * @param ffmpegConfig A reference to an instance of FFmpegConfigManager
     */
    FFmpegController(const FFmpegConfigManager& ffmpegConfig);

    /**
     * @brief Extracts the audio with FFmpeg
     *
     * @return True if successful, otherwise false
     */
    bool extractAudio();

    /**
     * @brief Splits the audio into chunk files
     *
     * @return A vector of the chunk file paths
     */
    std::vector<fs::path> splitMedia(int numChunks, double overlapDuration);

    /**
     * @brief Merges media chunks with FFmpeg
     *
     * @return True if successful, otherwise false
     */
    bool mergeChunks(const std::vector<fs::path>& processedChunkCol, double overlapDuration);

    /**
     * @brief Merges media files with FFmpeg
     *
     * @return True if successful, otherwise false
     */
    bool mergeMedia(const fs::path& videoPath, const fs::path& audioPath);

   private:
    const FFmpegConfigManager& m_ffmpegConfig;
    FFmpegCommandBuilder m_ffmpegCmdBuilder;

    void populateChunkDurations(std::vector<double>& startTimes, std::vector<double>& durations,
                                int numChunks, double overlapDuration);
    bool generateChunkFile(const int chunkIndex, const double startTime, const double duration,
                           std::vector<fs::path>& chunkPathCol);
};

}  // namespace MediaProcessor

#endif /* FFMPEGCONTROLLER_H */
