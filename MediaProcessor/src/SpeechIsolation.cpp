#include "SpeechIsolation.h"
#include "Utils.h"
#include <iostream>
#include <filesystem>
#include <string>

bool SpeechIsolation::isolateSpeech(const std::string &inputVideoPath, const std::string &outputAudioPath) {

    // TODO: use a proper logger

    json config = Utils::loadConfig("config.json");
    if (config.is_null()) {
        std::cerr << "Error: Could not load configuration." << std::endl;
        return false;
    }

    std::string deepFilterPath = config["deep_filter_path"];
    std::string ffmpegPath = config["ffmpeg_path"];

    // Make sure outdir exists
    std::filesystem::path outputDir = std::filesystem::path(outputAudioPath).parent_path();
    Utils::ensureDirectoryExists(outputDir.string());

    // To avoid `can't overwrite` issues, remove output file if it exists
    Utils::removeFileIfExists(outputAudioPath);

    std::cout << "Input video path: " << inputVideoPath << std::endl;
    std::cout << "Output audio path: " << outputAudioPath << std::endl;

    // Extract the audio with ffmpeg
    std::string ffmpegCommand = ffmpegPath + " -i \"" + inputVideoPath + "\" -ar 48000 -ac 1 \"" + outputAudioPath + "\"";
    if (!Utils::runCommand(ffmpegCommand)) {
        std::cerr << "Error: Failed to extract and convert audio using FFmpeg." << std::endl;
        return false;
    }

    std::cout << "Audio extracted successfully to: " << outputAudioPath << std::endl;

    // Apply DeepFilterNet filtering
    std::string deepFilterCommand = deepFilterPath + " -o \"" + outputDir.string() + "\" \"" + outputAudioPath + "\"";
    if (!Utils::runCommand(deepFilterCommand)) {
        std::cerr << "Error: Failed to isolate speech with DeepFilterNet." << std::endl;
        return false;
    }

    return true;
}
