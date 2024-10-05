#ifndef ICOMMANDBUILDER_H
#define ICOMMANDBUILDER_H

#include <filesystem>
#include <string>

namespace MediaProcessor {

/*
 * Interface for building commands
 */
class ICommandBuilder {
   public:
    virtual ~ICommandBuilder() = default;

    virtual std::string build() const = 0;

    virtual void addArgument(std::filesystem::path arg) = 0;
    virtual void addFlag(std::string flag) = 0;
    virtual void addFlag(std::string flag, std::filesystem::path value) = 0;
    virtual void addFlag(std::string flag, std::string value) = 0;
};

}  // namespace MediaProcessor

#endif  // ICOMMANDBUILDER_H
