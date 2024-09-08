#include "VideoProcessing.h"
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <string>

bool VideoProcessing::mergeAudioVideo(const std::string &videoPath, const std::string &audioPath, const std::string &outputPath) {
    
    // FIXME: fix hardcoded paths that were used for testing
    std::string absoluteVideoPath = std::filesystem::absolute(videoPath).string();
    std::string absoluteAudioPath = std::filesystem::absolute(audioPath).string();
    std::string absoluteOutputPath = std::filesystem::absolute(outputPath).string();

    std::cout << "Merging video and audio..." << std::endl;

    std::string ffmpegCommand = "ffmpeg -i \"" + absoluteVideoPath + "\" -i \"" + absoluteAudioPath + "\" -c:v copy -map 0:v:0 -map 1:a:0 -shortest \"" + absoluteOutputPath + "\"";

    std::cout << "Running FFmpeg command: " << ffmpegCommand << std::endl;

    int result = system(ffmpegCommand.c_str());
    if (result != 0) {
        std::cerr << "Error: Failed to merge audio and video using FFmpeg." << std::endl;
        return false;
    }

    std::cout << "Merging completed successfully." << std::endl;

    return true;
}
