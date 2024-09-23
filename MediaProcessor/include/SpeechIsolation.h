#ifndef SPEECHISOLATION_H
#define SPEECHISOLATION_H

#include <string>

class SpeechIsolation {
public:
    static bool isolateSpeech(const std::string &inputVideoFile, std::string &outputAudioFile);

};

#endif // SPEECHISOLATION_H
