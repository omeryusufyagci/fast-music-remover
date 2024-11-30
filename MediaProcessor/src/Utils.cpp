#include "Utils.h"

#include <array>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string_view>

#include "CommandBuilder.h"
#include "FFmpegSettingsManager.h"

namespace MediaProcessor::Utils {

bool runCommand(const std::string& command) {
    std::array<char, 128> buffer;
    std::string result;
    std::string fullCommand = command + " 2>&1";  // Redirect stderr to stdout
    FILE* pipe = popen(fullCommand.c_str(), "r");
    if (!pipe) {
        std::cerr << "Error: Failed to run command: " << command << std::endl;
        return false;
    }

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }

    int returnCode = pclose(pipe);
    if (returnCode != 0) {
        std::cerr << "Command failed with return code " << returnCode << ":" << std::endl;
        std::cerr << result << std::endl;
        return false;
    }
    return true;
}

std::optional<std::string> runCommand(const std::string& command, bool captureOutput) {
    if (!captureOutput) {
        return runCommand(command) ? std::optional<std::string>{} : std::nullopt;
    }

    std::array<char, 128> buffer;
    std::string result;
    auto pipe = std::unique_ptr<FILE, decltype(&pclose)>(popen(command.c_str(), "r"), pclose);

    if (!pipe) {
        std::cerr << "Error: Failed to run command: " << command << std::endl;
        return std::nullopt;
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    int returnCode = pclose(pipe.release());
    if (returnCode != 0) {
        std::cerr << "Command failed with return code " << returnCode << ":" << std::endl;
        std::cerr << result << std::endl;
        return std::nullopt;
    }

    return result.empty() ? std::nullopt : std::make_optional(result);
}

std::pair<std::filesystem::path, std::filesystem::path> prepareOutputPaths(
    const std::filesystem::path& videoPath) {
    std::string baseFilename = videoPath.stem().string();

    std::filesystem::path outputDir = videoPath.parent_path();

    std::filesystem::path vocalsPath = outputDir / (baseFilename + "_isolated_audio.wav");
    std::filesystem::path processedVideoPath = outputDir / (baseFilename + "_processed_video.mp4");

    return {vocalsPath, processedVideoPath};
}

fs::path prepareAudioOutputPath(const fs::path& inputPath) {
    return inputPath.parent_path() / (inputPath.stem().string() + "_processed.wav");
}

bool ensureDirectoryExists(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        std::cout << "Output directory does not exist, creating it: " << path << std::endl;
        std::filesystem::create_directories(path);
        return true;
    }
    return false;
}

bool removeFileIfExists(const std::filesystem::path& filePath) {
    if (std::filesystem::exists(filePath)) {
        std::cout << "File already exists, removing it: " << filePath << std::endl;
        std::filesystem::remove(filePath);
        return true;
    }
    return false;
}

bool containsWhitespace(const std::string& str) {
    return str.find(' ') != std::string::npos;
}

std::string trimTrailingSpace(const std::string& str) {
    if (str.empty() || str.back() != ' ') {
        return str;
    }
    return str.substr(0, str.size() - 1);
}

double getMediaDuration(const fs::path& mediaPath) {
    // Prepare ffprobe command
    CommandBuilder cmd;
    cmd.addArgument("ffprobe");
    cmd.addFlag("-v", "error");
    cmd.addFlag("-show_entries", "format=duration");
    cmd.addFlag("-of", "default=noprint_wrappers=1:nokey=1");
    cmd.addArgument(mediaPath.string());

    FILE* pipe = popen(cmd.build().c_str(), "r");
    if (!pipe) {
        std::cerr << "Error: Failed to run ffprobe to get media duration." << std::endl;
        return -1;
    }

    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof buffer, pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);

    try {
        return std::stod(result);
    } catch (const std::exception& e) {
        std::cerr << "Error: Could not parse media duration." << std::endl;
        return -1;
    }
}

template <typename T>
std::string enumToString(const T& value, const std::unordered_map<T, std::string>& valueMap) {
    auto it = valueMap.find(value);
    return (it != valueMap.end()) ? it->second : "unknown";
}

// Explicit instantiations ensure the compiler generates the template for a type
template std::string enumToString<AudioCodec>(
    const AudioCodec& codec, const std::unordered_map<AudioCodec, std::string>& codecMap);
template std::string enumToString<VideoCodec>(
    const VideoCodec& codec, const std::unordered_map<VideoCodec, std::string>& codecMap);

}  // namespace MediaProcessor::Utils
