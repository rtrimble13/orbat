#pragma once

#include "orbat/optimizer/markowitz.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace orbat {
namespace optimizer {

/**
 * @brief Export efficient frontier portfolios to CSV format.
 *
 * Exports portfolio data in a format suitable for visualization and analysis.
 * The CSV includes return, volatility (risk), and weights for each portfolio.
 *
 * CSV format:
 *   return,volatility,weight_0,weight_1,...,weight_n
 *   0.08,0.10,0.5,0.3,0.2
 *   0.09,0.11,0.4,0.35,0.25
 *   ...
 *
 * @param frontier Vector of MarkowitzResult representing efficient portfolios
 * @param filename Output CSV file path
 * @param assetLabels Optional vector of asset labels for column headers
 * @throws std::runtime_error if file cannot be opened
 * @throws std::invalid_argument if frontier is empty
 */
inline void exportFrontierToCSV(const std::vector<MarkowitzResult>& frontier,
                                const std::string& filename,
                                const std::vector<std::string>& assetLabels = {}) {
    if (frontier.empty()) {
        throw std::invalid_argument("Cannot export empty frontier");
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }

    // Write header
    file << "return,volatility";
    
    // Find first successful portfolio to get number of assets
    size_t numAssets = 0;
    for (const auto& result : frontier) {
        if (result.success()) {
            numAssets = result.weights.size();
            break;
        }
    }
    
    if (numAssets == 0) {
        throw std::invalid_argument("No successful portfolios in frontier");
    }

    // Add weight column headers
    for (size_t i = 0; i < numAssets; ++i) {
        file << ",";
        if (i < assetLabels.size() && !assetLabels[i].empty()) {
            file << assetLabels[i];
        } else {
            file << "weight_" << i;
        }
    }
    file << "\n";

    // Write data rows with high precision
    file << std::fixed << std::setprecision(8);
    for (const auto& result : frontier) {
        if (!result.success()) {
            continue;  // Skip failed optimizations
        }

        file << result.expectedReturn << "," << result.risk;

        for (size_t i = 0; i < result.weights.size(); ++i) {
            file << "," << result.weights[i];
        }
        file << "\n";
    }

    file.close();
}

/**
 * @brief Export efficient frontier portfolios to JSON format.
 *
 * Exports portfolio data in JSON format suitable for web visualization tools
 * and JavaScript-based plotting libraries.
 *
 * JSON format:
 * {
 *   "frontier": [
 *     {
 *       "return": 0.08,
 *       "volatility": 0.10,
 *       "weights": [0.5, 0.3, 0.2]
 *     },
 *     ...
 *   ]
 * }
 *
 * Or with labels:
 * {
 *   "assets": ["Stock A", "Stock B", "Stock C"],
 *   "frontier": [
 *     {
 *       "return": 0.08,
 *       "volatility": 0.10,
 *       "weights": [0.5, 0.3, 0.2]
 *     },
 *     ...
 *   ]
 * }
 *
 * @param frontier Vector of MarkowitzResult representing efficient portfolios
 * @param filename Output JSON file path
 * @param assetLabels Optional vector of asset labels
 * @throws std::runtime_error if file cannot be opened
 * @throws std::invalid_argument if frontier is empty
 */
inline void exportFrontierToJSON(const std::vector<MarkowitzResult>& frontier,
                                 const std::string& filename,
                                 const std::vector<std::string>& assetLabels = {}) {
    if (frontier.empty()) {
        throw std::invalid_argument("Cannot export empty frontier");
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }

    file << std::fixed << std::setprecision(8);
    file << "{\n";

    // Add asset labels if provided
    if (!assetLabels.empty()) {
        file << "  \"assets\": [";
        for (size_t i = 0; i < assetLabels.size(); ++i) {
            if (i > 0) {
                file << ", ";
            }
            file << "\"" << assetLabels[i] << "\"";
        }
        file << "],\n";
    }

    // Write frontier data
    file << "  \"frontier\": [\n";

    bool first = true;
    for (const auto& result : frontier) {
        if (!result.success()) {
            continue;  // Skip failed optimizations
        }

        if (!first) {
            file << ",\n";
        }
        first = false;

        file << "    {\n";
        file << "      \"return\": " << result.expectedReturn << ",\n";
        file << "      \"volatility\": " << result.risk << ",\n";
        file << "      \"weights\": [";

        for (size_t i = 0; i < result.weights.size(); ++i) {
            if (i > 0) {
                file << ", ";
            }
            file << result.weights[i];
        }

        file << "]\n";
        file << "    }";
    }

    file << "\n  ]\n";
    file << "}\n";

    file.close();
}

/**
 * @brief Convert efficient frontier to JSON string.
 *
 * Similar to exportFrontierToJSON but returns a string instead of writing to file.
 *
 * @param frontier Vector of MarkowitzResult representing efficient portfolios
 * @param assetLabels Optional vector of asset labels
 * @return JSON string representation
 * @throws std::invalid_argument if frontier is empty
 */
inline std::string frontierToJSONString(const std::vector<MarkowitzResult>& frontier,
                                        const std::vector<std::string>& assetLabels = {}) {
    if (frontier.empty()) {
        throw std::invalid_argument("Cannot export empty frontier");
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(8);
    oss << "{\n";

    // Add asset labels if provided
    if (!assetLabels.empty()) {
        oss << "  \"assets\": [";
        for (size_t i = 0; i < assetLabels.size(); ++i) {
            if (i > 0) {
                oss << ", ";
            }
            oss << "\"" << assetLabels[i] << "\"";
        }
        oss << "],\n";
    }

    // Write frontier data
    oss << "  \"frontier\": [\n";

    bool first = true;
    for (const auto& result : frontier) {
        if (!result.success()) {
            continue;  // Skip failed optimizations
        }

        if (!first) {
            oss << ",\n";
        }
        first = false;

        oss << "    {\n";
        oss << "      \"return\": " << result.expectedReturn << ",\n";
        oss << "      \"volatility\": " << result.risk << ",\n";
        oss << "      \"weights\": [";

        for (size_t i = 0; i < result.weights.size(); ++i) {
            if (i > 0) {
                oss << ", ";
            }
            oss << result.weights[i];
        }

        oss << "]\n";
        oss << "    }";
    }

    oss << "\n  ]\n";
    oss << "}\n";

    return oss.str();
}

}  // namespace optimizer
}  // namespace orbat
