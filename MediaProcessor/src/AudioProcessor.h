#ifndef AUDIOPROCESSOR_H
#define AUDIOPROCESSOR_H

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace MediaProcessor {

constexpr int DEFAULT_NUM_CHUNKS = 6;
constexpr double DEFAULT_OVERLAP_DURATION = 0.5;

class AudioProcessor {
   public:
    AudioProcessor(const fs::path &inputVideoPath, const fs::path &outputAudioPath);

    bool isolateVocals();

   private:
    fs::path m_inputVideoPath;
    fs::path m_outputAudioPath;

    int m_numChunks;
    double m_overlapDuration;

    fs::path m_outputDir;
    fs::path m_chunksDir;
    fs::path m_processedChunksDir;
    std::vector<fs::path> m_chunkPaths;
    std::vector<fs::path> m_processedChunkPaths;
    std::vector<double> m_startTimes;
    std::vector<double> m_durations;
    double m_totalDuration;

    bool extractAudio();
    bool chunkAudio();
    bool filterChunks();
    bool mergeChunks();

    double getAudioDuration(const fs::path &audioPath);
};

}  // namespace MediaProcessor

#endif  // AUDIOPROCESSOR_H
