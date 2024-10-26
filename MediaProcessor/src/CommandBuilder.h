#ifndef COMMANDBUILDER_H
#define COMMANDBUILDER_H

#include <string>
#include <vector>

#include "ICommandBuilder.h"

namespace MediaProcessor {

/**
 * @brief Builds command-line commands by adding arguments and flags.
 */
class CommandBuilder : public ICommandBuilder {
   public:
    CommandBuilder& addArgument(const std::string& arg) override;
    CommandBuilder& addFlag(const std::string& flag) override;
    CommandBuilder& addFlag(const std::string& flag, const std::string& value) override;

    /**
     * @brief Constructs the command string from the added arguments and flags.
     */
    std::string build() const override;

   private:
    std::vector<std::string> m_arguments;

    std::string formatArgument(const std::string& arg) const;
};

}  // namespace MediaProcessor

#endif  // COMMANDBUILDER_H
