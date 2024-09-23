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
    static double getAudioDuration(const std::string &audioPath);
};

#endif // UTILS_H
