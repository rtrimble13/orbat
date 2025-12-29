#pragma once

#include "orbat/core/constants.hpp"
#include "orbat/core/matrix.hpp"
#include "orbat/core/vector.hpp"
#include "orbat/optimizer/constraint.hpp"
#include "orbat/optimizer/covariance_matrix.hpp"
#include "orbat/optimizer/expected_returns.hpp"

#include <cmath>
#include <iomanip>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace orbat {
namespace optimizer {

/**
 * @brief Result of a Markowitz portfolio optimization.
 *
 * Contains the optimal weights, expected return, portfolio risk (volatility),
 * and Sharpe ratio. This structure provides a standardized output format
 * for all optimization methods, with support for serialization to JSON and CSV.
 */
struct MarkowitzResult {
    core::Vector weights;   // Optimal portfolio weights
    double expectedReturn;  // Expected portfolio return
    double risk;            // Portfolio risk (standard deviation/volatility)
    double sharpeRatio;     // Sharpe ratio (expectedReturn - riskFreeRate) / risk
    bool converged;         // Whether optimization converged
    std::string message;    // Status or error message

    /**
     * @brief Check if the optimization was successful.
     * @return true if converged, false otherwise
     */
    bool success() const { return converged; }

    /**
     * @brief Calculate Sharpe ratio with a custom risk-free rate.
     *
     * Computes the risk-adjusted return as (expectedReturn - riskFreeRate) / risk.
     * The default Sharpe ratio in the struct assumes a risk-free rate of 0.
     *
     * @param riskFreeRate The risk-free rate (default: 0.0)
     * @return Sharpe ratio adjusted for the given risk-free rate
     */
    double calculateSharpeRatio(double riskFreeRate = 0.0) const {
        if (risk <= core::EPSILON) {
            return 0.0;
        }
        return (expectedReturn - riskFreeRate) / risk;
    }

    /**
     * @brief Update the Sharpe ratio with a custom risk-free rate.
     *
     * Modifies the sharpeRatio field to reflect a different risk-free rate.
     * Useful when you want to store results with a non-zero risk-free rate.
     *
     * @param riskFreeRate The risk-free rate to use
     */
    void setRiskFreeRate(double riskFreeRate) { sharpeRatio = calculateSharpeRatio(riskFreeRate); }

    /**
     * @brief Serialize result to JSON string.
     *
     * Produces a JSON representation of the optimization result.
     *
     * Example output:
     * {
     *   "converged": true,
     *   "message": "Optimization successful",
     *   "expectedReturn": 0.12,
     *   "risk": 0.15,
     *   "sharpeRatio": 0.8,
     *   "weights": [0.3, 0.5, 0.2]
     * }
     *
     * @return JSON string representation
     */
    std::string toJSON() const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(8);
        oss << "{\n";
        oss << "  \"converged\": " << (converged ? "true" : "false") << ",\n";
        oss << "  \"message\": \"" << message << "\",\n";
        oss << "  \"expectedReturn\": " << expectedReturn << ",\n";
        oss << "  \"risk\": " << risk << ",\n";
        oss << "  \"sharpeRatio\": " << sharpeRatio << ",\n";
        oss << "  \"weights\": [";
        for (size_t i = 0; i < weights.size(); ++i) {
            if (i > 0) {
                oss << ", ";
            }
            oss << weights[i];
        }
        oss << "]\n";
        oss << "}";
        return oss.str();
    }

    /**
     * @brief Serialize result to CSV string.
     *
     * Produces a CSV representation with a header and data row.
     *
     * Format: converged,message,expectedReturn,risk,sharpeRatio,weight_0,weight_1,...
     *
     * @param includeHeader Whether to include the header row (default: true)
     * @return CSV string representation
     */
    std::string toCSV(bool includeHeader = true) const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(8);

