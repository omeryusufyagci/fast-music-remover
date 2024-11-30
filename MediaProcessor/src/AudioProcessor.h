#ifndef AUDIOPROCESSOR_H
#define AUDIOPROCESSOR_H

#include <filesystem>
#include <string>
#include <vector>

#include "ConfigManager.h"
#include "DeepFilterNetFFI.h"

namespace fs = std::filesystem;

namespace MediaProcessor {
constexpr double DEFAULT_OVERLAP_DURATION = 0.5;

/**
 * @brief Handles audio processing tasks, such as extracting, chunking,
 *        filtering, and merging audio.
 */
class AudioProcessor {
   public:
    /**
     * @brief Initializes the AudioProcessor with input and output paths.
     */
    AudioProcessor(const fs::path& inputVideoPath, const fs::path& outputAudioPath);

    /**
     * @brief Isolates vocals from the input video by processing the audio.
     *
     * The process includes extracting audio, chunking it, filtering each chunk,
     * and merging the processed chunks back together.
     *
     * @return true if the operation completes successfully, false otherwise.
     */
    bool isolateVocals();

   private:
    fs::path m_inputVideoPath;
    fs::path m_outputAudioPath;
    fs::path m_outputPath;
    fs::path m_chunksPath;
    fs::path m_processedChunksPath;
    std::vector<fs::path> m_chunkColPath;
    std::vector<fs::path> m_processedChunkColPath;

    int m_numChunks;

    double m_totalDuration;
    double m_overlapDuration;
    float m_filterAttenuationLimit;

    ConfigManager& m_configManager;

    bool extractAudio();
    bool splitAudioIntoChunks();
    bool generateChunkFile(int index, const double startTime, const double duration,
                           const fs::path& ffmpegPath);
    bool filterChunks();
    bool mergeChunks();
    bool invokeDeepFilter(fs::path chunkPath);

    bool invokeDeepFilterFFI(fs::path chunkPath, DFState* df_state, std::vector<float>& inputBuffer,
                             std::vector<float>& outputBuffer);

    std::string buildFilterComplex() const;

    void populateChunkDurations(std::vector<double>& startTimes,
                                std::vector<double>& durations) const;
};

}  // namespace MediaProcessor

#endif  // AUDIOPROCESSOR_H
