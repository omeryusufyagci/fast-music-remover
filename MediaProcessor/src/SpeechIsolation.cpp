#include "SpeechIsolation.h"

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "Utils.h"

bool SpeechIsolation::isolateSpeech(const std::string &inputVideoPath,
                                    std::string &outputAudioPath) {
    /*
     * TODO: this turned into a monster while fixing issue after issue related to chunking
     * everything needs to be refactored and it's time SpeecProcessing gets a class
     * The functionality works well, but not thoroughly tested for different file sizes and hw limitations
     */

    json config = Utils::loadConfig("config.json");
    if (config.is_null()) {
        std::cerr << "Error: Could not load configuration." << std::endl;
        return false;
    }

    // to be put in ConfigManager
    std::string deepFilterPath = config["deep_filter_path"];
    std::string ffmpegPath = config["ffmpeg_path"];

    // Ensure output directory exists and remove output file if it exits
    std::filesystem::path outputDir = std::filesystem::path(outputAudioPath).parent_path();
    Utils::ensureDirectoryExists(outputDir.string());
    Utils::removeFileIfExists(outputAudioPath);

    std::cout << "Input video path: " << inputVideoPath << std::endl;
    std::cout << "Output audio path: " << outputAudioPath << std::endl;

    // Extract the audio with FFmpeg (to be put in AudioExtractor)
    std::string ffmpegCommand = ffmpegPath + " -y -i \"" + inputVideoPath +
                                "\" -ar 48000 -ac 1 -c:a pcm_s16le \"" + outputAudioPath + "\"";
    if (!Utils::runCommand(ffmpegCommand)) {
        std::cerr << "Error: Failed to extract and convert audio using FFmpeg." << std::endl;
        return false;
    }

    std::cout << "Audio extracted successfully to: " << outputAudioPath << std::endl;

    double totalDuration = Utils::getAudioDuration(outputAudioPath);
    if (totalDuration <= 0) {
        std::cerr << "Error: Invalid audio duration." << std::endl;
        return false;
    }

    // to be put in ChunkProcessor
    int numChunks = 6;  // Number of threads/chunks
    double overlapDuration =
        0.5;  // in seconds  // TODO: had issues when playing with this, tbc again now that it works

    // Calculate chunk durations and start times
    std::vector<double> startTimes;
    std::vector<double> durations;
    double chunkDuration = totalDuration / numChunks;
    for (int i = 0; i < numChunks; ++i) {
        double startTime = i * chunkDuration;
        double duration = chunkDuration + overlapDuration;

        // Handle duration for the last chunk
        if (startTime + duration > totalDuration) {
            duration = totalDuration - startTime;
        }

        startTimes.push_back(startTime);
        durations.push_back(duration);

        // std::cout << "Chunk " << i << ": Start Time = " << startTime << "s, Duration = " << duration << "s" << std::endl; // debug
    }

    // Storage for chunks
    std::string chunksDir = outputDir.string() + "/chunks";
    std::string processedChunksDir = outputDir.string() + "/processed_chunks";
    Utils::ensureDirectoryExists(chunksDir);
    Utils::ensureDirectoryExists(processedChunksDir);

    // Chunk the audio (to be put in ChunkProcessor)
    std::vector<std::string> chunkPaths;
    for (int i = 0; i < numChunks; ++i) {
        std::string chunkPath = chunksDir + "/chunk_" + std::to_string(i) + ".wav";

        // Using setprecision(6) helped? not really clear as I changed multiple things at the same time
        // eventually would be nice to check if it's really needed
        std::ostringstream ssStartTime, ssDuration;
        ssStartTime << std::fixed << std::setprecision(6) << startTimes[i];
        ssDuration << std::fixed << std::setprecision(6) << durations[i];

        std::string ffmpegSplitCommand = ffmpegPath + " -y -ss " + ssStartTime.str() + " -t " +
                                         ssDuration.str() + " -i \"" + outputAudioPath +
                                         "\" -ar 48000 -ac 1 -c:a pcm_s16le \"" + chunkPath + "\"";

        if (!Utils::runCommand(ffmpegSplitCommand)) {
            std::cerr << "Error: Failed to split audio into chunks." << std::endl;
            return false;
        }
        chunkPaths.push_back(chunkPath);
    }

    // Parallel process chunks with the filter
    std::vector<std::thread> threads;
    for (int i = 0; i < numChunks; ++i) {
        threads.emplace_back([&, i]() {
            std::string chunkPath = chunkPaths[i];

            // `-D` flag is super important! uses built in compensation to avoid sync issues
            std::string deepFilterCommand =
                deepFilterPath + " -D -o \"" + processedChunksDir + "\" \"" + chunkPath + "\"";
            if (!Utils::runCommand(deepFilterCommand)) {
                std::cerr << "Error: Failed to process chunk with DeepFilterNet: " << chunkPath
                          << std::endl;
            }
        });
    }

    // block until all threads are done
    for (auto &t : threads) {
        t.join();
    }

    // Prepare paths for processed chunks
    std::vector<std::string> processedChunkPaths;
    for (int i = 0; i < numChunks; ++i) {
        std::filesystem::path chunkPath(chunkPaths[i]);
        std::string processedChunkPath =
            processedChunksDir + "/" +
            chunkPath.filename().string();  // DeepFilter overwrites the given file by default
        processedChunkPaths.push_back(processedChunkPath);
    }

    // Merge processed chunks with `crossfading`
    // this was crucial to fix sync issues, otherwise trimmed audio len
    // is less than raw audio causing sync issues (as processed audio leads video)
    std::string ffmpegConcatCommand = ffmpegPath + " -y ";
    for (int i = 0; i < processedChunkPaths.size(); ++i) {
        ffmpegConcatCommand += "-i \"" + processedChunkPaths[i] + "\" ";
    }

    // this following `crossfade` works and it's cruical for merging back the media, but really hard to read
    // TODO: use a helper to make it a bit more readable (probably to be put in ChunkProcessor as well)
    std::string filterComplex = "";  // set of instructions for ffmpeg (called filter graph)
    int filterIndex = 0;
    for (int i = 0; i < processedChunkPaths.size() - 1; ++i) {
        if (i == 0) {
            // generate a crossfade for the first chunk pair (0 and 1)
            filterComplex += "[" + std::to_string(i) + ":a][" + std::to_string(i + 1) +
                             ":a]acrossfade=d=" + std::to_string(overlapDuration) +
                             ":c1=tri:c2=tri[a" + std::to_string(filterIndex) + "]; ";
        } else {
            // For the rest, use the result of the previous crossfade (a<filterIndex-1>)
            // and apply a new crossfade with the next chunk (chunk i+1)
            filterComplex += "[a" + std::to_string(filterIndex - 1) + "][" + std::to_string(i + 1) +
                             ":a]acrossfade=d=" + std::to_string(overlapDuration) +
                             ":c1=tri:c2=tri[a" + std::to_string(filterIndex) + "]; ";
        }

        filterIndex++;
    }

    /* 
     * Merge the output of the last crossfade into a final output audio stream
     *  `[a<filterIndex - 1>]` is the last processed audio chunk
     *  `amerge=inputs=1` means we are merging the last audio stream
     *  `[outa]` is the label for the final merged audio stream
     */
    filterComplex += "[a" + std::to_string(filterIndex - 1) + "]amerge=inputs=1[outa]";

    ffmpegConcatCommand += "-filter_complex \"" + filterComplex +
                           "\" -map \"[outa]\" -c:a pcm_s16le -ar 48000 \"" + outputAudioPath +
                           "\"";

    if (!Utils::runCommand(ffmpegConcatCommand)) {
        std::cerr << "Error: Failed to concatenate processed audio chunks with crossfading."
                  << std::endl;
        return false;
    }

    // debug
    // std::filesystem::remove_all(chunksDir);
    // std::filesystem::remove_all(processedChunksDir);

    return true;
}
