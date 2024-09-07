#include "SpeechIsolation.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <filesystem>

bool SpeechIsolation::isolateSpeech(const std::string &inputVideoPath, const std::string &outputAudioPath) {
    
    // TODO: use a proper logger

    // Make sure output file is a file and not a directory
    // TODO: check if this is still needed; could be related with issues earlier
    std::filesystem::path outputDir = std::filesystem::path(outputAudioPath).parent_path();

    // Make sure outdir exists
    if (!std::filesystem::exists(outputDir)) {
        std::cout << "Output directory does not exist, creating it: " << outputDir << std::endl;
        std::filesystem::create_directories(outputDir);
    }

    // To avoid `can't overwrite` issues, remove an identical output file if it exists
    if (std::filesystem::exists(outputAudioPath)) {
        std::cout << "Output audio file already exists, removing it: " << outputAudioPath << std::endl;
        std::filesystem::remove(outputAudioPath);
    }

    std::cout << "Input video path: " << inputVideoPath << std::endl;
    std::cout << "Output audio path: " << outputAudioPath << std::endl;

    // Extract the audio with ffmpeg (for testing hardcoded command..)
    // TODO: encapsulate parametrically 
    std::string ffmpegCommand = "ffmpeg -i \"" + inputVideoPath + "\" -ar 48000 -ac 1 \"" + outputAudioPath + "\"";

    std::cout << "Running FFmpeg command: " << ffmpegCommand << std::endl;

    int ffmpegResult = system(ffmpegCommand.c_str());
    if (ffmpegResult != 0) {
        std::cerr << "Error: Failed to extract and convert audio using FFmpeg." << std::endl;
        return false;
    }

    std::cout << "Audio extracted successfully to: " << outputAudioPath << std::endl;

    // TODO: fix abs path of deepfilternet binary
    // Run the filter on the audio file (which will filter out noise and music)
    std::string deepFilterPath = "/home/oyagci/Documents/ws_fast_music_remover/media_processor/res/deep-filter-0.5.6-x86_64-unknown-linux-musl";
    std::string deepFilterCommand = deepFilterPath + " -o \"" + outputDir.string() + "\" \"" + outputAudioPath + "\"";

    std::cout << "Running DeepFilterNet command: " << deepFilterCommand << std::endl;

    int deepFilterResult = system(deepFilterCommand.c_str());
    if (deepFilterResult != 0) {
        std::cerr << "Error: Failed to isolate speech with DeepFilterNet." << std::endl;
        return false;
    }

    return true;
}
