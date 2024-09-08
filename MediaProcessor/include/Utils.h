#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Utils {
public:
    static json loadConfig(const std::string &configFilePath);
    static bool runCommand(const std::string &command);
    static bool ensureDirectoryExists(const std::string &path);
    static bool removeFileIfExists(const std::string &filePath);
};

#endif // UTILS_H
