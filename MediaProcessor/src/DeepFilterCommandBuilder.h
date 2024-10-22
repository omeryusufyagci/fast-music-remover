#ifndef DEEPFILTERCOMMANDBUILDER_H
#define DEEPFILTERCOMMANDBUILDER_H

#include "CommandBuilder.h"

namespace MediaProcessor {

/**
 * @brief A builder class for constructing DeepFilter commands.
 */
class DeepFilterCommandBuilder : public CommandBuilder {
   public:
    /**
     * @brief Sets the input audio file for the DeepFilter command.
     *
     * @note
     * To ensure the correct behavior, please call `setInputFile()` as the first
     * method when configuring this builder. This guarantees that the input file
     * is added in the proper order before other optional parameters.
     *
     * DeepFilter not providing an input file flag poses challenges to implement
     * this without breaking the builder pattern or sacrificing const-correctness.
     * We'll try to find a good solution for this ASAP.
     *
     * @return A reference to the updated object for method chaining.
     */
    DeepFilterCommandBuilder& setInputFile(const std::string& inputAudioPath);

    /**
     * @brief Sets the output audio file for the DeepFilter command.
     *
     * @return A reference to the updated object for method chaining.
     */
    DeepFilterCommandBuilder& setOutputFile(const std::string& outputAudioPath);

    /**
     * @brief Sets the noise reduction level for the DeepFilter command.
     *
     * @param level The noise reduction level: from 0.0 (no reduction) to 1.0 (max reduction).
     * @return A reference to the updated DeepFilterCommandBuilder instance.
     *
     * @throws std::invalid_argument if the level is not within the range [0.0, 1.0].
     */
    DeepFilterCommandBuilder& setNoiseReductionLevel(double level);

    /**
     * @brief Enabled filtering delay compensation for the DeepFilter command.
     *
     * @return A reference to the updated DeepFilterCommandBuilder instance.
     */
    DeepFilterCommandBuilder& enableDelayCompensation();

    /**
     * @brief Builds the final DeepFilter command string.
     *
     * Constructs the command string using the provided parameters and options.
     *
     * @return The constructed command string.
     *
     * @throws std::runtime_error if required parameters (input or output file) are missing.
     */
    std::string build() const override;

   private:
    std::string m_inputAudioPath;
    std::string m_outputAudioPath;
    double m_noiseReductionLevel = 0.5;
};

}  // namespace MediaProcessor

#endif