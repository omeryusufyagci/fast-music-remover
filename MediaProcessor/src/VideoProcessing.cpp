#include "VideoProcessing.h"

#include <filesystem>
#include <iostream>
#include <string>

#include "Utils.h"

bool VideoProcessing::mergeMedia(const std::string &videoPath, const std::string &audioPath,
                                 const std::string &outputPath) {
    json config = Utils::loadConfig("config.json");
    if (config.is_null()) {
        std::cerr << "Error: Could not load configuration." << std::endl;
        return false;
    }

    std::string ffmpegPath = config["ffmpeg_path"];
    std::string absoluteVideoPath = std::filesystem::absolute(videoPath).string();
    std::string absoluteAudioPath = std::filesystem::absolute(audioPath).string();
    std::string absoluteOutputPath = std::filesystem::absolute(outputPath).string();

    // Remove the output file if it already exists (avoid interactive FFmpeg prompt)
    Utils::removeFileIfExists(absoluteOutputPath);

    std::cout << "Merging video and audio..." << std::endl;

    // Prepare ffmpeg command (-y flag to overwrite the file)
    std::string ffmpegCommand =
        ffmpegPath + " -y -i \"" + absoluteVideoPath + "\" -i \"" + absoluteAudioPath +
        "\" -c:v copy -c:a aac -strict experimental -map 0:v:0 -map 1:a:0 -shortest \"" +
        absoluteOutputPath + "\"";

    std::cout << "Running FFmpeg command: " << ffmpegCommand << std::endl;
    bool success = Utils::runCommand(ffmpegCommand);

    if (!success) {
        std::cerr << "Error: Failed to merge audio and video using FFmpeg." << std::endl;
        return false;
    }

    std::cout << "Merging completed successfully." << std::endl;

    return true;
}
