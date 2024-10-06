#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <filesystem>

namespace fs = std::filesystem;

namespace MediaProcessor {

class VideoProcessor {
   public:
    VideoProcessor(const fs::path &videoPath, const fs::path &audioPath,
                   const fs::path &outputPath);

    bool mergeMedia();

   private:
    fs::path m_videoPath;
    fs::path m_audioPath;
    fs::path m_outputPath;
    fs::path m_ffmpegPath;
};

}  // namespace MediaProcessor

#endif  // VIDEOPROCESSOR_H
