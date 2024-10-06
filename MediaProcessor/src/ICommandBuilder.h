#ifndef ICOMMANDBUILDER_H
#define ICOMMANDBUILDER_H

#include <string>

namespace MediaProcessor {

/*
 * Interface for building commands
 */
class ICommandBuilder {
   public:
    virtual ~ICommandBuilder() = default;

    virtual std::string build() const = 0;

    virtual void addArgument(const std::string& arg) = 0;
    virtual void addFlag(const std::string& flag) = 0;
    virtual void addFlag(const std::string& flag, const std::string& value) = 0;
};

}  // namespace MediaProcessor

#endif  // ICOMMANDBUILDER_H
