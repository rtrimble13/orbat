#pragma once

#include "orbat/optimizer/constraint.hpp"
#include "orbat/optimizer/markowitz.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "arg_parser.hpp"
#include "file_parser.hpp"

namespace orbat {
namespace cli {

/**
 * @brief Modern Portfolio Theory command implementation.
 *
 * Implements the 'mpt' command for mean-variance optimization using
 * the Markowitz optimizer.
 */
class MptCommand {
public:
    /**
     * @brief Execute the MPT command.
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
                throw std::runtime_error("Missing required flag: --returns");
            }
            if (!parser.hasFlag("covariance")) {
                throw std::runtime_error("Missing required flag: --covariance");
            }

            // Load input data
            std::string returnsFile = parser.getFlagValue("returns");
            std::string covarianceFile = parser.getFlagValue("covariance");

            auto returns = FileParser::parseReturns(returnsFile);
            auto covariance = FileParser::parseCovariance(covarianceFile);

            // Validate dimensions
            if (returns.size() != covariance.size()) {
                throw std::runtime_error("Returns and covariance dimensions do not match");
            }

            // Parse optional risk-free rate
            double riskFreeRate = 0.0;
            if (parser.hasFlag("rf-rate")) {
                try {
                    riskFreeRate = std::stod(parser.getFlagValue("rf-rate"));
                } catch (const std::exception&) {
                    throw std::runtime_error("Invalid risk-free rate value");
                }
            }

            // Setup constraints (default: long-only)
            optimizer::ConstraintSet constraints;
            constraints.add(std::make_shared<optimizer::LongOnlyConstraint>());

            // Parse additional constraints if provided
            // Note: Custom constraints file parsing is not yet implemented.
            // Planned feature: Support loading constraint definitions from JSON/CSV files
            // to specify box constraints, sector constraints, and other custom limitations.
            if (parser.hasFlag("constraints")) {
                std::cerr << "Warning: --constraints flag is not yet implemented. Using default "
                             "long-only constraint."
                          << std::endl;
            }

            // Create optimizer
            optimizer::MarkowitzOptimizer optimizer(returns, covariance, constraints);

            // Run minimum variance optimization
            auto result = optimizer.minimumVariance();

            // Update Sharpe ratio with custom risk-free rate if provided
            if (riskFreeRate != 0.0) {
                result.setRiskFreeRate(riskFreeRate);
            }

            // Output results
            std::string outputFile = parser.getFlagValue("output", "");
            if (outputFile.empty()) {
                // Print to stdout
                printResult(result, riskFreeRate);
            } else {
                // Write to file
                writeResult(result, outputFile);
                std::cout << "Results written to: " << outputFile << std::endl;
            }

            return 0;

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            std::cerr << "Use 'orbat mpt --help' for usage information." << std::endl;
            return 1;
        }
    }

    /**
     * @brief Print help message for MPT command.
     */
    static void printHelp() {
        std::cout << "Usage: orbat mpt [OPTIONS]\n"
                  << "\n"
                  << "Modern Portfolio Theory (Mean-Variance) Optimization\n"
                  << "\n"
                  << "Required Options:\n"
                  << "  --returns <file>       Path to returns CSV file\n"
                  << "  --covariance <file>    Path to covariance matrix CSV file\n"
                  << "\n"
                  << "Optional Flags:\n"
                  << "  --rf-rate <value>      Risk-free rate (default: 0.0)\n"
                  << "  --constraints <file>   Path to constraints file (not yet implemented)\n"
                  << "  --output <file>        Output file (JSON format, default: stdout)\n"
                  << "  --help, -h             Show this help message\n"
                  << "\n"
                  << "Examples:\n"
                  << "  orbat mpt --returns returns.csv --covariance cov.csv\n"
                  << "  orbat mpt --returns returns.csv --covariance cov.csv --rf-rate 0.02 "
                     "--output result.json\n";
    }

private:
    /**
     * @brief Print optimization result to stdout.
     */
    static void printResult(const optimizer::MarkowitzResult& result, double riskFreeRate) {
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "\n=== Modern Portfolio Theory Optimization ===" << std::endl;
        std::cout << "\nStatus: " << (result.success() ? "SUCCESS" : "FAILED") << std::endl;
        if (!result.message.empty()) {
            std::cout << "Message: " << result.message << std::endl;
        }
        std::cout << "\nPortfolio Metrics:" << std::endl;
        std::cout << "  Expected Return:  " << result.expectedReturn * 100 << "%" << std::endl;
        std::cout << "  Risk (Std Dev):   " << result.risk * 100 << "%" << std::endl;
        std::cout << "  Sharpe Ratio:     " << result.sharpeRatio;
        if (riskFreeRate != 0.0) {
            std::cout << " (rf=" << riskFreeRate * 100 << "%)";
        }
        std::cout << std::endl;
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
