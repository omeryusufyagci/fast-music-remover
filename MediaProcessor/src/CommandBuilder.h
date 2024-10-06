#ifndef COMMANDBUILDER_H
#define COMMANDBUILDER_H

#include <string>
#include <vector>

#include "ICommandBuilder.h"

namespace MediaProcessor {

class CommandBuilder : public ICommandBuilder {
   public:
    void addArgument(const std::string& arg) override;
    void addFlag(const std::string& flag) override;
    void addFlag(const std::string& flag, const std::string& value) override;
    std::string build() const override;

   private:
    std::vector<std::string> m_arguments;

    std::string formatArgument(const std::string& arg) const;
};

}  // namespace MediaProcessor

#endif  // COMMANDBUILDER_H
