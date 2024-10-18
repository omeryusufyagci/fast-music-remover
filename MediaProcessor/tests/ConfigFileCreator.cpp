#include "ConfigFileCreator.h"

#include <fstream>

namespace MediaProcessor::Tests {

void TestConfigFile::writeJsonToFile(const nlohmann::json& jsonContent) {
    std::ofstream file(m_filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open Test Configuration File file for writing.");
    }
    file << jsonContent.dump(4);
    file.close();
}
bool TestConfigFile::deleteConfigFile() {
    if (std::filesystem::exists(m_filePath)) {
        std::filesystem::remove(m_filePath);
        return true;
    }
    return false;
}

}  // namespace MediaProcessor::Tests