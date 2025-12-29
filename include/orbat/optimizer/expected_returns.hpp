#pragma once

#include "orbat/core/vector.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace orbat {
namespace optimizer {

/**
 * @brief Container for expected returns of assets in a portfolio.
 *
 * This class provides a standardized interface for managing expected returns
 * with support for CSV and JSON import/export and validation.
 *
 * Expected returns represent the anticipated mean return for each asset
 * in a portfolio, typically expressed as decimal returns (e.g., 0.08 for 8%).
 *
 * Example:
 *   ExpectedReturns returns = ExpectedReturns::fromCSV("returns.csv");
 *   size_t n = returns.size();
 *   const Vector& data = returns.data();
 */
class ExpectedReturns {
public:
    /**
     * @brief Construct empty expected returns.
     */
    ExpectedReturns() = default;

    /**
     * @brief Construct from a vector of returns.
     * @param returns Vector of expected returns
     * @throws std::invalid_argument if returns vector is empty
     */
    explicit ExpectedReturns(const core::Vector& returns) : returns_(returns) { validate(); }

    /**
     * @brief Construct from a vector of returns (move).
     * @param returns Vector of expected returns
     * @throws std::invalid_argument if returns vector is empty
     */
    explicit ExpectedReturns(core::Vector&& returns) : returns_(std::move(returns)) { validate(); }

    /**
     * @brief Construct from a vector of returns with labels.
     * @param returns Vector of expected returns
     * @param labels Vector of asset labels
     * @throws std::invalid_argument if returns vector is empty or sizes don't match
     */
    ExpectedReturns(const core::Vector& returns, const std::vector<std::string>& labels)
        : returns_(returns), labels_(labels) {
        if (!labels.empty() && labels.size() != returns.size()) {
            throw std::invalid_argument("Labels size must match returns size or be empty");
        }
        validate();
    }

    /**
     * @brief Construct from initializer list.
     * @param returns Initializer list of returns
     * @throws std::invalid_argument if returns list is empty
     */
    ExpectedReturns(std::initializer_list<double> returns) : returns_(returns) { validate(); }

    /**
     * @brief Get the number of assets.
     * @return Number of assets
     */
    size_t size() const { return returns_.size(); }

    /**
     * @brief Check if empty.
     * @return true if empty, false otherwise
     */
    bool empty() const { return returns_.empty(); }

    /**
     * @brief Get the underlying data vector (const).
     * @return Const reference to returns vector
     */
    const core::Vector& data() const { return returns_; }

    /**
     * @brief Get the underlying data vector.
     * @return Reference to returns vector
     */
    core::Vector& data() { return returns_; }

    /**
     * @brief Access element (const).
     * @param index Asset index
     * @return Expected return value
     */
    double operator[](size_t index) const { return returns_[index]; }

    /**
     * @brief Access element (non-const).
     * @param index Asset index
     * @return Reference to expected return value
     */
    double& operator[](size_t index) { return returns_[index]; }

    /**
     * @brief Get asset labels.
     * @return Const reference to labels vector (empty if no labels)
     */
    const std::vector<std::string>& labels() const { return labels_; }

    /**
     * @brief Set asset labels.
     * @param labels Vector of asset labels
     * @throws std::invalid_argument if labels size doesn't match returns size
     */
    void setLabels(const std::vector<std::string>& labels) {
        if (!labels.empty() && labels.size() != returns_.size()) {
            throw std::invalid_argument("Labels size must match returns size or be empty");
        }
        labels_ = labels;
    }

    /**
     * @brief Check if asset has a label.
     * @param index Asset index
     * @return true if asset has a label, false otherwise
     */
    bool hasLabel(size_t index) const {
        return !labels_.empty() && index < labels_.size() && !labels_[index].empty();
    }

    /**
     * @brief Get asset label.
     * @param index Asset index
     * @return Asset label or default label if not set
     */
    std::string getLabel(size_t index) const {
        if (hasLabel(index)) {
            return labels_[index];
        }
        return "Asset " + std::to_string(index);
    }

    /**
     * @brief Load expected returns from CSV file.
     *
     * CSV format should be either:
     * - Single column of returns (one per line)
     * - Multiple columns where the first column contains returns
     * - Optional header row (detected automatically if non-numeric)
     *
     * Example CSV:
     *   0.08
     *   0.12
     *   0.10
     *
     * Or with header:
     *   expected_return
     *   0.08
     *   0.12
     *   0.10
     *
     * @param filename Path to CSV file
     * @return ExpectedReturns object
     * @throws std::runtime_error if file cannot be opened or parsed
     * @throws std::invalid_argument if data is invalid
     */
    static ExpectedReturns fromCSV(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }

        std::vector<double> returns;
        std::string line;
        bool first_line = true;

        while (std::getline(file, line)) {
            // Skip empty lines
            if (line.empty()) {
                continue;
            }

            // Try to parse as number
            std::istringstream iss(line);
            double value;

            // Handle potential commas in CSV
            std::string first_field;
            if (line.find(',') != std::string::npos) {
                std::getline(iss, first_field, ',');
            } else {
                first_field = line;
            }

            // Skip header row if it exists (first non-numeric line)
            if (first_line) {
                first_line = false;
                std::istringstream test(first_field);
                if (!(test >> value)) {
                    // Non-numeric first line, treat as header
                    continue;
                }
                returns.push_back(value);
            } else {
                std::istringstream value_stream(first_field);
                if (!(value_stream >> value)) {
                    throw std::runtime_error("Invalid numeric value in CSV: " + first_field);
                }
                returns.push_back(value);
            }
        }

        if (returns.empty()) {
            throw std::runtime_error("No valid data found in CSV file: " + filename);
        }

        return ExpectedReturns(core::Vector(returns));
    }

    /**
     * @brief Load expected returns from JSON file.
     *
     * JSON format can be:
     * 1. Simple array: [0.08, 0.12, 0.10]
     * 2. Object with returns: {"returns": [0.08, 0.12, 0.10]}
     * 3. Object with returns and labels: {"returns": [0.08, 0.12], "labels": ["Stock A", "Stock
     * B"]}
     *
     * @param filename Path to JSON file
     * @return ExpectedReturns object
     * @throws std::runtime_error if file cannot be opened or parsed
     * @throws std::invalid_argument if data is invalid
     */
    static ExpectedReturns fromJSON(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }

        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());

        return fromJSONString(content);
    }

    /**
     * @brief Parse expected returns from JSON string.
     *
     * Supports multiple formats:
     * 1. Simple array: [0.08, 0.12, 0.10]
     * 2. Object with returns: {"returns": [0.08, 0.12, 0.10]}
     * 3. Object with returns and labels: {"returns": [0.08, 0.12], "labels": ["Stock A", "Stock
     * B"]}
     *
     * @param json_str JSON string
     * @return ExpectedReturns object
     * @throws std::runtime_error if JSON cannot be parsed
     * @throws std::invalid_argument if data is invalid
     */
    static ExpectedReturns fromJSONString(const std::string& json_str) {
        std::vector<double> returns;
        std::vector<std::string> labels;

        std::string trimmed = trim(json_str);

        // Check if it's an object or array
        if (trimmed[0] == '{') {
            // Object format: {"returns": [...], "labels": [...]}
            parseJSONObject(trimmed, returns, labels);
        } else if (trimmed[0] == '[') {
            // Simple array format: [0.08, 0.12, 0.10]
            parseJSONArray(trimmed, returns);
        } else {
            throw std::runtime_error("Invalid JSON: expected object or array");
        }

        if (returns.empty()) {
            throw std::runtime_error("No valid data found in JSON");
        }

        if (labels.empty()) {
            return ExpectedReturns(core::Vector(returns));
        } else {
            return ExpectedReturns(core::Vector(returns), labels);
        }
    }

    /**
     * @brief Validate the expected returns data.
     *
     * Checks:
     * - Data is not empty
     * - All values are finite (no NaN or infinity)
     *
     * @throws std::invalid_argument if validation fails
     */
    void validate() const {
        if (returns_.empty()) {
            throw std::invalid_argument("Expected returns cannot be empty");
        }

        for (size_t i = 0; i < returns_.size(); ++i) {
            if (!std::isfinite(returns_[i])) {
                throw std::invalid_argument("Expected returns must be finite (no NaN or infinity)");
            }
        }
    }

