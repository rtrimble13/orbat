#include <iostream>
#include <string>

#include "arg_parser.hpp"
#include "bl_command.hpp"
#include "mpt_command.hpp"

using namespace orbat::cli;

/**
 * @brief Print general help message for the CLI.
 */
void printHelp() {
    std::cout << "orbat - Portfolio Optimization Command Line Tool\n"
              << "\n"
              << "Usage: orbat <command> [options]\n"
              << "\n"
              << "Available Commands:\n"
              << "  mpt        Modern Portfolio Theory (Mean-Variance) optimization\n"
              << "  bl         Black-Litterman portfolio optimization\n"
              << "\n"
              << "Options:\n"
              << "  --help, -h Show help for the command\n"
              << "\n"
              << "Examples:\n"
              << "  orbat mpt --help\n"
              << "  orbat bl --help\n"
              << "  orbat mpt --returns returns.csv --covariance cov.csv\n"
              << "  orbat bl --returns market_weights.csv --covariance cov.csv\n"
              << "\n"
              << "For more information, visit: https://github.com/rtrimble13/orbat\n";
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    ArgParser parser(argc, argv);

    // Check if no arguments provided
    if (parser.count() == 0) {
        printHelp();
        return 0;
    }

    // Get the command
    std::string command = parser.getCommand();

    // Check if help requested without a command
    if (command.empty() && parser.isHelp()) {
        printHelp();
        return 0;
    }

    if (command.empty()) {
        std::cerr << "Error: No command specified" << std::endl;
        std::cerr << "Use 'orbat --help' for usage information." << std::endl;
        return 1;
    }

    // Route to appropriate command handler
    if (command == "mpt") {
        return MptCommand::execute(parser);
    } else if (command == "bl") {
        return BlCommand::execute(parser);
    } else {
        std::cerr << "Error: Unknown command '" << command << "'" << std::endl;
        std::cerr << "Use 'orbat --help' for available commands." << std::endl;
        return 1;
    }
}