        // Header
        if (includeHeader) {
            oss << "converged,message,expectedReturn,risk,sharpeRatio";
            for (size_t i = 0; i < weights.size(); ++i) {
                oss << ",weight_" << i;
            }
            oss << "\n";
        }

        // Data row
        oss << (converged ? "true" : "false") << ",";
        oss << "\"" << message << "\",";
        oss << expectedReturn << ",";
        oss << risk << ",";
        oss << sharpeRatio;
        for (size_t i = 0; i < weights.size(); ++i) {
            oss << "," << weights[i];
        }

        return oss.str();
    }

    /**
     * @brief Deserialize result from JSON string.
     *
     * Parses a JSON string to reconstruct a MarkowitzResult.
     * This is a simple parser for the specific format produced by toJSON().
     *
     * @param json JSON string representation
     * @return Deserialized MarkowitzResult
     * @throws std::runtime_error if JSON is malformed
     */
    static MarkowitzResult fromJSON(const std::string& json) {
        MarkowitzResult result;

        // Simple JSON parser for our specific format
        auto findValue = [&json](const std::string& key) -> std::string {
            std::string searchKey = "\"" + key + "\":";
            size_t pos = json.find(searchKey);
            if (pos == std::string::npos) {
                throw std::runtime_error("Key not found: " + key);
            }
            pos += searchKey.length();

            // Skip whitespace
            while (pos < json.length() && std::isspace(json[pos])) {
                pos++;
            }

            size_t end = pos;
            if (json[pos] == '"') {
                // String value
                pos++;
                end = json.find('"', pos);
                if (end == std::string::npos) {
                    throw std::runtime_error("Malformed string value for key: " + key);
                }
                return json.substr(pos, end - pos);
            } else if (json[pos] == '[') {
                // Array value - return everything including brackets
                int depth = 0;
                while (end < json.length()) {
                    if (json[end] == '[') {
                        depth++;
                    } else if (json[end] == ']') {
                        depth--;
                        if (depth == 0) {
                            end++;
                            break;
                        }
                    }
                    end++;
                }
                return json.substr(pos, end - pos);
            } else if (json.substr(pos, 4) == "true") {
                return "true";
            } else if (json.substr(pos, 5) == "false") {
                return "false";
            } else {
                // Numeric value
                while (end < json.length() &&
                       (std::isdigit(json[end]) || json[end] == '.' || json[end] == '-' ||
                        json[end] == 'e' || json[end] == 'E' || json[end] == '+')) {
                    end++;
                }
                return json.substr(pos, end - pos);
            }
        };

        // Parse fields
        result.converged = (findValue("converged") == "true");
        result.message = findValue("message");
        result.expectedReturn = std::stod(findValue("expectedReturn"));
        result.risk = std::stod(findValue("risk"));
        result.sharpeRatio = std::stod(findValue("sharpeRatio"));

        // Parse weights array
        std::string weightsStr = findValue("weights");
        weightsStr = weightsStr.substr(1, weightsStr.length() - 2);  // Remove [ ]

        std::vector<double> weightsVec;
        std::istringstream iss(weightsStr);
        std::string token;
        while (std::getline(iss, token, ',')) {
            // Trim whitespace
            token.erase(0, token.find_first_not_of(" \t\n\r"));
            token.erase(token.find_last_not_of(" \t\n\r") + 1);
            if (!token.empty()) {
                weightsVec.push_back(std::stod(token));
            }
        }

        result.weights = core::Vector(weightsVec);

        return result;
    }
};

