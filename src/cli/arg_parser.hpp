#pragma once

#include <algorithm>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace orbat {
namespace cli {

/**
 * @brief Simple command-line argument parser.
 *
 * Provides basic functionality for parsing command-line arguments,
 * including flags with values and boolean flags.
 */
class ArgParser {
public:
    /**
     * @brief Construct a new ArgParser from command line arguments.
     * @param argc Argument count
     * @param argv Argument values
     */
    ArgParser(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            args_.emplace_back(argv[i]);
        }
    }

    /**
     * @brief Get the command (first non-flag argument).
     * @return The command string, or empty if none provided
     */
    std::string getCommand() const {
        if (args_.empty() || args_[0].rfind("-", 0) == 0) {
            return "";
        }
        return args_[0];
    }

    /**
     * @brief Check if a flag is present.
     * @param flag Flag name (without -- or -)
     * @return true if flag is present
     */
    bool hasFlag(const std::string& flag) const {
        // Support both --flag and -f (single char) formats
        std::string fullFlag = (flag.length() == 1 ? "-" : "--") + flag;
        return std::find(args_.begin(), args_.end(), fullFlag) != args_.end();
    }

    /**
     * @brief Get the value of a flag.
     * @param flag Flag name (without -- or -)
     * @return The value following the flag
     * @throws std::runtime_error if flag not found or has no value
     */
    std::string getFlagValue(const std::string& flag) const {
        // Support both --flag and -f (single char) formats
        std::string fullFlag = (flag.length() == 1 ? "-" : "--") + flag;
        auto it = std::find(args_.begin(), args_.end(), fullFlag);
        if (it == args_.end()) {
            throw std::runtime_error("Flag " + fullFlag + " not found");
        }
        ++it;
        if (it == args_.end()) {
            throw std::runtime_error("Flag " + fullFlag + " has no value");
        }
        return *it;
    }

    /**
     * @brief Get the value of a flag with a default.
     * @param flag Flag name (without -- or -)
     * @param defaultValue Default value if flag not present
     * @return The flag value or default
     */
    std::string getFlagValue(const std::string& flag, const std::string& defaultValue) const {
        try {
            return getFlagValue(flag);
        } catch (const std::runtime_error&) {
            return defaultValue;
        }
    }

    /**
     * @brief Get all arguments as a vector.
     * @return Vector of all arguments
     */
    const std::vector<std::string>& getArgs() const { return args_; }

    /**
     * @brief Check if help was requested.
     * @return true if --help or -h flag is present
     */
    bool isHelp() const { return hasFlag("help") || hasFlag("h"); }

    /**
     * @brief Get count of arguments.
     * @return Number of arguments (excluding program name)
     */
    size_t count() const { return args_.size(); }

private:
    std::vector<std::string> args_;
};

}  // namespace cli
}  // namespace orbat
