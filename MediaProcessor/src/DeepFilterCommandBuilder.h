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
    std::string m_inputAudioPath;
    std::string m_outputAudioPath;  
    double m_noiseReductionLevel = 0.5;  // Default noise reduction level.
};

}  // namespace MediaProcessor

#endif 