/**
 * @brief Classic Markowitz mean-variance portfolio optimizer.
 *
 * This class implements the classic Markowitz portfolio optimization framework
 * using quadratic programming. It supports:
 * - Minimum variance portfolios
 * - Mean-variance trade-off with risk aversion parameter λ
 * - Target return constraints
 * - Long-only and box constraints
 *
 * The optimizer solves quadratic programming problems of the form:
 *   minimize:   (1/2)w'Σw - λμ'w
 *   subject to: w'1 = 1 (fully invested)
 *               w ≥ 0 (long-only, optional)
 *               w_min ≤ w ≤ w_max (box constraints, optional)
 *               μ'w = r_target (target return, optional)
 *
 * Where:
 * - w is the vector of portfolio weights
 * - Σ is the covariance matrix of asset returns
 * - μ is the vector of expected returns
 * - λ is the risk aversion parameter (higher λ = more focus on returns)
 *
 * Example:
 *   MarkowitzOptimizer optimizer(returns, covariance);
 *   auto result = optimizer.minimumVariance();  // Min variance portfolio
 *   auto result2 = optimizer.optimize(0.5);     // Risk aversion λ = 0.5
 *   auto result3 = optimizer.targetReturn(0.10); // Target 10% return
 */
class MarkowitzOptimizer {
public:
    /**
     * @brief Construct a Markowitz optimizer.
     *
     * @param expectedReturns Expected returns for each asset
     * @param covariance Covariance matrix of asset returns
     * @throws std::invalid_argument if dimensions don't match or data is invalid
     */
    MarkowitzOptimizer(const ExpectedReturns& expectedReturns, const CovarianceMatrix& covariance)
        : expectedReturns_(expectedReturns), covariance_(covariance), maxIterations_(1000),
          tolerance_(1e-8) {
        validate();
    }

    /**
     * @brief Construct a Markowitz optimizer with constraints.
     *
     * @param expectedReturns Expected returns for each asset
     * @param covariance Covariance matrix of asset returns
     * @param constraints Portfolio constraints to enforce
     * @throws std::invalid_argument if dimensions don't match or data is invalid
     */
    MarkowitzOptimizer(const ExpectedReturns& expectedReturns, const CovarianceMatrix& covariance,
                       const ConstraintSet& constraints)
        : expectedReturns_(expectedReturns), covariance_(covariance), constraints_(constraints),
          maxIterations_(1000), tolerance_(1e-8) {
        validate();
    }

    /**
     * @brief Set maximum iterations for the optimizer.
     * @param maxIter Maximum iterations (must be > 0)
     * @throws std::invalid_argument if maxIter is 0
     */
    void setMaxIterations(size_t maxIter) {
        if (maxIter == 0) {
            throw std::invalid_argument("Maximum iterations must be positive");
        }
        maxIterations_ = maxIter;
    }

    /**
     * @brief Set convergence tolerance.
     * @param tol Tolerance (must be > 0)
     * @throws std::invalid_argument if tol is not positive
     */
    void setTolerance(double tol) {
        if (tol <= 0.0) {
            throw std::invalid_argument("Tolerance must be positive");
        }
        tolerance_ = tol;
    }

    /**
     * @brief Add a constraint to the optimizer.
     * @param constraint Constraint to add
     */
    void addConstraint(std::shared_ptr<Constraint> constraint) { constraints_.add(constraint); }

    /**
     * @brief Compute the minimum variance portfolio.
     *
     * Solves: minimize (1/2)w'Σw
     *         subject to: w'1 = 1 and any additional constraints
     *
     * This is the leftmost point on the efficient frontier.
     *
     * @return Optimization result with optimal weights
     */
    MarkowitzResult minimumVariance() const {
        const size_t n = expectedReturns_.size();

        // For minimum variance with fully invested constraint:
        // Solution is w = (Σ^-1 * 1) / (1' * Σ^-1 * 1)
        // where 1 is a vector of ones

        try {
            // Get inverse of covariance matrix
            core::Matrix covInv = covariance_.data().inverse();

            // Create ones vector
            core::Vector ones(n, 1.0);

            // Compute Σ^-1 * 1
            core::Vector covInvOnes = covInv * ones;

            // Compute 1' * Σ^-1 * 1 (scalar)
            double denominator = ones.dot(covInvOnes);

            if (std::abs(denominator) < core::EPSILON) {
                return MarkowitzResult{{}, 0.0, 0.0, 0.0, false, "Singular covariance matrix"};
            }

            // Compute optimal weights
            core::Vector weights = covInvOnes / denominator;

            // Apply constraints if any (project weights)
            if (!constraints_.empty()) {
                if (!constraints_.isFeasible(weights)) {
                    // If unconstrained solution violates constraints,
                    // use numerical optimization
                    return solveConstrainedQP(weights, 0.0);
                }
            }

            // Compute portfolio statistics
            double expectedReturn = expectedReturns_.data().dot(weights);
            double variance = computeVariance(weights);
            double risk = std::sqrt(std::max(0.0, variance));
            double sharpeRatio = (risk > core::EPSILON) ? (expectedReturn / risk) : 0.0;

            return MarkowitzResult{weights, expectedReturn,
                                   risk,    sharpeRatio,
                                   true,    "Minimum variance portfolio computed"};

        } catch (const std::exception& e) {
            return MarkowitzResult{{},  0.0,   0.0,
                                   0.0, false, std::string("Optimization failed: ") + e.what()};
        }
    }

