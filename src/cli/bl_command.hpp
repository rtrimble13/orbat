#pragma once

#include "orbat/optimizer/black_litterman.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

#include "arg_parser.hpp"
#include "error_codes.hpp"
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
                return static_cast<int>(ExitCode::SUCCESS);
            }

            // Parse required flags
            if (!parser.hasFlag("returns")) {
                std::cerr << "Error: Missing required input - Market equilibrium weights not "
                             "provided"
                          << std::endl;
                std::cerr << "Usage: Use --returns <file> to specify market weights CSV file"
                          << std::endl;
                std::cerr << "Run 'orbat bl --help' for more information." << std::endl;
                return static_cast<int>(ExitCode::INVALID_ARGUMENTS);
            }
            if (!parser.hasFlag("covariance")) {
                std::cerr << "Error: Missing required input - Covariance matrix not provided"
                          << std::endl;
                std::cerr << "Usage: Use --covariance <file> to specify covariance matrix CSV file"
                          << std::endl;
                std::cerr << "Run 'orbat bl --help' for more information." << std::endl;
                return static_cast<int>(ExitCode::INVALID_ARGUMENTS);
            }

            // Load input data
            // Note: For Black-Litterman, --returns actually contains market weights
            std::string marketWeightsFile = parser.getFlagValue("returns");
            std::string covarianceFile = parser.getFlagValue("covariance");

            // Parse market weights with enhanced error handling
            optimizer::ExpectedReturns marketWeightsData;
            try {
                marketWeightsData = FileParser::parseReturns(marketWeightsFile);
                if (marketWeightsData.empty()) {
                    std::cerr << "Error: Empty market weights - File '" << marketWeightsFile
                              << "' contains no valid data" << std::endl;
                    std::cerr << "Expected: A CSV file with numeric weight values summing to 1.0"
                              << std::endl;
                    return static_cast<int>(ExitCode::VALIDATION_ERROR);
                }
            } catch (const std::runtime_error& e) {
                std::cerr << "Error: Failed to load market weights from '" << marketWeightsFile
                          << "'" << std::endl;
                std::cerr << "Details: " << e.what() << std::endl;
                std::cerr << "Hint: Check that the file exists and contains valid numeric data"
                          << std::endl;
                return static_cast<int>(ExitCode::VALIDATION_ERROR);
            }

            // Parse covariance with enhanced error handling
            optimizer::CovarianceMatrix covariance;
            try {
                covariance = FileParser::parseCovariance(covarianceFile);
                if (covariance.empty()) {
                    std::cerr << "Error: Empty covariance matrix - File '" << covarianceFile
                              << "' contains no valid data" << std::endl;
                    std::cerr << "Expected: A CSV file with an NxN symmetric matrix of covariances"
                              << std::endl;
                    return static_cast<int>(ExitCode::VALIDATION_ERROR);
                }
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error: Invalid covariance matrix in file '" << covarianceFile << "'"
                          << std::endl;
                std::cerr << "Details: " << e.what() << std::endl;
                std::cerr << "Hint: The covariance matrix must be square, symmetric, and "
                             "positive-definite"
                          << std::endl;
                return static_cast<int>(ExitCode::VALIDATION_ERROR);
            } catch (const std::runtime_error& e) {
                std::cerr << "Error: Failed to load covariance matrix from '" << covarianceFile
                          << "'" << std::endl;
                std::cerr << "Details: " << e.what() << std::endl;
                std::cerr << "Hint: Check that the file exists and contains a valid NxN matrix"
                          << std::endl;
                return static_cast<int>(ExitCode::VALIDATION_ERROR);
            }

            // Extract the underlying vector from ExpectedReturns
            core::Vector marketWeights(marketWeightsData.data());

            // Validate dimensions
            if (marketWeights.size() != covariance.size()) {
                std::cerr << "Error: Dimension mismatch - Market weights and covariance dimensions "
                             "do not match"
                          << std::endl;
                std::cerr << "Details: Market weights has " << marketWeights.size()
                          << " assets, but covariance matrix is " << covariance.size() << "x"
                          << covariance.size() << std::endl;
                std::cerr << "Hint: Both files must describe the same number of assets"
                          << std::endl;
                return static_cast<int>(ExitCode::VALIDATION_ERROR);
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

            // Check if optimization succeeded
            if (!result.success()) {
                std::cerr << "Error: Optimization failed" << std::endl;
                if (!result.message.empty()) {
                    std::cerr << "Details: " << result.message << std::endl;
                }
                return static_cast<int>(ExitCode::COMPUTATION_ERROR);
            }

            // Output results
            std::string outputFile = parser.getFlagValue("output", "");
            if (outputFile.empty()) {
                // Print to stdout
                printResult(blOptimizer, result);
            } else {
                // Write to file
                try {
                    writeResult(result, outputFile);
                    std::cout << "Results written to: " << outputFile << std::endl;
                } catch (const std::runtime_error& e) {
                    std::cerr << "Error: Failed to write output to '" << outputFile << "'"
                              << std::endl;
                    std::cerr << "Details: " << e.what() << std::endl;
                    return static_cast<int>(ExitCode::VALIDATION_ERROR);
                }
            }

            return static_cast<int>(ExitCode::SUCCESS);

        } catch (const std::exception& e) {
            std::cerr << "Error: Unexpected error occurred" << std::endl;
            std::cerr << "Details: " << e.what() << std::endl;
            std::cerr << "Use 'orbat bl --help' for usage information." << std::endl;
            return static_cast<int>(ExitCode::INTERNAL_ERROR);
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
