#ifndef ENGINE_H
#define ENGINE_H

#include <filesystem>
#include <nlohmann/json.hpp>

namespace MediaProcessor {

enum class MediaType { Audio, Video, Unsupported };

/**
 * @brief Media processing engine that supports audio and video files.
 */
class Engine {
   public:
    explicit Engine(const std::filesystem::path& mediaPath);

    /**
     * @brief Processes a media file (audio or video) to isolate vocals.
     *
     * Processes the media file located at m_mediaPath.
     * Processing pipeline is selected dynamically by determining media type.
     *
     * @return true if processing was successful, false otherwise.
     */
    bool processMedia();

   private:
    std::filesystem::path m_mediaPath;

    /**
     * @brief Processes an audio file.
     *
     * @return true if processing was successful, false otherwise.
     */
    bool processAudio();

    /**
     * @brief Processes a video file by extracting and isolating vocals and merges back to source.
     *
     * @return true if processing was successful, false otherwise.
     */
    bool processVideo();

    /**
     * @brief Detects the media type (audio or video) of the file located at m_mediaPath.
     *
     * @return MediaType of the file.
     *
     * @throws std::runtime_error if detection fails.
     */
    MediaType getMediaType() const;

    /**
     * @brief Checks if the provided stream is valid video stream and not static media (i.e. cover image)
     * 
     * @param stream json data of stream
     *
     * @return True if the stream is valid video stream. False otherwise.
     */
    bool hasValidVideoStream(const nlohmann::json& stream) const;

    /**
     * @brief Checks if the provided stream is valid audio stream
     * 
     * @param stream json data of stream
     *
     * @return True if the stream is valid. False otherwise.
     */
    bool hasValidAudioStream(const nlohmann::json& stream) const;

    /**
     * @brief Checks if the stream's avg_frame_rate is 0/0
     * 
     * @param frameRate the avg_frame_rate field from stream
     *
     * @return True if avg_frame_rate equals to 0/0. False otherwise.
     */
    bool hasZeroFrameRate(const std::string& frameRate) const;
};

}  // namespace MediaProcessor

#endif  // ENGINE_H
