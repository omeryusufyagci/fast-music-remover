#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <utility>

namespace MediaProcessor::Utils {

bool runCommand(const std::string &command);
std::pair<std::string, std::string> prepareOutputPaths(const std::string &videoPath);
bool ensureDirectoryExists(const std::string &path);
bool removeFileIfExists(const std::string &filePath);
bool containsWhitespace(const std::string &str);
std::string trimTrailingSpace(const std::string &str);

}  // namespace MediaProcessor::Utils

#endif  // UTILS_H