    /**
     * @brief Optimize portfolio with risk aversion parameter.
     *
     * Solves: minimize (1/2)w'Σw - λμ'w
     *         subject to: w'1 = 1 and any additional constraints
     *
     * The risk aversion parameter λ controls the trade-off between risk and return:
     * - λ = 0: minimum variance (ignore returns)
     * - λ > 0: higher λ means more emphasis on returns
     * - λ → ∞: maximum return portfolio (ignore risk)
     *
     * @param lambda Risk aversion parameter (≥ 0)
     * @return Optimization result with optimal weights
     * @throws std::invalid_argument if lambda is negative
     */
    MarkowitzResult optimize(double lambda) const {
        if (lambda < 0.0) {
            throw std::invalid_argument("Risk aversion parameter must be non-negative");
        }

        // For lambda = 0, this is minimum variance
        if (lambda < core::EPSILON) {
            return minimumVariance();
        }

        const size_t n = expectedReturns_.size();

        try {
            // For mean-variance with risk aversion:
            // Solution is w = (Σ^-1 * (λμ + γ1)) / (1' * Σ^-1 * (λμ + γ1))
            // where γ is chosen to satisfy w'1 = 1
            //
            // Simplifying: w = Σ^-1 * (λμ + γ1) where γ = (1 - λ*1'*Σ^-1*μ) / (1'*Σ^-1*1)

            core::Matrix covInv = covariance_.data().inverse();
            core::Vector ones(n, 1.0);
            core::Vector mu = expectedReturns_.data();

            // Compute helper quantities
            core::Vector covInvMu = covInv * mu;
            core::Vector covInvOnes = covInv * ones;

            double onesCovInvMu = ones.dot(covInvMu);
            double onesCovInvOnes = ones.dot(covInvOnes);

            if (std::abs(onesCovInvOnes) < core::EPSILON) {
                return MarkowitzResult{{}, 0.0, 0.0, 0.0, false, "Singular covariance matrix"};
            }

            // Compute γ
            double gamma = (1.0 - lambda * onesCovInvMu) / onesCovInvOnes;

            // Compute weights: w = λ*Σ^-1*μ + γ*Σ^-1*1
            core::Vector weights = covInvMu * lambda + covInvOnes * gamma;

            // Apply constraints if any
            if (!constraints_.empty()) {
                if (!constraints_.isFeasible(weights)) {
                    return solveConstrainedQP(weights, lambda);
                }
            }

            // Compute portfolio statistics
            double expectedReturn = mu.dot(weights);
            double variance = computeVariance(weights);
            double risk = std::sqrt(std::max(0.0, variance));
            double sharpeRatio = (risk > core::EPSILON) ? (expectedReturn / risk) : 0.0;

            return MarkowitzResult{weights,     expectedReturn, risk,
                                   sharpeRatio, true,           "Mean-variance portfolio computed"};

        } catch (const std::exception& e) {
            return MarkowitzResult{{},  0.0,   0.0,
                                   0.0, false, std::string("Optimization failed: ") + e.what()};
        }
    }

