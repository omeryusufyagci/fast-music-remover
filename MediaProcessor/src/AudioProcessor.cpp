#include "AudioProcessor.h"

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

#include "ConfigManager.h"
#include "Utils.h"

namespace MediaProcessor {

AudioProcessor::AudioProcessor(const std::string &inputVideoPath,
                               const std::string &outputAudioPath)
    : m_inputVideoPath(inputVideoPath),
      m_outputAudioPath(outputAudioPath),
      m_numChunks(DEFAULT_NUM_CHUNKS),
      m_overlapDuration(DEFAULT_OVERLAP_DURATION) {
    m_outputDir = std::filesystem::path(outputAudioPath).parent_path().string();
    m_chunksDir = (std::filesystem::path(m_outputDir) / "chunks").string();
    m_processedChunksDir = (std::filesystem::path(m_outputDir) / "processed_chunks").string();

    /*
     * TODO: 
     * * numChunks (numThreads) should be configurable, as well as the overlap (which seems much less critical?)
     * * Need an ffmpeg utils to establish an API for extaction and probing
     */
}

bool AudioProcessor::extractVocals() {
    /*
     * Extracts vocals from a video by chunking, parallel processing, and merging the audio.
     */

    ConfigManager &configManager = ConfigManager::getInstance();

    // Ensure output directory exists and remove output file if it exists
    Utils::ensureDirectoryExists(m_outputDir);
    Utils::removeFileIfExists(m_outputAudioPath);

    std::cout << "Input video path: " << m_inputVideoPath << std::endl;
    std::cout << "Output audio path: " << m_outputAudioPath << std::endl;

    if (!extractAudio()) {
        return false;
    }

    m_totalDuration = Utils::getAudioDuration(m_outputAudioPath);
    if (m_totalDuration <= 0) {
        std::cerr << "Error: Invalid audio duration." << std::endl;
        return false;
    }

    if (!chunkAudio()) {
        return false;
    }

    if (!processChunks()) {
        return false;
    }

    if (!mergeChunks()) {
        return false;
    }

    // The intermediary files can be useful for debugging, but for release we'll delete them
    std::filesystem::remove_all(m_chunksDir);
    std::filesystem::remove_all(m_processedChunksDir);

    return true;
}

bool AudioProcessor::extractAudio() {
    ConfigManager &configManager = ConfigManager::getInstance();
    std::string ffmpegPath = configManager.getFFmpegPath();

    // Extract the audio with FFmpeg
    std::string ffmpegCommand = ffmpegPath + " -y -i \"" + m_inputVideoPath +
                                "\" -ar 48000 -ac 1 -c:a pcm_s16le \"" + m_outputAudioPath + "\"";
    if (!Utils::runCommand(ffmpegCommand)) {
        std::cerr << "Error: Failed to extract and convert audio using FFmpeg." << std::endl;
        return false;
    }

    std::cout << "Audio extracted successfully to: " << m_outputAudioPath << std::endl;
    return true;
}

bool AudioProcessor::chunkAudio() {
    Utils::ensureDirectoryExists(m_chunksDir);

    // Calculate chunk durations and start times
    double chunkDuration = m_totalDuration / m_numChunks;
    for (int i = 0; i < m_numChunks; ++i) {
        double startTime = i * chunkDuration;
        double duration = chunkDuration + m_overlapDuration;

        // Handle duration for the last chunk
        if (startTime + duration > m_totalDuration) {
            duration = m_totalDuration - startTime;
        }

        m_startTimes.push_back(startTime);
        m_durations.push_back(duration);
    }

    ConfigManager &configManager = ConfigManager::getInstance();
    std::string ffmpegPath = configManager.getFFmpegPath();

    // Chunk the audio
    for (int i = 0; i < m_numChunks; ++i) {
        std::string chunkPath = m_chunksDir + "/chunk_" + std::to_string(i) + ".wav";

        // Set higher precision for chunk boundaries
        std::ostringstream ssStartTime, ssDuration;
        ssStartTime << std::fixed << std::setprecision(6) << m_startTimes[i];
        ssDuration << std::fixed << std::setprecision(6) << m_durations[i];

        std::string ffmpegSplitCommand = ffmpegPath + " -y -ss " + ssStartTime.str() + " -t " +
                                         ssDuration.str() + " -i \"" + m_outputAudioPath +
                                         "\" -ar 48000 -ac 1 -c:a pcm_s16le \"" + chunkPath + "\"";

        if (!Utils::runCommand(ffmpegSplitCommand)) {
            std::cerr << "Error: Failed to split audio into chunks." << std::endl;
            return false;
        }
        m_chunkPaths.push_back(chunkPath);
    }

    return true;
}

bool AudioProcessor::processChunks() {
    Utils::ensureDirectoryExists(m_processedChunksDir);

    ConfigManager &configManager = ConfigManager::getInstance();
    std::string deepFilterPath = configManager.getDeepFilterPath();

    // Parallel process chunks with the filter
    std::vector<std::thread> threads;
    for (int i = 0; i < m_numChunks; ++i) {
        threads.emplace_back([&, i]() {
            std::string chunkPath =
                std::filesystem::path(m_chunkPaths[i]).make_preferred().string();

            // `-D` flag is super important! uses built-in compensation to avoid sync issues
            // that happens as the processed audio becomes shorter than the org video length
            std::string deepFilterCommand =
                deepFilterPath + " -D -o \"" + m_processedChunksDir + "\" \"" + chunkPath + "\"";
            if (!Utils::runCommand(deepFilterCommand)) {
                std::cerr << "Error: Failed to process chunk with DeepFilterNet: " << chunkPath
                          << std::endl;
            }
        });
    }

    // Block until all threads are done
    for (auto &t : threads) {
        t.join();
    }

    // Prepare paths for processed chunks
    for (int i = 0; i < m_numChunks; ++i) {
        std::filesystem::path chunkPath(m_chunkPaths[i]);
        std::filesystem::path processedChunkPath =
            std::filesystem::path(m_processedChunksDir) / chunkPath.filename();
        m_processedChunkPaths.push_back(processedChunkPath.string());
    }

    return true;
}

bool AudioProcessor::mergeChunks() {
    ConfigManager &configManager = ConfigManager::getInstance();
    std::string ffmpegPath = configManager.getFFmpegPath();

    // Merge processed chunks with `crossfade`
    std::string ffmpegMergeCommand = ffmpegPath + " -y ";
    for (int i = 0; i < m_processedChunkPaths.size(); ++i) {
        ffmpegMergeCommand += "-i \"" + m_processedChunkPaths[i] + "\" ";
    }

    // Build filter complex, i.e. a set of instructions for FFmpeg (called filter graph)
    std::string filterComplex = "";
    int filterIndex = 0;
    for (int i = 0; i < m_processedChunkPaths.size() - 1; ++i) {
        if (i == 0) {
            // Generate a `crossfade` for the first chunk pair (0 and 1)
            filterComplex += "[" + std::to_string(i) + ":a][" + std::to_string(i + 1) +
                             ":a]acrossfade=d=" + std::to_string(m_overlapDuration) +
                             ":c1=tri:c2=tri[a" + std::to_string(filterIndex) + "]; ";
        } else {
            // For the rest, use the result of the previous crossfade (a<filterIndex-1>)
            // and apply a new crossfade with the next chunk (chunk i+1)
            filterComplex += "[a" + std::to_string(filterIndex - 1) + "][" + std::to_string(i + 1) +
                             ":a]acrossfade=d=" + std::to_string(m_overlapDuration) +
                             ":c1=tri:c2=tri[a" + std::to_string(filterIndex) + "]; ";
        }
        filterIndex++;
    }

    // Merge the output of the last crossfade into a final output audio stream
    filterComplex += "[a" + std::to_string(filterIndex - 1) + "]amerge=inputs=1[outa]";

    ffmpegMergeCommand += "-filter_complex \"" + filterComplex +
                          "\" -map \"[outa]\" -c:a pcm_s16le -ar 48000 \"" + m_outputAudioPath +
                          "\"";

    if (!Utils::runCommand(ffmpegMergeCommand)) {
        std::cerr << "Error: Failed to merge back processed audio chunks with crossfading."
                  << std::endl;
        return false;
    }

    return true;
}

}  // namespace MediaProcessor
