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
     * @brief Generates the chunk file with FFmpeg
     *
     * @return True if successful, otherwise false
     */
    bool generateChunkFile();

        /**
     * @brief Merges media chunks with FFmpeg
     *
     * @return True if successful, otherwise false
     */
    bool mergeChunks();

    /**
     * @brief Merges media files with FFmpeg
     *
     * @return True if successful, otherwise false
     */
    bool mergeMedia(const fs::path& videoPath, const fs::path& audioPath);

   private:
    const FFmpegConfigManager& m_ffmpegConfig;
    FFmpegCommandBuilder m_ffmpegCmdBuilder;
};

}  // namespace MediaProcessor

#endif /* FFMPEGCONTROLLER_H */
