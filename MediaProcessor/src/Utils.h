#ifndef UTILS_H
#define UTILS_H

#include <filesystem>
#include <string>
#include <utility>

namespace fs = std::filesystem;

namespace MediaProcessor::Utils {

bool runCommand(const std::string &command);
std::pair<fs::path, fs::path> prepareOutputPaths(const fs::path &videoPath);
bool ensureDirectoryExists(const fs::path &path);
bool removeFileIfExists(const fs::path &filePath);
bool containsWhitespace(const std::string &str);
std::string trimTrailingSpace(const std::string &str);

}  // namespace MediaProcessor::Utils

#endif  // UTILS_H