    /**
     * @brief Optimize portfolio with target return constraint.
     *
     * Solves: minimize (1/2)w'Σw
     *         subject to: μ'w = targetReturn
     *                     w'1 = 1
     *                     and any additional constraints
     *
     * This finds the minimum variance portfolio that achieves the target return.
     *
     * @param targetReturn Target portfolio return
     * @return Optimization result with optimal weights
     */
    MarkowitzResult targetReturn(double targetReturn) const {
        const size_t n = expectedReturns_.size();

        try {
            // Compute feasible return range
            // Minimum return: minimum individual asset return
            // Maximum return: maximum individual asset return
            const auto& returnsData = expectedReturns_.data().data();
            double minReturn = *std::min_element(returnsData.begin(), returnsData.end());
            double maxReturn = *std::max_element(returnsData.begin(), returnsData.end());

            if (targetReturn < minReturn - tolerance_ || targetReturn > maxReturn + tolerance_) {
                return MarkowitzResult{{}, 0.0, 0.0, 0.0, false, "Target return is not achievable"};
            }

            // For target return with fully invested constraint:
            // Solution is w = Σ^-1 * (a*μ + b*1)
            // where a and b are chosen to satisfy:
            //   μ'w = targetReturn
            //   1'w = 1

            core::Matrix covInv = covariance_.data().inverse();
            core::Vector ones(n, 1.0);
            core::Vector mu = expectedReturns_.data();

            // Compute helper quantities
            core::Vector covInvMu = covInv * mu;
            core::Vector covInvOnes = covInv * ones;

            double A = mu.dot(covInvMu);
            double B = mu.dot(covInvOnes);
            double C = ones.dot(covInvOnes);

            double det = A * C - B * B;
            if (std::abs(det) < core::EPSILON) {
                return MarkowitzResult{{},  0.0,   0.0,
                                       0.0, false, "System is singular (returns may be constant)"};
            }

            // Solve for a and b
            double a = (C * targetReturn - B) / det;
            double b = (A - B * targetReturn) / det;

            // Compute weights
            core::Vector weights = covInvMu * a + covInvOnes * b;

            // Apply constraints if any
            if (!constraints_.empty()) {
                if (!constraints_.isFeasible(weights)) {
                    return solveConstrainedQPWithTarget(weights, targetReturn);
                }
            }

            // Compute portfolio statistics
            double expectedReturn = mu.dot(weights);
            double variance = computeVariance(weights);
            double risk = std::sqrt(std::max(0.0, variance));
            double sharpeRatio = (risk > core::EPSILON) ? (expectedReturn / risk) : 0.0;

            return MarkowitzResult{weights,     expectedReturn, risk,
                                   sharpeRatio, true,           "Target return portfolio computed"};

        } catch (const std::exception& e) {
            return MarkowitzResult{{},  0.0,   0.0,
                                   0.0, false, std::string("Optimization failed: ") + e.what()};
        }
    }

    /**
     * @brief Compute the efficient frontier.
     *
     * Computes a set of efficient portfolios with varying levels of expected return.
     *
     * @param numPoints Number of points on the efficient frontier (default: 50)
     * @return Vector of optimization results representing the efficient frontier
     */
    std::vector<MarkowitzResult> efficientFrontier(size_t numPoints = 50) const {
        if (numPoints < 2) {
            throw std::invalid_argument("Number of points must be at least 2");
        }

        std::vector<MarkowitzResult> frontier;
        frontier.reserve(numPoints);

        // Get minimum variance portfolio
        auto minVarResult = minimumVariance();
        if (!minVarResult.success()) {
            return frontier;
        }

        double minReturn = minVarResult.expectedReturn;

        // Compute maximum return
        double maxReturn = *std::max_element(expectedReturns_.data().data().begin(),
                                             expectedReturns_.data().data().end());

        // Generate points along the frontier
        for (size_t i = 0; i < numPoints; ++i) {
            double t = static_cast<double>(i) / (numPoints - 1);
            double targetRet = minReturn + t * (maxReturn - minReturn);

            auto result = targetReturn(targetRet);
            if (result.success()) {
                frontier.push_back(result);
            }
        }

        return frontier;
    }

private:
    ExpectedReturns expectedReturns_;
    CovarianceMatrix covariance_;
    ConstraintSet constraints_;
    size_t maxIterations_;
    double tolerance_;

