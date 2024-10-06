#include "AudioProcessor.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

#include "CommandBuilder.h"
#include "ConfigManager.h"
#include "Utils.h"

namespace fs = std::filesystem;

namespace MediaProcessor {

AudioProcessor::AudioProcessor(const fs::path& inputVideoPath, const fs::path& outputAudioPath)
    : m_inputVideoPath(inputVideoPath),
      m_outputAudioPath(outputAudioPath),
      m_overlapDuration(DEFAULT_OVERLAP_DURATION) {
    m_outputDir = m_outputAudioPath.parent_path();
    m_chunksDir = m_outputDir / "chunks";
    m_processedChunksDir = m_outputDir / "processed_chunks";

    m_numChunks = ConfigManager::getInstance().getOptimalThreadCount();
    std::cout << "INFO: using " << m_numChunks << " threads." << std::endl;
}

bool AudioProcessor::isolateVocals() {
    /*
     * Extracts vocals from a video by chunking, parallel processing, and merging the audio.
     */

    ConfigManager& configManager = ConfigManager::getInstance();

    // Ensure output directory exists and remove output file if it exists
    Utils::ensureDirectoryExists(m_outputDir);
    Utils::removeFileIfExists(m_outputAudioPath);

    std::cout << "Input video path: " << m_inputVideoPath << std::endl;
    std::cout << "Output audio path: " << m_outputAudioPath << std::endl;

    if (!extractAudio()) {
        return false;
    }

    m_totalDuration = getAudioDuration(m_outputAudioPath);
    if (m_totalDuration <= 0) {
        std::cerr << "Error: Invalid audio duration." << std::endl;
        return false;
    }

    if (!chunkAudio()) {
        return false;
    }

    if (!filterChunks()) {
        return false;
    }

    if (!mergeChunks()) {
        return false;
    }

    // The intermediary files can be useful for debugging, but for release we'll delete them
    fs::remove_all(m_chunksDir);
    fs::remove_all(m_processedChunksDir);

    return true;
}

bool AudioProcessor::extractAudio() {
    ConfigManager& configManager = ConfigManager::getInstance();
    fs::path ffmpegPath = configManager.getFFmpegPath();

    // Extract the audio with FFmpeg
    CommandBuilder cmd;
    cmd.addArgument(ffmpegPath.string());
    cmd.addFlag("-y");
    cmd.addFlag("-i", m_inputVideoPath.string());
    cmd.addFlag("-ar", "48000");
    cmd.addFlag("-ac", "1");
    cmd.addFlag("-c:a", "pcm_s16le");
    cmd.addArgument(m_outputAudioPath.string());

    if (!Utils::runCommand(cmd.build())) {
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

    ConfigManager& configManager = ConfigManager::getInstance();
    fs::path ffmpegPath = configManager.getFFmpegPath();

    // Chunk the audio
    for (int i = 0; i < m_numChunks; ++i) {
        fs::path chunkPath = m_chunksDir / ("chunk_" + std::to_string(i) + ".wav");

        // Set higher precision for chunk boundaries
        std::ostringstream ssStartTime, ssDuration;
        ssStartTime << std::fixed << std::setprecision(6) << m_startTimes[i];
        ssDuration << std::fixed << std::setprecision(6) << m_durations[i];

        CommandBuilder cmd;
        cmd.addArgument(ffmpegPath.string());
        cmd.addFlag("-y");
        cmd.addFlag("-ss", ssStartTime.str());
        cmd.addFlag("-t", ssDuration.str());
        cmd.addFlag("-i", m_outputAudioPath.string());
        cmd.addFlag("-ar", "48000");
        cmd.addFlag("-ac", "1");
        cmd.addFlag("-c:a", "pcm_s16le");
        cmd.addArgument(chunkPath.string());

        if (!Utils::runCommand(cmd.build())) {
            std::cerr << "Error: Failed to split audio into chunks." << std::endl;
            return false;
        }
        m_chunkPaths.push_back(chunkPath);
    }

    return true;
}

bool AudioProcessor::filterChunks() {
    Utils::ensureDirectoryExists(m_processedChunksDir);

    ConfigManager& configManager = ConfigManager::getInstance();
    fs::path deepFilterPath = configManager.getDeepFilterPath();

    // Parallel process chunks with the filter
    std::vector<std::thread> threads;
    for (int i = 0; i < m_numChunks; ++i) {
        threads.emplace_back([&, i]() {
            fs::path chunkPath = m_chunkPaths[i];

            // `-D` flag is super important! uses built-in compensation to avoid sync issues
            // that happens as the processed audio becomes shorter than the org video length
            CommandBuilder cmd;
            cmd.addArgument(deepFilterPath.string());
            cmd.addFlag("-D");
            cmd.addFlag("-o", m_processedChunksDir.string());
            cmd.addArgument(chunkPath.string());

            if (!Utils::runCommand(cmd.build())) {
                std::cerr << "Error: Failed to process chunk with DeepFilterNet: " << chunkPath
                          << std::endl;
            }
        });
    }

    // Block until all threads are done
    for (auto& t : threads) {
        t.join();
    }

    // Prepare paths for processed chunks
    for (int i = 0; i < m_numChunks; ++i) {
        fs::path chunkPath = m_chunkPaths[i];
        fs::path processedChunkPath = m_processedChunksDir / chunkPath.filename();
        m_processedChunkPaths.push_back(processedChunkPath);
    }

    return true;
}

std::string AudioProcessor::buildFilterComplex() const {
    // Build filter complex, i.e. a set of instructions for FFmpeg (called filter graph)
    std::string filterComplex = "";
    int filterIndex = 0;

    if (m_processedChunkPaths.size() < 2) {
        return filterComplex;  // Return empty string if not enough chunks
    }

    for (int i = 0; i < static_cast<int>(m_processedChunkPaths.size()) - 1; ++i) {
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
    return filterComplex;
}

bool AudioProcessor::mergeChunks() {
    ConfigManager& configManager = ConfigManager::getInstance();
    fs::path ffmpegPath = configManager.getFFmpegPath();

    // Merge processed chunks with `crossfade` using CommandBuilder
    CommandBuilder cmd;
    cmd.addArgument(ffmpegPath.string());
    cmd.addFlag("-y");
    for (const auto& chunkPath : m_processedChunkPaths) {
        cmd.addFlag("-i", chunkPath.string());
    }
  
    // If there's more then one processed chunk
    if (static_cast<int>(m_processedChunkPaths.size()) >= 2) {
        cmd.addFlag("-filter_complex", buildFilterComplex());
        cmd.addFlag("-map", "[outa]");
    }
    
    cmd.addFlag("-c:a", "pcm_s16le");
    cmd.addFlag("-ar", "48000");
    cmd.addArgument(m_outputAudioPath.string());

    if (!Utils::runCommand(cmd.build())) {
        std::cerr << "Error: Failed to merge back processed audio chunks with crossfading."
                  << std::endl;
        return false;
    }

    return true;
}

double AudioProcessor::getAudioDuration(const fs::path& audioPath) {
    /*
     * TODO: ffprobe command to be used via the to-be-made FFMpegUtils class...
     */

    CommandBuilder cmd;
    cmd.addArgument("ffprobe");
    cmd.addFlag("-v", "error");
    cmd.addFlag("-show_entries", "format=duration");
    cmd.addFlag("-of", "default=noprint_wrappers=1:nokey=1");
    cmd.addArgument(audioPath.string());

    FILE* pipe = popen(cmd.build().c_str(), "r");
    if (!pipe) {
        std::cerr << "Error: Failed to run ffprobe to get audio duration." << std::endl;
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
    } catch (std::exception& e) {
        std::cerr << "Error: Could not parse audio duration." << std::endl;
        return -1;
    }
}

}  // namespace MediaProcessor
