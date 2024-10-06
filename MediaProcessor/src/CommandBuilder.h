#ifndef COMMANDBUILDER_H
#define COMMANDBUILDER_H

#include <filesystem>
#include <string>
#include <vector>

#include "ICommandBuilder.h"

namespace MediaProcessor {

class CommandBuilder : public ICommandBuilder {
   public:
    virtual ~CommandBuilder() = default;

    void addArgument(std::filesystem::path arg) override;
    void addFlag(std::string flag) override;
    void addFlag(std::string flag, std::string value) override;

    std::string build() const override;

   protected:
    std::string formatArgument(const std::string &arg) const;

    std::vector<std::string> m_arguments;
};

}  // namespace MediaProcessor

#endif  // COMMANDBUILDER_H
