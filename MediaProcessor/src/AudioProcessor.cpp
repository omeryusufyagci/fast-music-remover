#include "AudioProcessor.h"

#include <sndfile.h>

#include <future>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

#include "CommandBuilder.h"
#include "ConfigManager.h"
#include "DeepFilterNetFFI.h"
#include "ThreadPool.h"
#include "Utils.h"

namespace fs = std::filesystem;

namespace MediaProcessor {

AudioProcessor::AudioProcessor(const fs::path& inputVideoPath, const fs::path& outputAudioPath)
    : m_inputVideoPath(inputVideoPath),
      m_outputAudioPath(outputAudioPath),
      m_overlapDuration(DEFAULT_OVERLAP_DURATION) {
    m_outputPath = m_outputAudioPath.parent_path();
    m_chunksPath = m_outputPath / "chunks";
    m_processedChunksDir = m_outputPath / "processed_chunks";

    m_numChunks = ConfigManager::getInstance().getOptimalThreadCount();
    std::cout << "INFO: using " << m_numChunks << " threads." << std::endl;
}

bool AudioProcessor::isolateVocals() {
    /*
     * Extracts vocals from a video by chunking, parallel processing, and merging the audio.
     */

    ConfigManager& configManager = ConfigManager::getInstance();

    // Ensure output directory exists and remove output file if it exists
    Utils::ensureDirectoryExists(m_outputPath);
    Utils::removeFileIfExists(m_outputAudioPath);

    std::cout << "Input video path: " << m_inputVideoPath << std::endl;
    std::cout << "Output audio path: " << m_outputAudioPath << std::endl;

    if (!extractAudio()) {
        return false;
    }

    m_totalDuration = Utils::getMediaDuration(m_outputAudioPath);
    if (m_totalDuration <= 0) {
        std::cerr << "Error: Invalid audio duration." << std::endl;
        return false;
    }

    if (!splitAudioIntoChunks()) {
        return false;
    }

    if (!filterChunks()) {
        return false;
    }

    if (!mergeChunks()) {
        return false;
    }

    // Intermediary files
    fs::remove_all(m_chunksPath);
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

bool AudioProcessor::splitAudioIntoChunks() {
    ConfigManager& configManager = ConfigManager::getInstance();
    fs::path ffmpegPath = configManager.getFFmpegPath();

    Utils::ensureDirectoryExists(m_chunksPath);

    std::vector<double> chunkStartTimes;
    std::vector<double> chunkDurations;
    populateChunkDurations(chunkStartTimes, chunkDurations);

    for (int i = 0; i < m_numChunks; ++i) {
        if (!generateChunkFile(i, chunkStartTimes[i], chunkDurations[i], ffmpegPath)) {
            std::cerr << "Error: Failed to split audio into chunks." << std::endl;
            return false;
        }
    }
    return true;
}

bool AudioProcessor::generateChunkFile(int index, const double startTime, const double duration,
                                       const fs::path& ffmpegPath) {
    fs::path chunkPath = m_chunksPath / ("chunk_" + std::to_string(index) + ".wav");

    // Set higher precision for chunk boundaries
    std::ostringstream ssStartTime, ssDuration;
    ssStartTime << std::fixed << std::setprecision(6) << startTime;
    ssDuration << std::fixed << std::setprecision(6) << duration;

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
        return false;
    }

    m_chunkPathCol.push_back(chunkPath);
    return true;
}

bool AudioProcessor::invokeDeepFilter(fs::path chunkPath) {
    ConfigManager& configManager = ConfigManager::getInstance();
    const fs::path deepFilterPath = configManager.getDeepFilterPath();
    const fs::path deepFilterTarballPath = configManager.getDeepFilterTarballPath();

    // `--compensate-delay` ensures the audio remains in sync after filtering
    CommandBuilder cmd;
    cmd.addArgument(deepFilterPath.string());
    cmd.addFlag("--compensate-delay");
    cmd.addFlag("--output-dir", m_processedChunksDir.string());
    cmd.addArgument(chunkPath.string());

    if (!Utils::runCommand(cmd.build())) {
        std::cerr << "Error: Failed to process chunk with DeepFilterNet: " << chunkPath
                  << std::endl;
        return false;
    }

    return true;
}

bool AudioProcessor::invokeDeepFilterFFI(fs::path chunkPath) {
    // TODO: think about the method name

    ConfigManager& configManager = ConfigManager::getInstance();
    const fs::path deepFilterTarballPath = configManager.getDeepFilterTarballPath();

    DFState* df_state = df_create(deepFilterTarballPath.c_str(), 100.0f, nullptr);
    size_t frameLength = df_get_frame_length(df_state);

    // Open the input file with SNDFILE
    // TODO: extract into a utility
    SF_INFO sfInfoIn;
    SNDFILE* inputFile = sf_open(chunkPath.c_str(), SFM_READ, &sfInfoIn);
    if (!inputFile) {
        std::cerr << "Error: Could not open input WAV file: " << chunkPath << std::endl;
        df_free(df_state);
        return false;
    }

    // Prepare output file with SNDFILE
    // TODO: extract into a utility
    SF_INFO sfInfoOut = sfInfoIn;
    fs::path processedChunkPath = m_processedChunksDir / chunkPath.filename();
    SNDFILE* outputFile = sf_open(processedChunkPath.c_str(), SFM_WRITE, &sfInfoOut);
    if (!outputFile) {
        std::cerr << "Error: Could not open output WAV file: " << processedChunkPath << std::endl;
        sf_close(inputFile);
        df_free(df_state);
        return false;
    }

    std::vector<float> inputBuffer(frameLength);
    std::vector<float> outputBuffer(frameLength);

    // Process the audio as a stream
    sf_count_t numFrames;
    while ((numFrames = sf_readf_float(inputFile, inputBuffer.data(), frameLength)) > 0) {
        df_process_frame(df_state, inputBuffer.data(), outputBuffer.data());
        sf_writef_float(outputFile, outputBuffer.data(), numFrames);
    }

    sf_close(inputFile);
    sf_close(outputFile);

    df_free(df_state);

    return true;
}

bool AudioProcessor::filterChunks() {
    Utils::ensureDirectoryExists(m_processedChunksDir);

    ThreadPool pool(m_numChunks);

    std::vector<std::future<void>> results;
    for (int i = 0; i < m_numChunks; ++i) {
        results.emplace_back(pool.enqueue([&, i]() {
            fs::path chunkPath = m_chunkPathCol[i];

            invokeDeepFilter(chunkPath);
            // invokeDeepFilterFFI(chunkPath);  // RT API still under validation
        }));
    }

    // Block until all threads are done
    for (auto& result : results) {
        result.get();
    }

    // Prepare paths for processed chunks
    for (int i = 0; i < m_numChunks; ++i) {
        fs::path chunkPath = m_chunkPathCol[i];
        fs::path processedChunkPath = m_processedChunksDir / chunkPath.filename();
        m_processedChunkCol.push_back(processedChunkPath);
    }

    return true;
}

void AudioProcessor::populateChunkDurations(std::vector<double>& startTimes,
                                            std::vector<double>& durations) const {
    double chunkDuration = m_totalDuration / m_numChunks;
    for (int i = 0; i < m_numChunks; ++i) {
        double startTime = i * chunkDuration;
        double duration = chunkDuration + m_overlapDuration;

        // Handle duration for the last chunk
        if (startTime + duration > m_totalDuration) {
            duration = m_totalDuration - startTime;
        }

        startTimes.push_back(startTime);
        durations.push_back(duration);
    }
}

std::string AudioProcessor::buildFilterComplex() const {
    // Build filter complex, i.e. a set of instructions for FFmpeg (called filter graph)
    std::string filterComplex = "";
    int filterIndex = 0;

    if (m_processedChunkCol.size() < 2) {
        return filterComplex;  // Return empty string if not enough chunks
    }

    // TODO: extract this into an `applyCrossFade()` method.
    for (int i = 0; i < static_cast<int>(m_processedChunkCol.size()) - 1; ++i) {
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

    // Merge processed chunks with `crossfade`
    CommandBuilder cmd;
    cmd.addArgument(ffmpegPath.string());
    cmd.addFlag("-y");
    for (const auto& chunkPath : m_processedChunkCol) {
        cmd.addFlag("-i", chunkPath.string());
    }

    if (static_cast<int>(m_processedChunkCol.size()) >= 2) {
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

}  // namespace MediaProcessor
