#include "VideoProcessor.h"

#include <iostream>

#include "CommandBuilder.h"
#include "ConfigManager.h"
#include "Utils.h"

namespace fs = std::filesystem;

namespace MediaProcessor {

VideoProcessor::VideoProcessor(const fs::path& videoPath, const fs::path& audioPath,
                               const fs::path& outputPath)
    : m_videoPath(fs::absolute(videoPath)),
      m_audioPath(fs::absolute(audioPath)),
      m_outputPath(fs::absolute(outputPath)),
      m_ffmpegPath(ConfigManager::getInstance().getFFmpegPath()) {}

bool VideoProcessor::mergeMedia() {
    Utils::removeFileIfExists(m_outputPath);  // to avoid interactive ffmpeg prompt

    std::cout << "Merging video and audio..." << std::endl;

    // Prepare FFmpeg command
    CommandBuilder cmd;
    cmd.addArgument(m_ffmpegPath.string());
    cmd.addFlag("-y");  // overwrite enabled
    cmd.addFlag("-i", m_videoPath.string());
    cmd.addFlag("-i", m_audioPath.string());
    cmd.addFlag("-c:v", "copy");
    cmd.addFlag("-c:a", "aac");
    cmd.addFlag("-strict", "experimental");
    cmd.addFlag("-map", "0:v:0");
    cmd.addFlag("-map", "1:a:0");
    cmd.addFlag("-shortest");
    cmd.addArgument(m_outputPath.string());

    std::string ffmpegCommand = cmd.build();

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
