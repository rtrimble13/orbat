#pragma once

#include "orbat/optimizer/black_litterman.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

#include "arg_parser.hpp"
#include "file_parser.hpp"

namespace orbat {
namespace cli {

/**
 * @brief Black-Litterman command implementation.
 *
 * Implements the 'bl' command for Bayesian portfolio optimization using
 * the Black-Litterman model.
 */
class BlCommand {
public:
    /**
     * @brief Execute the Black-Litterman command.
     * @param parser Argument parser containing command-line arguments
     * @return Exit code (0 for success, non-zero for error)
     */
    static int execute(const ArgParser& parser) {
        try {
            // Check for help flag
            if (parser.isHelp()) {
                printHelp();
                return 0;
            }

            // Parse required flags
            if (!parser.hasFlag("returns")) {
                throw std::runtime_error(
                    "Missing required flag: --returns (market equilibrium weights)");
            }
            if (!parser.hasFlag("covariance")) {
                throw std::runtime_error("Missing required flag: --covariance");
            }

            // Load input data
            // Note: For Black-Litterman, --returns actually contains market weights
            std::string marketWeightsFile = parser.getFlagValue("returns");
            std::string covarianceFile = parser.getFlagValue("covariance");

            // Parse as returns, but interpret as weights (both are vectors)
            auto marketWeightsData = FileParser::parseReturns(marketWeightsFile);
            auto covariance = FileParser::parseCovariance(covarianceFile);

            // Extract the underlying vector from ExpectedReturns
            core::Vector marketWeights(marketWeightsData.data());

            // Validate dimensions
            if (marketWeights.size() != covariance.size()) {
                throw std::runtime_error("Market weights and covariance dimensions do not match");
            }

            // Parse optional parameters
            double riskAversion = 2.5;  // Default risk aversion
            double tau = 0.025;         // Default uncertainty in prior

            if (parser.hasFlag("rf-rate")) {
                // Risk-free rate not directly used in Black-Litterman
                // but could be used for Sharpe ratio calculation
            }

            // Create Black-Litterman optimizer
            optimizer::BlackLittermanOptimizer blOptimizer(marketWeights, covariance, riskAversion,
                                                           tau);

            // Note: View specification is not yet implemented in CLI.
            // Planned feature: Support loading investor views from JSON/CSV files
            // to incorporate subjective beliefs about expected returns.
            // For now, the optimizer uses only the equilibrium returns derived from market weights.
            auto result = blOptimizer.optimize();

            // Output results
            std::string outputFile = parser.getFlagValue("output", "");
            if (outputFile.empty()) {
                // Print to stdout
                printResult(blOptimizer, result);
            } else {
                // Write to file
                writeResult(result, outputFile);
                std::cout << "Results written to: " << outputFile << std::endl;
            }

            return 0;

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            std::cerr << "Use 'orbat bl --help' for usage information." << std::endl;
            return 1;
        }
    }

    /**
     * @brief Print help message for Black-Litterman command.
     */
    static void printHelp() {
        std::cout << "Usage: orbat bl [OPTIONS]\n"
                  << "\n"
                  << "Black-Litterman Portfolio Optimization\n"
                  << "\n"
                  << "Required Options:\n"
                  << "  --returns <file>       Path to market equilibrium weights CSV file\n"
                  << "  --covariance <file>    Path to covariance matrix CSV file\n"
                  << "\n"
                  << "Optional Flags:\n"
                  << "  --rf-rate <value>      Risk-free rate (for Sharpe ratio, default: 0.0)\n"
                  << "  --constraints <file>   Path to constraints file (not yet implemented)\n"
                  << "  --output <file>        Output file (JSON format, default: stdout)\n"
                  << "  --help, -h             Show this help message\n"
                  << "\n"
                  << "Note: The --returns file should contain market capitalization weights,\n"
                  << "      not expected returns. The Black-Litterman model computes implied\n"
                  << "      equilibrium returns from these weights.\n"
                  << "\n"
                  << "Examples:\n"
                  << "  orbat bl --returns market_weights.csv --covariance cov.csv\n"
                  << "  orbat bl --returns market_weights.csv --covariance cov.csv --output "
                     "result.json\n";
    }

private:
    /**
     * @brief Print optimization result to stdout.
     */
    static void printResult(const optimizer::BlackLittermanOptimizer& blOptimizer,
                            const optimizer::MarkowitzResult& result) {
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "\n=== Black-Litterman Portfolio Optimization ===" << std::endl;
        std::cout << "\nStatus: " << (result.success() ? "SUCCESS" : "FAILED") << std::endl;
        if (!result.message.empty()) {
            std::cout << "Message: " << result.message << std::endl;
        }

        // Show equilibrium returns
        std::cout << "\nImplied Equilibrium Returns:" << std::endl;
        const auto& eqReturns = blOptimizer.equilibriumReturns();
        for (size_t i = 0; i < eqReturns.size(); ++i) {
            std::cout << "  Asset " << (i + 1) << ": " << eqReturns[i] * 100 << "%" << std::endl;
        }

        std::cout << "\nPortfolio Metrics:" << std::endl;
        std::cout << "  Expected Return:  " << result.expectedReturn * 100 << "%" << std::endl;
        std::cout << "  Risk (Std Dev):   " << result.risk * 100 << "%" << std::endl;
        std::cout << "  Sharpe Ratio:     " << result.sharpeRatio << std::endl;

        std::cout << "\nOptimal Weights:" << std::endl;
        for (size_t i = 0; i < result.weights.size(); ++i) {
            std::cout << "  Asset " << (i + 1) << ": " << result.weights[i] * 100 << "%"
                      << std::endl;
        }
        std::cout << std::endl;
    }

    /**
     * @brief Write optimization result to file in JSON format.
     */
    static void writeResult(const optimizer::MarkowitzResult& result, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open output file: " + filename);
        }
        file << result.toJSON();
        file.close();
    }
};

}  // namespace cli
}  // namespace orbat
