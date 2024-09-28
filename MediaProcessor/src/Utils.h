#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace MediaProcessor::Utils {

bool runCommand(const std::string &command);
std::pair<std::string, std::string> prepareOutputPaths(const std::string &videoPath);
bool ensureDirectoryExists(const std::string &path);
bool removeFileIfExists(const std::string &filePath);
double getAudioDuration(const std::string &audioPath);

}  // namespace MediaProcessor::Utils

#endif  // UTILS_H
