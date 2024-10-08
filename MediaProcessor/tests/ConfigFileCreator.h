#ifndef CONFIGFILECREATOR_H
#define CONFIGFILECREATOR_H

#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace MediaProcessor {
/// @brief Create a temporary configuration file from a JSON object.
class TempConfigFile {
   public:
    TempConfigFile(const std::string path, const nlohmann::json& jsonContent) : m_filePath(path) {
        writeJsonToFile(jsonContent);
    }

    ~TempConfigFile() {
        remove(m_filePath.c_str());
    }

    std::string getFilePath() const {
        return m_filePath;
    }

   private:
    std::string m_filePath;

    void writeJsonToFile(const nlohmann::json& jsonContent);
};
}  // namespace MediaProcessor

#endif  // CONFIGFILECREATOR_H