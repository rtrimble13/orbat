#pragma once

#include "orbat/core/matrix.hpp"
#include "orbat/core/vector.hpp"
#include "orbat/optimizer/covariance_matrix.hpp"
#include "orbat/optimizer/expected_returns.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace orbat {
namespace cli {

/**
 * @brief File parser utilities for CLI input.
 *
 * Provides functions to parse returns and covariance matrices from CSV files.
 */
class FileParser {
public:
    /**
     * @brief Parse a returns vector from a CSV file.
     *
     * Expected format: single column of numbers or comma-separated values.
     * Comments starting with # are ignored.
     *
     * @param filename Path to the CSV file
     * @return ExpectedReturns object
     * @throws std::runtime_error if file cannot be read or format is invalid
     */
    static optimizer::ExpectedReturns parseReturns(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open returns file: " + filename);
        }

        std::vector<double> returns;
        std::string line;

        while (std::getline(file, line)) {
            // Skip empty lines and trim whitespace
            size_t start = line.find_first_not_of(" \t\r\n");
            if (start == std::string::npos || line.empty()) {
                continue;
            }
            // Skip comments (lines starting with # after whitespace)
            if (line[start] == '#') {
                continue;
            }

            // Parse comma-separated values
            std::istringstream iss(line);
            std::string token;
            while (std::getline(iss, token, ',')) {
                try {
                    returns.push_back(std::stod(token));
                } catch (const std::exception&) {
                    throw std::runtime_error("Invalid number in returns file: " + token);
                }
            }
        }

        if (returns.empty()) {
            throw std::runtime_error("No valid returns data found in file: " + filename);
        }

        return optimizer::ExpectedReturns(core::Vector(returns));
    }

    /**
     * @brief Parse a covariance matrix from a CSV file.
     *
     * Expected format: N rows of N comma-separated values (square matrix).
     * Comments starting with # are ignored.
     *
     * @param filename Path to the CSV file
     * @return CovarianceMatrix object
     * @throws std::runtime_error if file cannot be read or format is invalid
     */
    static optimizer::CovarianceMatrix parseCovariance(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open covariance file: " + filename);
        }

        std::vector<std::vector<double>> matrix;
        std::string line;

        while (std::getline(file, line)) {
            // Skip empty lines and trim whitespace
            size_t start = line.find_first_not_of(" \t\r\n");
            if (start == std::string::npos || line.empty()) {
                continue;
            }
            // Skip comments (lines starting with # after whitespace)
            if (line[start] == '#') {
                continue;
            }

            std::vector<double> row;
            std::istringstream iss(line);
            std::string token;

            while (std::getline(iss, token, ',')) {
                try {
                    row.push_back(std::stod(token));
                } catch (const std::exception&) {
                    throw std::runtime_error("Invalid number in covariance file: " + token);
                }
            }

            if (!row.empty()) {
                matrix.push_back(row);
            }
        }

        if (matrix.empty()) {
            throw std::runtime_error("No valid covariance data found in file: " + filename);
        }

        // Validate square matrix
        size_t n = matrix.size();
        for (const auto& row : matrix) {
            if (row.size() != n) {
                throw std::runtime_error("Covariance matrix must be square");
            }
        }

        // Construct Matrix from vector of vectors
        core::Matrix mat(n, n);
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                mat(i, j) = matrix[i][j];
            }
        }

        return optimizer::CovarianceMatrix(mat);
    }
};

}  // namespace cli
}  // namespace orbat
