#include "DeepFilterCommandBuilder.h"

#include <stdexcept>
#include <sstream>

namespace MediaProcessor {

DeepFilterCommandBuilder& DeepFilterCommandBuilder::setInputAudio(const std::string& inputAudioPath) {
    this->inputAudioPath = inputAudioPath;
    addArgument("--input");
    addArgument(inputAudioPath);
    return *this;
}

DeepFilterCommandBuilder& DeepFilterCommandBuilder::setOutputAudio(const std::string& outputAudioPath) {
    this->outputAudioPath = outputAudioPath;
    addArgument("--output");
    addArgument(outputAudioPath);
    return *this;
}

DeepFilterCommandBuilder& DeepFilterCommandBuilder::setNoiseReductionLevel(double level) {
    if (level < 0.0 || level > 1.0) {
        throw std::invalid_argument("Noise reduction level must be between 0.0 and 1.0");
    }
    this->noiseReductionLevel = level;

    addArgument("--noise-reduction");
    addArgument(std::to_string(level));
    return *this;
}

std::string DeepFilterCommandBuilder::build() const {
    if (inputAudioPath.empty() || outputAudioPath.empty()) {
        throw std::runtime_error("Input and output audio paths must be specified.");
    }

    return CommandBuilder::build();  // Use the inherited method to build the final command string.
}

}  // namespace MediaProcessor