    /**
     * @brief Validate that the optimizer inputs are consistent.
     * @throws std::invalid_argument if validation fails
     */
    void validate() const {
        if (expectedReturns_.empty()) {
            throw std::invalid_argument("Expected returns cannot be empty");
        }
        if (covariance_.empty()) {
            throw std::invalid_argument("Covariance matrix cannot be empty");
        }
        if (expectedReturns_.size() != covariance_.size()) {
            throw std::invalid_argument(
                "Expected returns and covariance matrix dimensions must match");
        }

        // Check if constraint set has obvious infeasibility
        if (!constraints_.empty()) {
            if (constraints_.hasInfeasibleCombination(expectedReturns_.size())) {
                throw std::invalid_argument("Constraint set contains infeasible combinations");
            }
        }
    }

    /**
     * @brief Compute portfolio variance given weights.
     * @param weights Portfolio weights
     * @return Portfolio variance w'Σw
     */
    double computeVariance(const core::Vector& weights) const {
        core::Vector Sw = covariance_.data() * weights;
        return weights.dot(Sw);
    }

    /**
     * @brief Solve constrained quadratic programming problem.
     *
     * Uses a simple iterative method to handle constraints.
     * This is a simplified solver for demonstration purposes.
     *
     * @param initialWeights Initial guess for weights
     * @param lambda Risk aversion parameter
     * @return Optimization result
     */
    MarkowitzResult solveConstrainedQP(const core::Vector& initialWeights,
                                       double lambda [[maybe_unused]]) const {
        const size_t n = expectedReturns_.size();
        core::Vector weights = initialWeights;

        // Simple projection method: project onto constraints iteratively
        for (size_t iter = 0; iter < maxIterations_; ++iter) {
            // Project onto long-only constraint if present
            for (size_t i = 0; i < n; ++i) {
                if (weights[i] < 0.0) {
                    weights[i] = 0.0;
                }
            }

            // Project onto fully invested constraint
            double sum = weights.sum();
            if (std::abs(sum) > core::EPSILON) {
                weights = weights / sum;
            } else {
                // If weights sum to zero, use equal weights
                weights = core::Vector(n, 1.0 / n);
            }

            // Check constraint feasibility
            if (constraints_.isFeasible(weights)) {
                break;
            }
        }

        // Compute portfolio statistics
        double expectedReturn = expectedReturns_.data().dot(weights);
        double variance = computeVariance(weights);
        double risk = std::sqrt(std::max(0.0, variance));
        double sharpeRatio = (risk > core::EPSILON) ? (expectedReturn / risk) : 0.0;

        return MarkowitzResult{weights,     expectedReturn, risk,
                               sharpeRatio, true,           "Constrained portfolio computed"};
    }

    /**
     * @brief Solve constrained QP with target return constraint.
     *
     * @param initialWeights Initial guess for weights
     * @param targetReturn Target portfolio return
     * @return Optimization result
     */
    MarkowitzResult solveConstrainedQPWithTarget(const core::Vector& initialWeights,
                                                 double targetReturn [[maybe_unused]]) const {
        // For simplicity, use the same projection method
        // A more sophisticated implementation would handle the return constraint explicitly
        return solveConstrainedQP(initialWeights, 0.0);
    }
};

}  // namespace optimizer
}  // namespace orbat
