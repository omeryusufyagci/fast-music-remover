#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <string>

namespace MediaProcessor {

class VideoProcessor {
   public:
    VideoProcessor(const std::string &videoPath, const std::string &audioPath,
                   const std::string &outputPath);

    bool mergeMedia();

   private:
    std::string m_videoPath;
    std::string m_audioPath;
    std::string m_outputPath;
    std::string m_ffmpegPath;
};

}  // namespace MediaProcessor

#endif  // VIDEOPROCESSOR_H
