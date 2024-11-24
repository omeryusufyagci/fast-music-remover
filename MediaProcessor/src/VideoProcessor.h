#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <filesystem>

namespace fs = std::filesystem;

namespace MediaProcessor {

/**
 * @brief Handles video processing tasks, such as merging audio and video files.
 */
class VideoProcessor {
   public:
    /**
     * @brief Initializes the VideoProcessor with paths for the video, audio, and output.
     */
    VideoProcessor(const fs::path& videoPath, const fs::path& audioPath,
                   const fs::path& outputPath);

    /**
     * @brief Merges the audio and video files into a single output file.
     *
     * @return true if the merge is successful, false otherwise.
     */
    bool mergeMedia();

   private:
    fs::path m_videoPath;
    fs::path m_audioPath;
    fs::path m_outputPath;
    fs::path m_ffmpegPath;
};

}  // namespace MediaProcessor

#endif  // VIDEOPROCESSOR_H
