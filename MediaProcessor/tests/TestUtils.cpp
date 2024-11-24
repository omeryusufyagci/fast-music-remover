#include "TestUtils.h"

#include <sndfile.h>

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace MediaProcessor::TestUtils {

void TestConfigFile::writeJsonToFile(const fs::path& path, const nlohmann::json& jsonObject) const {
    std::ofstream file(m_filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open Test Configuration File at " +
                                 m_filePath.string());
    }
    file << jsonObject.dump(4);
    file.close();
}

void TestConfigFile::generateConfigFile(const fs::path& path, const nlohmann::json& jsonObject) {
    deleteConfigFile();
    m_filePath = path;
    writeJsonToFile(path, jsonObject);
}

bool TestConfigFile::deleteConfigFile() const {
    if (!fs::exists(m_filePath)) {
        return false;
    }
    if (!fs::remove(m_filePath)) {
        throw std::runtime_error("Failed to delete Test Configuration File at" +
                                 m_filePath.string());
    }
    return true;
}

bool CompareFiles::compareFilesByteByByte(const fs::path& filePath1, const fs::path& filePath2,
                                          size_t chunkSize) {
    std::ifstream f1(filePath1, std::ios::binary);
    std::ifstream f2(filePath2, std::ios::binary);

    if (!f1.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath1.string());
    }
    if (!f2.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath2.string());
    }

    f1.seekg(0, std::ios::end);
    f2.seekg(0, std::ios::end);
    if (f1.tellg() != f2.tellg()) {
        return false;  // Files are of different sizes
    }

    f1.seekg(0);
    f2.seekg(0);

    std::vector<char> buffer1(chunkSize);
    std::vector<char> buffer2(chunkSize);

    // Compare in chunks
    while (f1 && f2) {
        f1.read(buffer1.data(), chunkSize);
        f2.read(buffer2.data(), chunkSize);

        std::streamsize bytesRead1 = f1.gcount();
        std::streamsize bytesRead2 = f2.gcount();

        if (bytesRead1 != bytesRead2 ||
            !std::equal(buffer1.begin(), buffer1.begin() + bytesRead1, buffer2.begin())) {
            return false;
        }
    }

    return true;
}

bool CompareFiles::compareAudioFiles(const fs::path& filePath1, const fs::path& filePath2,
                                     double tolerance, size_t chunkSize) {
    SF_INFO sfInfo1, sfInfo2;
    SNDFILE* sndFile1 = sf_open(filePath1.c_str(), SFM_READ, &sfInfo1);
    if (!sndFile1) {
        throw std::runtime_error("Failed to open file 1: " + filePath1.string());
    }

    SNDFILE* sndFile2 = sf_open(filePath2.c_str(), SFM_READ, &sfInfo2);
    if (!sndFile2) {
        sf_close(sndFile1);
        throw std::runtime_error("Failed to open file 2: " + filePath2.string());
    }

    std::cout << "Total frames: File 1 = " << sfInfo1.frames << ", File 2 = " << sfInfo2.frames
              << std::endl;

    if (sfInfo1.frames != sfInfo2.frames) {
        std::cerr << "Warning: Total frame count mismatch. Proceeding with overlap comparison.\n";
    }

    std::vector<float> buffer1(chunkSize * sfInfo1.channels);
    std::vector<float> buffer2(chunkSize * sfInfo2.channels);

    // Compare in chunks
    sf_count_t framesRead1, framesRead2;
    size_t chunkIndex = 0;
    while (true) {
        framesRead1 = sf_readf_float(sndFile1, buffer1.data(), chunkSize);
        framesRead2 = sf_readf_float(sndFile2, buffer2.data(), chunkSize);

        // Break if both files are at the end
        if (framesRead1 == 0 && framesRead2 == 0) {
            break;
        }

        std::cout << "Comparing chunk " << chunkIndex++ << ": " << framesRead1
                  << " frames from file 1, " << framesRead2 << " frames from file 2" << std::endl;

        // Handle comparison for last chunk which may be shorter than default chunk size
        size_t framesToCompare = std::min(framesRead1, framesRead2);
        if (!compareBuffersWithTolerance(buffer1.begin(), buffer1.begin() + framesToCompare,
                                         buffer2.begin(), buffer2.begin() + framesToCompare,
                                         tolerance)) {
            std::cerr << "Mismatch in audio data in chunk " << chunkIndex << std::endl;
            sf_close(sndFile1);
            sf_close(sndFile2);
            return false;
        }
    }

    sf_close(sndFile1);
    sf_close(sndFile2);

    std::cout << "Files matched within tolerance: " << tolerance << std::endl;
    return true;
}

template <typename T>
bool CompareFiles::isWithinTolerance(const T& a, const T& b, double tolerance,
                                     double relativeTolerance) {
    double diff = std::fabs(a - b);
    return diff <= tolerance || diff <= relativeTolerance * std::max(std::fabs(a), std::fabs(b));
}

template <typename T>
bool CompareFiles::compareBuffersWithTolerance(T begin1, T end1, T begin2, T end2, double tolerance,
                                               double relativeTolerance) {
    if (std::distance(begin1, end1) != std::distance(begin2, end2)) {
        std::cerr << "Buffer size mismatch: " << std::distance(begin1, end1) << " vs "
                  << std::distance(begin2, end2) << std::endl;
        return false;
    }

    size_t index = 0;
    size_t mismatchCount = 0;
    for (auto it1 = begin1, it2 = begin2; it1 != end1; ++it1, ++it2, ++index) {
        if (!isWithinTolerance(*it1, *it2, tolerance, relativeTolerance)) {
            ++mismatchCount;
            std::cerr << "Value mismatch at index " << index << ": " << *it1 << " vs " << *it2
                      << " (tolerance: " << tolerance
                      << ", relativeTolerance: " << relativeTolerance << ")" << std::endl;

            if (mismatchCount > 10) {
                std::cerr << "More than 10 mismatches detected, skipping further logs."
                          << std::endl;
                break;
            }
        }
    }

    return mismatchCount == 0;
}

}  // namespace MediaProcessor::TestUtils