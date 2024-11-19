#ifndef MEDIAPROCESSOR_H
#define MEDIAPROCESSOR_H

#include <filesystem>

namespace MediaProcessor {

enum class MediaType { Audio, Video, Unsupported };

class MediaProcessor {
   public:
    explicit MediaProcessor(const std::filesystem::path& mediaPath);

    /**
     * @brief Processes a media file (audio or video) to isolate vocals.
     *
     * Processes the media file located at m_mediaPath. Processing is dynamically
     * adjusted by detecting file type, i.e. audio or video.
     *
     * @return true if processing was successful, false otherwise.
     */
    bool process();

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
};

}  // namespace MediaProcessor

#endif  // MEDIAPROCESSOR_H
