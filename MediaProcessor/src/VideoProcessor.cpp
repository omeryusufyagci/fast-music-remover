#include "VideoProcessor.h"

#include <filesystem>
#include <iostream>

#include "ConfigManager.h"
#include "Utils.h"

namespace MediaProcessor {

VideoProcessor::VideoProcessor(const std::string &videoPath, const std::string &audioPath,
                               const std::string &outputPath)
    : m_videoPath(std::filesystem::absolute(videoPath).string()),
      m_audioPath(std::filesystem::absolute(audioPath).string()),
      m_outputPath(std::filesystem::absolute(outputPath).string()),
      m_ffmpegPath(ConfigManager::getInstance().getFFmpegPath()) {}

bool VideoProcessor::mergeMedia() {
    // Remove the output file if it already exists to avoid interactive FFmpeg prompt
    Utils::removeFileIfExists(m_outputPath);

    std::cout << "Merging video and audio..." << std::endl;

    // Prepare FFmpeg command (-y flag to overwrite the file)
    std::string ffmpegCommand =
        m_ffmpegPath + " -y -i \"" + m_videoPath + "\" -i \"" + m_audioPath +
        "\" -c:v copy -c:a aac -strict experimental -map 0:v:0 -map 1:a:0 -shortest \"" +
        m_outputPath + "\"";

    std::cout << "Running FFmpeg command: " << ffmpegCommand << std::endl;
    bool success = Utils::runCommand(ffmpegCommand);

    if (!success) {
        std::cerr << "Error: Failed to merge audio and video using FFmpeg." << std::endl;
        return false;
    }

    std::cout << "Merging completed successfully." << std::endl;

    return true;
}

}  // namespace MediaProcessor
