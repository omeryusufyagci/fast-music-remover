#include "testUtils.h"

#include <sndfile.h>

#include <fstream>
#include <stdexcept>
#include <vector>

namespace MediaProcessor::testUtils {

void TestConfigFile::writeJsonToFile(const fs::path& path,
                                     const nlohmann::json& jsonContent) const {
    std::ofstream file(m_filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open Test Configuration File at " +
                                 m_filePath.string());
    }
    file << jsonContent.dump(4);
    file.close();
}

void TestConfigFile::createConfigFile(const fs::path& path, const nlohmann::json& jsonContent) {
    deleteConfigFile();
    m_filePath = path;
    writeJsonToFile(path, jsonContent);
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

bool CompareFiles::compareFilesByteByByte(const fs::path& file1, const fs::path& file2,
                                          size_t chunkSize) {
    std::ifstream f1(file1, std::ios::binary);
    std::ifstream f2(file2, std::ios::binary);

    if (!f1.is_open()) {
        throw std::runtime_error("Failed to open file: " + file1.string());
    }
    if (!f2.is_open()) {
        throw std::runtime_error("Failed to open file: " + file2.string());
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

bool CompareFiles::compareAudioFiles(const fs::path& file1, const fs::path& file2, double tolerance,
                                     size_t chunkSize) {
    SF_INFO sfInfo1, sfInfo2;
    SNDFILE* sndFile1 = sf_open(file1.c_str(), SFM_READ, &sfInfo1);
    if (!sndFile1) {
        throw std::runtime_error("Failed to open file " + file1.string());
    }

    SNDFILE* sndFile2 = sf_open(file2.c_str(), SFM_READ, &sfInfo2);
    if (!sndFile2) {
        sf_close(sndFile1);
        throw std::runtime_error("Failed to open file " + file2.string());
    }

    // Check if the file formats are the same
    if (sfInfo1.channels != sfInfo2.channels || sfInfo1.samplerate != sfInfo2.samplerate) {
        sf_close(sndFile1);
        sf_close(sndFile2);
        return false;
    }

    std::vector<float> buffer1(chunkSize * sfInfo1.channels);
    std::vector<float> buffer2(chunkSize * sfInfo2.channels);

    // Compare in chunks
    sf_count_t framesRead1, framesRead2;
    while ((framesRead1 = sf_readf_float(sndFile1, buffer1.data(), chunkSize)) > 0 &&
           (framesRead2 = sf_readf_float(sndFile2, buffer2.data(), chunkSize)) > 0) {
        if (framesRead1 != framesRead2 ||
            !compareBuffersWithTolerance(buffer1.begin(), buffer1.begin() + framesRead1,
                                         buffer2.begin(), buffer2.begin() + framesRead2,
                                         tolerance)) {
            sf_close(sndFile1);
            sf_close(sndFile2);
            return false;
        }
    }

    sf_close(sndFile1);
    sf_close(sndFile2);
    return true;
}

template <typename T>
bool CompareFiles::isWithinTolerance(const T& a, const T& b, double tolerance) {
    return std::fabs(a - b) <= tolerance;
}

template <typename T>
bool CompareFiles::compareBuffersWithTolerance(T begin1, T end1, T begin2, T end2,
                                               double tolerance) {
    if (std::distance(begin1, end1) != std::distance(begin2, end2)) {
        return false;
    }
    return std::equal(begin1, end1, begin2, end2, [tolerance](const auto& a, const auto& b) {
        return isWithinTolerance(a, b, tolerance);
    });
}

}  // namespace MediaProcessor::testUtils