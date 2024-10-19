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
        throw std::runtime_error("Failed to open Test Configuration File file for writing.");
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
    if (fs::exists(m_filePath)) {
        return fs::remove(m_filePath);
    }
    return false;
}

bool compareFilesByteByByte(const fs::path& file1, const fs::path& file2) {
    /*
    compare two files byte by byte
    */
    std::ifstream f1(file1, std::ios::binary | std::ios::ate);
    std::ifstream f2(file2, std::ios::binary | std::ios::ate);

    // Check if files are of the same size
    if (f1.tellg() != f2.tellg()) {
        return false;
    }

    f1.seekg(0);
    f2.seekg(0);

    std::vector<char> buffer1((std::istreambuf_iterator<char>(f1)),
                              std::istreambuf_iterator<char>());
    std::vector<char> buffer2((std::istreambuf_iterator<char>(f2)),
                              std::istreambuf_iterator<char>());

    return buffer1 == buffer2;
}

bool compareAudioFiles(const fs::path& file1, const fs::path& file2, double tolerance) {
    SF_INFO sfInfo1, sfInfo2;
    SNDFILE* sndFile1 = sf_open(file1.c_str(), SFM_READ, &sfInfo1);
    SNDFILE* sndFile2 = sf_open(file2.c_str(), SFM_READ, &sfInfo2);

    if (!sndFile1 || !sndFile2) {
        if (sndFile1) sf_close(sndFile1);
        if (sndFile2) sf_close(sndFile2);
        throw std::runtime_error("File not found.");
    }

    // Check if the file formats are the same
    if (sfInfo1.channels != sfInfo2.channels || sfInfo1.samplerate != sfInfo2.samplerate ||
        sfInfo1.frames != sfInfo2.frames) {
        sf_close(sndFile1);
        sf_close(sndFile2);
        return false;
    }

    // Buffer to store samples
    std::vector<float> buffer1(sfInfo1.frames * sfInfo1.channels);
    std::vector<float> buffer2(sfInfo2.frames * sfInfo2.channels);

    // Read audio data
    sf_readf_float(sndFile1, buffer1.data(), sfInfo1.frames);
    sf_readf_float(sndFile2, buffer2.data(), sfInfo2.frames);

    // Close the files after reading
    sf_close(sndFile1);
    sf_close(sndFile2);

    // Compare the the two buffers
    return compareBuffersWithTolerance(buffer1.begin(), buffer1.end(), buffer2.begin(),
                                       buffer2.end(), tolerance);
}

template <typename Iter>
bool compareBuffersWithTolerance(Iter begin1, Iter end1, Iter begin2, Iter end2, double tolerance) {
    while (begin1 != end1 && begin2 != end2) {
        if (std::fabs(*begin1 - *begin2) > tolerance) {
            return false;
        }
        ++begin1;
        ++begin2;
    }

    // Check if both iterators reached their end
    return (begin1 == end1) && (begin2 == end2);
}

}  // namespace MediaProcessor::testUtils