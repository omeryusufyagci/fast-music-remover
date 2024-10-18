#ifndef CONFIGFILECREATOR_H
#define CONFIGFILECREATOR_H

#include <filesystem>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace fs = std::filesystem;
namespace MediaProcessor::Tests {
class TestConfigFile {
    /*
    Create a test configuration file from a JSON object.
    */
   public:
    TestConfigFile() {}
    TestConfigFile(const fs::path& path, const nlohmann::json& jsonContent) : m_filePath(path) {
        writeJsonToFile(jsonContent);
    }

    bool createTestConfigFile(const fs::path& path, const nlohmann::json& jsonContent) {
        deleteConfigFile();
        m_filePath = path;
        writeJsonToFile(jsonContent);
        return true;
    }

    ~TestConfigFile() {
        deleteConfigFile();
    }

    fs::path getFilePath() const {
        return m_filePath;
    }

   private:
    fs::path m_filePath;

    void writeJsonToFile(const nlohmann::json& jsonContent);
    bool deleteConfigFile();
};
}  // namespace MediaProcessor::Tests

#endif  // CONFIGFILECREATOR_H