#include "DeepFilterCommandBuilder.h"

#include <sstream>
#include <stdexcept>

#include "Utils.h"

namespace MediaProcessor {

DeepFilterCommandBuilder& DeepFilterCommandBuilder::setInputFile(
    const std::string& inputAudioPath) {
    /**
     * FIXME: Find a solution to ensure flexibility in calling `setInputFile()`.
     * See the header for more details on the problem.
     */

    m_inputAudioPath = inputAudioPath;
    addArgument(inputAudioPath);

    return *this;
}

DeepFilterCommandBuilder& DeepFilterCommandBuilder::setOutputFile(
    const std::string& outputAudioPath) {
    m_outputAudioPath = outputAudioPath;
    addFlag("--output-dir", outputAudioPath);

    return *this;
}

DeepFilterCommandBuilder& DeepFilterCommandBuilder::setNoiseReductionLevel(double level) {
    if (!Utils::isWithinRange(level, 0.0, 1.0)) {
        throw std::invalid_argument("Noise reduction level must be between 0.0 and 1.0");
    }
    m_noiseReductionLevel = level;
    addFlag("--noise-reduction", std::to_string(level));

    return *this;
}

DeepFilterCommandBuilder& DeepFilterCommandBuilder::enableDelayCompensation() {
    addFlag("--compensate-delay");

    return *this;
}

std::string DeepFilterCommandBuilder::build() const {
    if (m_inputAudioPath.empty()) {
        throw std::runtime_error("Input audio path must be specified.");
    }
    if (m_outputAudioPath.empty()) {
        throw std::runtime_error("Output audio path must be specified.");
    }

    return CommandBuilder::build();
}

}  // namespace MediaProcessor