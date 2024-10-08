#include "ConfigFileCreator.h"

#include <fstream>

namespace MediaProcessor {

void TempConfigFile::writeJsonToFile(const nlohmann::json& jsonContent) {
    std::ofstream file(m_filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing");
    }
    file << jsonContent.dump(4);
    file.close();
}
}  // namespace MediaProcessor