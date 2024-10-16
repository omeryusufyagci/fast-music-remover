#ifndef DEEPFILTERCOMMANDBUILDER_H
#define DEEPFILTERCOMMANDBUILDER_H

#include "CommandBuilder.h"

namespace MediaProcessor {

class DeepFilterCommandBuilder : public CommandBuilder {
public:
    DeepFilterCommandBuilder& setInputAudio(const std::string& inputAudioPath);
    DeepFilterCommandBuilder& setOutputAudio(const std::string& outputAudioPath);
    DeepFilterCommandBuilder& setNoiseReductionLevel(double level);

    std::string build() const override;

private:
    std::string inputAudioPath;
    std::string outputAudioPath;
    double noiseReductionLevel = 0.5;  // Default noise reduction level.
};

}  // namespace MediaProcessor

#endif 
