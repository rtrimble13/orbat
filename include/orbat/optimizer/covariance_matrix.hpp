#pragma once

#include "orbat/core/constants.hpp"
#include "orbat/core/matrix.hpp"

#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace orbat {
namespace optimizer {

/**
 * @brief Container for covariance matrix of asset returns.
 *
 * This class provides a standardized interface for managing covariance matrices
 * with support for CSV and JSON import/export and validation.
 *
 * A covariance matrix represents the pairwise covariances between asset returns
 * and is symmetric and positive semi-definite (positive-definite for invertibility).
 *
 * Example:
 *   CovarianceMatrix cov = CovarianceMatrix::fromCSV("covariance.csv");
 *   size_t n = cov.size();
 *   const Matrix& data = cov.data();
 */
class CovarianceMatrix {
public:
    /**
     * @brief Construct empty covariance matrix.
     */
    CovarianceMatrix() = default;

    /**
     * @brief Construct from a matrix.
     * @param matrix Covariance matrix
     * @throws std::invalid_argument if matrix is not square, symmetric, or positive-definite
     */
    explicit CovarianceMatrix(const core::Matrix& matrix) : matrix_(matrix) { validate(); }

    /**
     * @brief Construct from a matrix (move).
     * @param matrix Covariance matrix
     * @throws std::invalid_argument if matrix is not square, symmetric, or positive-definite
     */
    explicit CovarianceMatrix(core::Matrix&& matrix) : matrix_(std::move(matrix)) { validate(); }

    /**
     * @brief Construct from initializer list.
     * @param matrix Initializer list representing matrix rows
     * @throws std::invalid_argument if matrix is not square, symmetric, or positive-definite
     */
    CovarianceMatrix(std::initializer_list<std::initializer_list<double>> matrix)
        : matrix_(matrix) {
        validate();
    }

    /**
     * @brief Get the dimension (number of assets).
     * @return Number of assets
     */
    size_t size() const { return matrix_.rows(); }

    /**
     * @brief Check if empty.
     * @return true if empty, false otherwise
     */
    bool empty() const { return matrix_.empty(); }

    /**
     * @brief Get the underlying matrix (const).
     * @return Const reference to covariance matrix
     */
    const core::Matrix& data() const { return matrix_; }

    /**
     * @brief Get the underlying matrix.
     * @return Reference to covariance matrix
     */
    core::Matrix& data() { return matrix_; }

    /**
     * @brief Access element (const).
     * @param i Row index
     * @param j Column index
     * @return Covariance value
     */
    double operator()(size_t i, size_t j) const { return matrix_(i, j); }

    /**
     * @brief Access element (non-const).
     * @param i Row index
     * @param j Column index
     * @return Reference to covariance value
     */
    double& operator()(size_t i, size_t j) { return matrix_(i, j); }

    /**
     * @brief Load covariance matrix from CSV file.
     *
     * CSV format should be a square matrix of numbers:
     *   0.04,0.01,0.005
     *   0.01,0.0225,0.008
     *   0.005,0.008,0.01
     *
     * Optional header row is detected and skipped if all values are non-numeric.
     *
     * @param filename Path to CSV file
     * @return CovarianceMatrix object
     * @throws std::runtime_error if file cannot be opened or parsed
     * @throws std::invalid_argument if data is invalid or not square/symmetric
     */
    static CovarianceMatrix fromCSV(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }

        std::vector<std::vector<double>> rows;
        std::string line;
        bool first_line = true;

        while (std::getline(file, line)) {
            // Skip empty lines
            if (line.empty()) {
                continue;
            }

            std::vector<double> row;
            std::istringstream iss(line);
            std::string token;

            while (std::getline(iss, token, ',')) {
                token = trim(token);
                if (token.empty()) {
                    continue;
                }

                // Try to parse as number
                try {
                    double value = std::stod(token);
                    row.push_back(value);
                } catch (const std::exception& e) {
                    // If first line and cannot parse, it's a header
                    if (first_line) {
                        row.clear();
                        break;
                    }
                    throw std::runtime_error("Invalid numeric value in CSV: " + token);
                }
            }

            first_line = false;

            if (!row.empty()) {
                rows.push_back(row);
            }
        }

        if (rows.empty()) {
            throw std::runtime_error("No valid data found in CSV file: " + filename);
        }

        // Convert to Matrix
        size_t n = rows.size();
        size_t m = rows[0].size();

        if (n != m) {
            throw std::invalid_argument("Covariance matrix must be square (got " +
                                        std::to_string(n) + "x" + std::to_string(m) + ")");
        }

        core::Matrix matrix(n, m);
        for (size_t i = 0; i < n; ++i) {
            if (rows[i].size() != m) {
                throw std::invalid_argument("All rows must have the same length");
            }
            for (size_t j = 0; j < m; ++j) {
                matrix(i, j) = rows[i][j];
            }
        }

        return CovarianceMatrix(std::move(matrix));
    }

