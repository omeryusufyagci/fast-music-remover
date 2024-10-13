#ifndef CONFIGFILECREATOR_H
#define CONFIGFILECREATOR_H

#include <filesystem>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace MediaProcessor {
class TestConfigFile {
    /*
    Create a test configuration file from a JSON object.
    */
   public:
    TestConfigFile() {}
    TestConfigFile(const std::filesystem::path& path, const nlohmann::json& jsonContent)
        : m_filePath(path) {
        writeJsonToFile(jsonContent);
    }

    bool createTestConfigFile(const std::filesystem::path& path,
                              const nlohmann::json& jsonContent) {
        delete_config_file();
        m_filePath = path;
        writeJsonToFile(jsonContent);
        return true;
    }

    ~TestConfigFile() {
        delete_config_file();
    }

    std::string getFilePath() const {
        return m_filePath;
    }

   private:
    std::filesystem::path m_filePath;

    void writeJsonToFile(const nlohmann::json& jsonContent);
    bool delete_config_file();
};
}  // namespace MediaProcessor

#endif  // CONFIGFILECREATOR_H