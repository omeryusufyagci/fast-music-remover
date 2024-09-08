#include "SpeechIsolation.h"
#include "VideoProcessing.h"
#include <iostream>
#include <string>
#include <filesystem>

int main(int argc, char *argv[]) {
    /*
     * Receive a filepath to:
     *   1) Extract the audio with ffmpeg
     *   2) Filter out music and noise with DeepFilterNet to generate an isolated_audio
     *   3) Merge the original video with the isolatedAudio again with ffmpeg, to produce the processed_video
     * 
     * General TODOs: 
     *  - ffmpeg commands are hardcoded
     *  - Overall refactor including cleanup of main
     */

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <video_file_path>" << std::endl;
        return 1;
    }

    std::string videoPath = argv[1];

    // Extract the filename without extenstion via path::stem
    std::filesystem::path videoFilePath(videoPath);
    std::string baseFilename = videoFilePath.stem().string();

    // Prepare output file paths
    std::string outputDir = videoFilePath.parent_path().string();
    std::string isolatedAudioPath = outputDir + "/" + baseFilename + "_isolated_audio.wav"; // speech, i.e. without noise or music
    std::string finalVideoPath = outputDir + "/" + baseFilename + "_processed_video.mp4"; // merged media (isolated_audio + video) 

    if (!SpeechIsolation::isolateSpeech(videoPath, isolatedAudioPath)) {
        std::cerr << "Failed to isolate speech." << std::endl;
        return 1;
    }

    if (!VideoProcessing::mergeAudioVideo(videoPath, isolatedAudioPath, finalVideoPath)) {
        std::cerr << "Failed to merge audio and video." << std::endl;
        return 1;
    }

    std::cout << "Video processed successfully: " << finalVideoPath << std::endl;
    return 0;
}
