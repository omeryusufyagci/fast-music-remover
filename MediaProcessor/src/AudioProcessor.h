#ifndef AUDIOPROCESSOR_H
#define AUDIOPROCESSOR_H

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace MediaProcessor {

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
    double m_totalDuration;

    bool extractAudio();
    bool splitAudioIntoChunks();
    bool generateChunkFile(int index, const double startTime, const double duration,
                           const fs::path &ffmpegPath);
    bool filterChunks();
    bool mergeChunks();
    bool invokeDeepFilter(fs::path chunkPath);
    bool invokeDeepFilterFFI(fs::path chunkPath);

    std::string buildFilterComplex() const;

    double getAudioDuration(const fs::path &audioPath);

    void populateChunkDurations(std::vector<double> &startTimes,
                                std::vector<double> &durations) const;
};

}  // namespace MediaProcessor

#endif  // AUDIOPROCESSOR_H
