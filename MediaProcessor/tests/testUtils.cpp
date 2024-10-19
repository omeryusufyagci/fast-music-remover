#include "testUtils.h"

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

bool areFilesIdentical(const fs::path& file1, const fs::path& file2) {
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

}  // namespace MediaProcessor::testUtils