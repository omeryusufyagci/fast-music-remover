#ifndef AUDIOPROCESSOR_H
#define AUDIOPROCESSOR_H

#include <string>
#include <vector>

namespace MediaProcessor {

constexpr int DEFAULT_NUM_CHUNKS = 6;
constexpr double DEFAULT_OVERLAP_DURATION = 0.5;

class AudioProcessor {
   public:
    AudioProcessor(const std::string &inputVideoPath, const std::string &outputAudioPath);

    bool isolateVocals();

   private:
    std::string m_inputVideoPath;
    std::string m_outputAudioPath;

    int m_numChunks;
    double m_overlapDuration;

    std::string m_outputDir;
    std::string m_chunksDir;
    std::string m_processedChunksDir;
    std::vector<std::string> m_chunkPaths;
    std::vector<std::string> m_processedChunkPaths;
    std::vector<double> m_startTimes;
    std::vector<double> m_durations;
    double m_totalDuration;

    bool extractAudio();
    bool chunkAudio();
    bool filterChunks();
    bool mergeChunks();

    double getAudioDuration(const std::string &audioPath);
};

}  // namespace MediaProcessor

#endif  // AUDIOPROCESSOR_H
