#include "Engine.h"

#include <iostream>

#include "AudioProcessor.h"
#include "ConfigManager.h"
#include "Utils.h"
#include "VideoProcessor.h"

namespace MediaProcessor {

Engine::Engine(const std::filesystem::path& mediaPath)
    : m_mediaPath(std::filesystem::absolute(mediaPath)) {}

bool Engine::processMedia() {
    ConfigManager& configManager = ConfigManager::getInstance();
    if (!configManager.loadConfig("config.json")) {
        std::cerr << "Error: Could not load configuration." << std::endl;
        return false;
    }

    MediaType mediaType;
    mediaType = TRY(getMediaType());

    switch (mediaType) {
        case MediaType::Audio:
            return processAudio();
        case MediaType::Video:
            return processVideo();
        default:
            std::cerr << "Unsupported file type." << std::endl;
            return false;
    }
}

bool Engine::processAudio() {
    AudioProcessor audioProcessor(m_mediaPath, Utils::prepareAudioOutputPath(m_mediaPath));
    if (!audioProcessor.isolateVocals()) {
        std::cerr << "Failed to process audio." << std::endl;
        return false;
    }
    std::cout << "Audio processed successfully: " << Utils::prepareAudioOutputPath(m_mediaPath)
              << std::endl;
    return true;
}

bool Engine::processVideo() {
    auto [extractedVocalsPath, processedMediaPath] = Utils::prepareOutputPaths(m_mediaPath);
    AudioProcessor audioProcessor(m_mediaPath, extractedVocalsPath);

    if (!audioProcessor.isolateVocals()) {
        std::cerr << "Failed to extract vocals from video." << std::endl;
        return false;
    }

    VideoProcessor videoProcessor(m_mediaPath, extractedVocalsPath, processedMediaPath);
    if (!videoProcessor.mergeMedia()) {
        std::cerr << "Failed to merge audio and video." << std::endl;
        return false;
    }

    std::cout << "Video processed successfully: " << processedMediaPath << std::endl;
    return true;
}

MediaType Engine::getMediaType() const {
    const std::string command =
        "ffprobe -loglevel error -show_entries stream -of json \"" + m_mediaPath.string() + "\"";

    std::optional<std::string> output = Utils::runCommand(command, true);
    if (!output || output->empty()) {
        throw std::runtime_error("Failed to detect media type.");
    }

    nlohmann::json streamData = nlohmann::json::parse(*output);

    bool containsAudioStream;
    for (const auto& stream : streamData["streams"]) {
        if (isActualVideoStream(stream)) {
            return MediaType::Video;
        } 
        if (hasValidAudioStream(stream)) {
            containsAudioStream = true;
        }
    }

    if (containsAudioStream) {
        return MediaType::Audio;
    }

    return MediaType::Unsupported;
}

bool Engine::isActualVideoStream(const nlohmann::json& stream) const {
    if (stream["codec_type"] != "video") {
        return false;
    }
    if(!stream.contains("avg_frame_rate")) {
        return false;
    }
    if (hasZeroFrameRate(stream["avg_frame_rate"])) {
        return false;
    }

    return true;
}

bool Engine::hasValidAudioStream(const nlohmann::json& stream) const {
    return (stream["codec_type"] == "audio");
}

bool Engine::hasZeroFrameRate(const std::string& frameRate) const {
    return (frameRate == "0/0");
}

}  // namespace MediaProcessor
