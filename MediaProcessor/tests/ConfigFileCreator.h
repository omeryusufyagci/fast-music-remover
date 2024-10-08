#ifndef CONFIGFILECREATOR_H
#define CONFIGFILECREATOR_H

#include <fstream>
#include <map>

class TempConfigFile {
   public:
    TempConfigFile(const std::string path, const std::map<std::string, std::string> keyValuePairs) {
        if (path.empty()) {
            throw std::invalid_argument("File path cannot be empty");
        }
        m_filePath = path;
        std::ofstream configFile(m_filePath);
        if (!configFile.is_open()) {
            throw std::runtime_error("Failed to open file for writing: " + m_filePath);
        }

        try {
            configFile << "{";
            for (const auto& pair : keyValuePairs) {
                if (pair.first.empty() || pair.second.empty()) {
                    throw std::invalid_argument("Key-value pairs cannot be empty");
                }

                configFile << "\"" << pair.first << "\": " << pair.second;
                if (pair != *keyValuePairs.rbegin()) {
                    configFile << ",";
                }
            }
            configFile << "}";
            configFile.close();
        } catch (const std::exception& e) {
            configFile.close();
            throw std::runtime_error("Failed to write to file: " + m_filePath +
                                     ". Error: " + e.what());
        }
    }

    ~TempConfigFile() {
        remove(m_filePath.c_str());
    }

    std::string getFilePath() const {
        return m_filePath;
    }

   private:
    std::string m_filePath;
};

#endif  // CONFIGFILECREATOR_H