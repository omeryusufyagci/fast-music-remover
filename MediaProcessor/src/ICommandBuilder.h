#ifndef ICOMMANDBUILDER_H
#define ICOMMANDBUILDER_H

#include <string>

namespace MediaProcessor {

/**
 * @brief Interface for building command-line commands.
 */
class ICommandBuilder {
   public:
    virtual ~ICommandBuilder() = default;

    /**
     * @brief Constructs the command string.
     *
     * @return The constructed command string.
     */
    virtual std::string build() const = 0;

    /**
     * @brief Adds an argument to the command.
     */
    virtual ICommandBuilder& addArgument(const std::string& arg) = 0;

    /**
     * @brief Adds a flag to the command.
     */
    virtual ICommandBuilder& addFlag(const std::string& flag) = 0;

    /**
     * @brief Adds a flag with a specified value to the command.
     */
    virtual ICommandBuilder& addFlag(const std::string& flag, const std::string& value) = 0;
};

}  // namespace MediaProcessor

#endif  // ICOMMANDBUILDER_H
