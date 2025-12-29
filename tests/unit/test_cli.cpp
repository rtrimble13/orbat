#include "cli/arg_parser.hpp"
#include "cli/bl_command.hpp"
#include "cli/mpt_command.hpp"
#include <gtest/gtest.h>

using namespace orbat::cli;

// Test ArgParser functionality
TEST(ArgParserTest, EmptyArgs) {
    char* argv[] = {const_cast<char*>("orbat")};
    ArgParser parser(1, argv);
    EXPECT_EQ(parser.count(), 0);
    EXPECT_EQ(parser.getCommand(), "");
    EXPECT_FALSE(parser.isHelp());
}

TEST(ArgParserTest, CommandOnly) {
    char* argv[] = {const_cast<char*>("orbat"), const_cast<char*>("mpt")};
    ArgParser parser(2, argv);
    EXPECT_EQ(parser.count(), 1);
    EXPECT_EQ(parser.getCommand(), "mpt");
}

TEST(ArgParserTest, CommandWithFlags) {
    char* argv[] = {const_cast<char*>("orbat"),        const_cast<char*>("mpt"),
                    const_cast<char*>("--returns"),    const_cast<char*>("returns.csv"),
                    const_cast<char*>("--covariance"), const_cast<char*>("cov.csv")};
    ArgParser parser(6, argv);
    EXPECT_EQ(parser.getCommand(), "mpt");
    EXPECT_TRUE(parser.hasFlag("returns"));
    EXPECT_TRUE(parser.hasFlag("covariance"));
    EXPECT_EQ(parser.getFlagValue("returns"), "returns.csv");
    EXPECT_EQ(parser.getFlagValue("covariance"), "cov.csv");
}

TEST(ArgParserTest, HelpFlag) {
    char* argv[] = {const_cast<char*>("orbat"), const_cast<char*>("--help")};
    ArgParser parser(2, argv);
    EXPECT_TRUE(parser.isHelp());
}

TEST(ArgParserTest, ShortHelpFlag) {
    char* argv[] = {const_cast<char*>("orbat"), const_cast<char*>("--h")};
    ArgParser parser(2, argv);
    EXPECT_TRUE(parser.hasFlag("h"));
}

TEST(ArgParserTest, MissingFlagValue) {
    char* argv[] = {const_cast<char*>("orbat"), const_cast<char*>("mpt"),
                    const_cast<char*>("--returns")};
    ArgParser parser(3, argv);
    EXPECT_THROW(parser.getFlagValue("returns"), std::runtime_error);
}

TEST(ArgParserTest, NonexistentFlag) {
    char* argv[] = {const_cast<char*>("orbat"), const_cast<char*>("mpt")};
    ArgParser parser(2, argv);
    EXPECT_THROW(parser.getFlagValue("returns"), std::runtime_error);
}

TEST(ArgParserTest, DefaultFlagValue) {
    char* argv[] = {const_cast<char*>("orbat"), const_cast<char*>("mpt")};
    ArgParser parser(2, argv);
    EXPECT_EQ(parser.getFlagValue("output", "default.json"), "default.json");
}

TEST(ArgParserTest, OptionalFlagWithValue) {
    char* argv[] = {const_cast<char*>("orbat"), const_cast<char*>("mpt"),
                    const_cast<char*>("--rf-rate"), const_cast<char*>("0.02")};
    ArgParser parser(4, argv);
    EXPECT_EQ(parser.getFlagValue("rf-rate"), "0.02");
}

// Test command help output (basic sanity checks)
TEST(MptCommandTest, HelpOutput) {
    // This test just verifies that help can be called without crashing
    // In a real scenario, we'd capture stdout and verify content
    EXPECT_NO_THROW(MptCommand::printHelp());
}

TEST(BlCommandTest, HelpOutput) {
    // This test just verifies that help can be called without crashing
    EXPECT_NO_THROW(BlCommand::printHelp());
}

// Test invalid flag detection
TEST(MptCommandTest, MissingRequiredFlags) {
    char* argv[] = {const_cast<char*>("orbat"), const_cast<char*>("mpt")};
    ArgParser parser(2, argv);

    // Should fail without required flags
    int result = MptCommand::execute(parser);
    EXPECT_NE(result, 0);  // Non-zero exit code indicates error
}

TEST(BlCommandTest, MissingRequiredFlags) {
    char* argv[] = {const_cast<char*>("orbat"), const_cast<char*>("bl")};
    ArgParser parser(2, argv);

    // Should fail without required flags
    int result = BlCommand::execute(parser);
    EXPECT_NE(result, 0);  // Non-zero exit code indicates error
}

// Test help flag takes precedence
TEST(MptCommandTest, HelpFlagTakesPrecedence) {
    char* argv[] = {const_cast<char*>("orbat"), const_cast<char*>("mpt"),
                    const_cast<char*>("--help")};
    ArgParser parser(3, argv);

    // Should succeed with help flag (returns 0)
    int result = MptCommand::execute(parser);
    EXPECT_EQ(result, 0);
}

TEST(BlCommandTest, HelpFlagTakesPrecedence) {
    char* argv[] = {const_cast<char*>("orbat"), const_cast<char*>("bl"),
                    const_cast<char*>("--help")};
    ArgParser parser(3, argv);

    // Should succeed with help flag (returns 0)
    int result = BlCommand::execute(parser);
    EXPECT_EQ(result, 0);
}