    /**
     * @brief Load covariance matrix from JSON file.
     *
     * JSON format should be a 2D array:
     *   [[0.04, 0.01, 0.005],
     *    [0.01, 0.0225, 0.008],
     *    [0.005, 0.008, 0.01]]
     *
     * @param filename Path to JSON file
     * @return CovarianceMatrix object
     * @throws std::runtime_error if file cannot be opened or parsed
     * @throws std::invalid_argument if data is invalid or not square/symmetric
     */
    static CovarianceMatrix fromJSON(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }

        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());

        return fromJSONString(content);
    }

    /**
     * @brief Parse covariance matrix from JSON string.
     *
     * Supports same formats as fromJSON().
     *
     * @param json_str JSON string
     * @return CovarianceMatrix object
     * @throws std::runtime_error if JSON cannot be parsed
     * @throws std::invalid_argument if data is invalid or not square/symmetric
     */
    static CovarianceMatrix fromJSONString(const std::string& json_str) {
        // Simple JSON parser for 2D array format
        std::string trimmed = trim(json_str);

        // Look for array start
        size_t array_start = trimmed.find('[');
        if (array_start == std::string::npos) {
            throw std::runtime_error("Invalid JSON: expected array");
        }

        // Find the outer array bounds
        size_t outer_end = trimmed.rfind(']');
        if (outer_end == std::string::npos) {
            throw std::runtime_error("Invalid JSON: unclosed array");
        }

        // Parse rows
        std::vector<std::vector<double>> rows;
        size_t pos = array_start + 1;

        while (pos < outer_end) {
            // Skip whitespace
            while (pos < outer_end && std::isspace(trimmed[pos])) {
                ++pos;
            }

            if (pos >= outer_end) {
                break;
            }

            // Look for inner array
            if (trimmed[pos] == '[') {
                size_t row_start = pos;
                size_t row_end = trimmed.find(']', row_start);
                if (row_end == std::string::npos || row_end > outer_end) {
                    throw std::runtime_error("Invalid JSON: unclosed inner array");
                }

                // Extract row content
                std::string row_content = trimmed.substr(row_start + 1, row_end - row_start - 1);

                // Parse values in row
                std::vector<double> row;
                std::istringstream iss(row_content);
                std::string token;

                while (std::getline(iss, token, ',')) {
                    token = trim(token);
                    if (token.empty()) {
                        continue;
                    }

                    try {
                        double value = std::stod(token);
                        row.push_back(value);
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Invalid numeric value in JSON: " + token);
                    }
                }

                if (!row.empty()) {
                    rows.push_back(row);
                }

                pos = row_end + 1;

                // Skip comma if present
                while (pos < outer_end && (std::isspace(trimmed[pos]) || trimmed[pos] == ',')) {
                    ++pos;
                }
            } else if (trimmed[pos] == ',') {
                ++pos;
            } else if (!std::isspace(trimmed[pos])) {
                // Unexpected non-whitespace character
                throw std::runtime_error(std::string("Invalid JSON: unexpected character '") +
                                         trimmed[pos] + "' at position " + std::to_string(pos));
            } else {
                ++pos;
            }
        }

        if (rows.empty()) {
            throw std::runtime_error("No valid data found in JSON");
        }

        // Convert to Matrix
        size_t n = rows.size();
        size_t m = rows[0].size();

        if (n != m) {
            throw std::invalid_argument("Covariance matrix must be square (got " +
                                        std::to_string(n) + "x" + std::to_string(m) + ")");
        }

        core::Matrix matrix(n, m);
        for (size_t i = 0; i < n; ++i) {
            if (rows[i].size() != m) {
                throw std::invalid_argument("All rows must have the same length");
            }
            for (size_t j = 0; j < m; ++j) {
                matrix(i, j) = rows[i][j];
            }
        }

        return CovarianceMatrix(std::move(matrix));
    }

    /**
     * @brief Validate the covariance matrix.
     *
     * Checks:
     * - Matrix is not empty
     * - Matrix is square
     * - Matrix is symmetric (within tolerance)
     * - All diagonal elements are positive (variance must be positive)
     * - All values are finite (no NaN or infinity)
     *
     * @throws std::invalid_argument if validation fails
     */
    void validate() const {
        if (matrix_.empty()) {
            throw std::invalid_argument("Covariance matrix cannot be empty");
        }

        if (!matrix_.isSquare()) {
            throw std::invalid_argument("Covariance matrix must be square");
        }

        const size_t n = matrix_.rows();

        // Check all values are finite
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                if (!std::isfinite(matrix_(i, j))) {
                    throw std::invalid_argument(
                        "Covariance matrix must have finite values (no NaN or infinity)");
                }
            }
        }

        // Check diagonal elements are positive
        for (size_t i = 0; i < n; ++i) {
            if (matrix_(i, i) <= 0.0) {
                throw std::invalid_argument(
                    "Covariance matrix diagonal elements (variances) must be positive");
            }
        }

        // Check symmetry
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = i + 1; j < n; ++j) {
                double diff = std::abs(matrix_(i, j) - matrix_(j, i));
                double scale = std::max(std::abs(matrix_(i, j)), std::abs(matrix_(j, i)));
                double tolerance = core::EPSILON * std::max(1.0, scale);

                if (diff > tolerance) {
                    throw std::invalid_argument("Covariance matrix must be symmetric");
                }
            }
        }
    }

    /**
     * @brief Check if dimensions match with another object.
     * @param n Expected dimension
     * @return true if dimensions match, false otherwise
     */
    bool dimensionsMatch(size_t n) const { return size() == n; }

private:
    core::Matrix matrix_;

    /**
     * @brief Trim whitespace from string.
     * @param str String to trim
     * @return Trimmed string
     */
    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) {
            return "";
        }
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, last - first + 1);
    }
};

}  // namespace optimizer
}  // namespace orbat
