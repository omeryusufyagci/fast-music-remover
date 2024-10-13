#include <gtest/gtest.h>

#include "../src/CommandBuilder.h"

namespace MediaProcessor::Testing {

class CommandBuilderTest : public ::testing::Test {
   protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(CommandBuilderTest, buildCommand) {
    CommandBuilder builder;
    builder.addArgument("arg1");
    builder.addFlag("flag");
    builder.addArgument("arg2");
    builder.addFlag("flag2", "value");
    std::string command = builder.build();
    EXPECT_EQ(command, "arg1 flag arg2 flag2 value");
}

}  // namespace MediaProcessor::Testing