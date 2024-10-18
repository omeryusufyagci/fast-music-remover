#include "DeepFilterCommandBuilder.h"
#include <stdexcept>
#include <sstream>

namespace MediaProcessor {

DeepFilterCommandBuilder& DeepFilterCommandBuilder::setInputAudio(const std::string& inputAudioPath) {
    m_inputAudioPath = inputAudioPath;
    addFlag("--input");
    addArgument(inputAudioPath);
    return *this;
}

DeepFilterCommandBuilder& DeepFilterCommandBuilder::setOutputAudio(const std::string& outputAudioPath) {
    m_outputAudioPath = outputAudioPath;
    addFlag("--output");
    addArgument(outputAudioPath);
    return *this;
}

DeepFilterCommandBuilder& DeepFilterCommandBuilder::setNoiseReductionLevel(double level) {
    if (level < 0.0 || level > 1.0) {
        throw std::invalid_argument("Noise reduction level must be between 0.0 and 1.0");
    }
    m_noiseReductionLevel = level;

    addFlag("--noise-reduction");
    addArgument(std::to_string(level));
    return *this;
}

std::string DeepFilterCommandBuilder::build() const {
    if (m_inputAudioPath.empty()) {
        throw std::runtime_error("Input audio path must be specified.");
    }
    if (m_outputAudioPath.empty()) {
        throw std::runtime_error("Output audio path must be specified.");
    }

    return CommandBuilder::build();  // Use the inherited method to build the final command string.
}

}  // namespace MediaProcessor
