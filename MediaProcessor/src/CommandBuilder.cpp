#include "CommandBuilder.h"

#include <sstream>

#include "Utils.h"

namespace MediaProcessor {

void CommandBuilder::addArgument(std::filesystem::path arg) {
    m_arguments.push_back(std::move(arg).string());
}

void CommandBuilder::addFlag(std::string flag) {
    m_arguments.push_back(std::move(flag));
}

void CommandBuilder::addFlag(std::string flag, std::filesystem::path value) {
    m_arguments.push_back(std::move(flag));
    m_arguments.push_back(std::move(value).string());
}

void CommandBuilder::addFlag(std::string flag, std::string value) {
    m_arguments.push_back(std::move(flag));
    m_arguments.push_back(std::move(value));
}

std::string CommandBuilder::build() const {
    using MediaProcessor::Utils::trimTrailingSpace;

    std::ostringstream command;
    for (const auto &arg : m_arguments) {
        command << formatArgument(arg) << " ";
    }
    return trimTrailingSpace(command.str());
}

std::string CommandBuilder::formatArgument(const std::string &arg) const {
    using MediaProcessor::Utils::containsWhitespace;

    if (containsWhitespace(arg)) {
        return "\"" + arg + "\"";
    }
    return arg;
}

}  // namespace MediaProcessor
