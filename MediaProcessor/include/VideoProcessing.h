#ifndef VIDEO_PROCESSING_H
#define VIDEO_PROCESSING_H

#include <string>

class VideoProcessing {
public:
    static bool mergeAudioVideo(const std::string &videoPath, const std::string &audioPath, const std::string &outputPath);
};

#endif // VIDEO_PROCESSING_H