private:
    core::Vector returns_;
    std::vector<std::string> labels_;

    /**
     * @brief Parse JSON array format: [0.08, 0.12, 0.10]
     */
    static void parseJSONArray(const std::string& json_str, std::vector<double>& returns) {
        std::string trimmed = trim(json_str);

        // Look for array start
        size_t array_start = trimmed.find('[');
        if (array_start == std::string::npos) {
            throw std::runtime_error("Invalid JSON: expected array");
        }

        size_t array_end = trimmed.find(']', array_start);
        if (array_end == std::string::npos) {
            throw std::runtime_error("Invalid JSON: unclosed array");
        }

        // Extract array content
        std::string array_content = trimmed.substr(array_start + 1, array_end - array_start - 1);

        // Parse comma-separated values
        std::istringstream iss(array_content);
        std::string token;

        while (std::getline(iss, token, ',')) {
            token = trim(token);
            if (token.empty()) {
                continue;
            }

            try {
                double value = std::stod(token);
                returns.push_back(value);
            } catch (const std::exception&) {
                throw std::runtime_error("Invalid numeric value in JSON: " + token);
            }
        }
    }

    /**
     * @brief Parse JSON string array: ["Stock A", "Stock B"]
     */
    static void parseJSONStringArray(const std::string& json_str,
                                     std::vector<std::string>& labels) {
        std::string trimmed = trim(json_str);

        size_t array_start = trimmed.find('[');
        if (array_start == std::string::npos) {
            throw std::runtime_error("Invalid JSON: expected array");
        }

        size_t array_end = trimmed.find(']', array_start);
        if (array_end == std::string::npos) {
            throw std::runtime_error("Invalid JSON: unclosed array");
        }

        std::string array_content = trimmed.substr(array_start + 1, array_end - array_start - 1);

        // Parse string values
        size_t pos = 0;
        while (pos < array_content.length()) {
            // Skip whitespace
            while (pos < array_content.length() && std::isspace(array_content[pos])) {
                ++pos;
            }

            if (pos >= array_content.length()) {
                break;
            }

            // Look for string start
            if (array_content[pos] == '"') {
                ++pos;
                size_t string_end = array_content.find('"', pos);
                if (string_end == std::string::npos) {
                    throw std::runtime_error("Invalid JSON: unclosed string");
                }
                labels.push_back(array_content.substr(pos, string_end - pos));
                pos = string_end + 1;

                // Skip to next comma or end
                while (pos < array_content.length() &&
                       (std::isspace(array_content[pos]) || array_content[pos] == ',')) {
                    ++pos;
                }
            } else {
                ++pos;
            }
        }
    }

    /**
     * @brief Parse JSON object format: {"returns": [...], "labels": [...]}
     */
    static void parseJSONObject(const std::string& json_str, std::vector<double>& returns,
                                std::vector<std::string>& labels) {
        std::string trimmed = trim(json_str);

        // Find "returns" field
        size_t returns_pos = trimmed.find("\"returns\"");
        if (returns_pos == std::string::npos) {
            returns_pos = trimmed.find("'returns'");
        }
        if (returns_pos == std::string::npos) {
            throw std::runtime_error("Invalid JSON: 'returns' field not found");
        }

        // Find the array after "returns"
        size_t colon_pos = trimmed.find(':', returns_pos);
        if (colon_pos == std::string::npos) {
            throw std::runtime_error("Invalid JSON: expected ':' after 'returns'");
        }

        size_t array_start = trimmed.find('[', colon_pos);
        if (array_start == std::string::npos) {
            throw std::runtime_error("Invalid JSON: expected array for 'returns'");
        }

        size_t array_end = trimmed.find(']', array_start);
        if (array_end == std::string::npos) {
            throw std::runtime_error("Invalid JSON: unclosed array for 'returns'");
        }

        std::string returns_array = trimmed.substr(array_start, array_end - array_start + 1);
        parseJSONArray(returns_array, returns);

        // Try to find "labels" field (optional)
        size_t labels_pos = trimmed.find("\"labels\"");
        if (labels_pos == std::string::npos) {
            labels_pos = trimmed.find("'labels'");
        }

        if (labels_pos != std::string::npos) {
            size_t labels_colon = trimmed.find(':', labels_pos);
            if (labels_colon != std::string::npos) {
                size_t labels_array_start = trimmed.find('[', labels_colon);
                if (labels_array_start != std::string::npos) {
                    size_t labels_array_end = trimmed.find(']', labels_array_start);
                    if (labels_array_end != std::string::npos) {
                        std::string labels_array = trimmed.substr(
                            labels_array_start, labels_array_end - labels_array_start + 1);
                        parseJSONStringArray(labels_array, labels);
                    }
                }
            }
        }
    }

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
