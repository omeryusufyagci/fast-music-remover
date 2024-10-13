#include "ConfigFileCreator.h"

#include <fstream>

namespace MediaProcessor::UnitTesting {

void TestConfigFile::writeJsonToFile(const nlohmann::json& jsonContent) {
    std::ofstream file(m_filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing");
    }
    file << jsonContent.dump(4);
    file.close();
}
bool TestConfigFile::delete_config_file() {
    if (std::filesystem::exists(m_filePath)) {
        std::filesystem::remove(m_filePath);
        return true;
    }
    return false;
}

}  // namespace MediaProcessor::UnitTesting