#pragma once

#include "orbat/core/constants.hpp"
#include "orbat/core/vector.hpp"

#include <cmath>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace orbat {
namespace optimizer {

/**
 * @brief Abstract base class for portfolio constraints.
 *
 * This interface defines the contract for portfolio constraints that can be
 * applied during optimization. Constraints are used to enforce investment
 * rules and regulatory requirements.
 *
 * Constraints follow CFA Institute best practices for portfolio construction
 * and are designed to be composable and reusable across different optimization
 * strategies.
 *
 * Example:
 *   auto fullyInvested = std::make_shared<FullyInvestedConstraint>();
 *   auto longOnly = std::make_shared<LongOnlyConstraint>();
 *   Vector weights({0.3, 0.4, 0.3});
 *   if (fullyInvested->isFeasible(weights) && longOnly->isFeasible(weights)) {
 *       // Weights satisfy both constraints
 *   }
 */
class Constraint {
public:
    /**
     * @brief Virtual destructor for proper cleanup of derived classes.
     */
    virtual ~Constraint() = default;

    /**
     * @brief Check if a portfolio weight vector satisfies this constraint.
     *
     * @param weights Portfolio weights vector
     * @return true if the weights satisfy the constraint within tolerance
     * @return false otherwise
     */
    virtual bool isFeasible(const core::Vector& weights) const = 0;

    /**
     * @brief Get a human-readable name for this constraint.
     *
     * @return Constraint name
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Get a detailed description of the constraint.
     *
     * @return Constraint description
     */
    virtual std::string getDescription() const = 0;
};

/**
 * @brief Constraint that enforces fully invested portfolios (weights sum to 1).
 *
 * This is a fundamental constraint in portfolio optimization that ensures
 * all capital is allocated. The sum of weights must equal 1.0 within a
 * specified tolerance.
 *
 * This constraint is consistent with CFA Institute guidelines on portfolio
 * construction and is typically required for long-only portfolios.
 *
 * Example:
 *   FullyInvestedConstraint constraint(1e-6);  // Custom tolerance
 *   Vector weights({0.4, 0.35, 0.25});
 *   bool feasible = constraint.isFeasible(weights);  // true
 */
class FullyInvestedConstraint : public Constraint {
public:
    /**
     * @brief Construct a fully invested constraint with default tolerance.
     *
     * Uses the library's default EPSILON for tolerance checking.
     */
    FullyInvestedConstraint() : tolerance_(core::EPSILON) {}

    /**
     * @brief Construct a fully invested constraint with custom tolerance.
     *
     * @param tolerance Maximum allowed deviation from sum = 1.0
     * @throws std::invalid_argument if tolerance is negative
     */
    explicit FullyInvestedConstraint(double tolerance) : tolerance_(tolerance) {
        if (tolerance < 0.0) {
            throw std::invalid_argument("Tolerance must be non-negative");
        }
    }

    /**
     * @brief Check if weights sum to 1.0 within tolerance.
     *
     * @param weights Portfolio weights vector
     * @return true if |sum(weights) - 1.0| <= tolerance
     */
    bool isFeasible(const core::Vector& weights) const override {
        if (weights.empty()) {
            return false;
        }
        double sum = weights.sum();
        return std::abs(sum - 1.0) <= tolerance_;
    }

    /**
     * @brief Get the name of this constraint.
     *
     * @return "FullyInvested"
     */
    std::string getName() const override { return "FullyInvested"; }

    /**
     * @brief Get the description of this constraint.
     *
     * @return Detailed description including tolerance
     */
    std::string getDescription() const override {
        return "Portfolio weights must sum to 1.0 (tolerance: " + std::to_string(tolerance_) + ")";
    }

    /**
     * @brief Get the tolerance for this constraint.
     *
     * @return Tolerance value
     */
    double getTolerance() const { return tolerance_; }

private:
    double tolerance_;
};

/**
 * @brief Constraint that enforces non-negative weights (no short positions).
 *
 * This constraint ensures that all portfolio weights are greater than or
 * equal to zero, which is equivalent to prohibiting short selling.
 *
 * Long-only constraints are common in many institutional portfolios and
 * are consistent with CFA Institute guidelines for traditional portfolio
 * management.
 *
 * Example:
 *   LongOnlyConstraint constraint;
 *   Vector weights({0.4, 0.35, 0.25});
 *   bool feasible = constraint.isFeasible(weights);  // true
 *
 *   Vector shortWeights({0.5, -0.2, 0.7});
 *   feasible = constraint.isFeasible(shortWeights);  // false
 */
class LongOnlyConstraint : public Constraint {
public:
    /**
     * @brief Construct a long-only constraint with default tolerance.
     */
    LongOnlyConstraint() : tolerance_(core::EPSILON) {}

    /**
     * @brief Construct a long-only constraint with custom tolerance.
     *
     * @param tolerance Maximum allowed negative deviation (for numerical stability)
     * @throws std::invalid_argument if tolerance is negative
     */
    explicit LongOnlyConstraint(double tolerance) : tolerance_(tolerance) {
        if (tolerance < 0.0) {
            throw std::invalid_argument("Tolerance must be non-negative");
        }
    }

    /**
     * @brief Check if all weights are non-negative.
     *
     * @param weights Portfolio weights vector
     * @return true if all weights >= -tolerance
     */
    bool isFeasible(const core::Vector& weights) const override {
        if (weights.empty()) {
            return false;
        }
        for (size_t i = 0; i < weights.size(); ++i) {
            if (weights[i] < -tolerance_) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Get the name of this constraint.
     *
     * @return "LongOnly"
     */
    std::string getName() const override { return "LongOnly"; }

    /**
     * @brief Get the description of this constraint.
     *
     * @return Detailed description
     */
    std::string getDescription() const override {
        return "All portfolio weights must be non-negative (no short selling)";
    }

    /**
     * @brief Get the tolerance for this constraint.
     *
     * @return Tolerance value
     */
    double getTolerance() const { return tolerance_; }

private:
    double tolerance_;
};

/**
 * @brief Constraint that enforces bounds on individual asset weights.
 *
 * Box constraints limit each asset's weight to a specified range [lower, upper].
 * This is useful for:
 * - Limiting concentration risk
 * - Enforcing regulatory requirements
 * - Implementing portfolio management rules
 *
 * Box constraints are a standard tool in portfolio optimization and align
 * with CFA Institute guidance on risk management and diversification.
 *
 * Example:
 *   // Each asset between 0% and 40%
 *   BoxConstraint constraint(0.0, 0.4);
 *   Vector weights({0.3, 0.35, 0.35});
 *   bool feasible = constraint.isFeasible(weights);  // true
 *
 *   // Per-asset bounds
 *   std::vector<double> lower = {0.1, 0.0, 0.0};
 *   std::vector<double> upper = {0.5, 0.4, 0.6};
 *   BoxConstraint customConstraint(lower, upper);
 */
class BoxConstraint : public Constraint {
public:
    /**
     * @brief Construct box constraint with uniform bounds for all assets.
     *
     * @param lowerBound Lower bound for all assets
     * @param upperBound Upper bound for all assets
     * @throws std::invalid_argument if lower > upper
     */
    BoxConstraint(double lowerBound, double upperBound)
        : lowerBounds_(), upperBounds_(), uniformLower_(lowerBound), uniformUpper_(upperBound),
          uniformBounds_(true), tolerance_(core::EPSILON) {
        if (lowerBound > upperBound) {
            throw std::invalid_argument("Lower bound must be <= upper bound");
        }
    }

    /**
     * @brief Construct box constraint with uniform bounds and custom tolerance.
     *
     * @param lowerBound Lower bound for all assets
     * @param upperBound Upper bound for all assets
     * @param tolerance Tolerance for bound checking
     * @throws std::invalid_argument if lower > upper or tolerance < 0
     */
    BoxConstraint(double lowerBound, double upperBound, double tolerance)
        : lowerBounds_(), upperBounds_(), uniformLower_(lowerBound), uniformUpper_(upperBound),
          uniformBounds_(true), tolerance_(tolerance) {
        if (lowerBound > upperBound) {
            throw std::invalid_argument("Lower bound must be <= upper bound");
        }
        if (tolerance < 0.0) {
            throw std::invalid_argument("Tolerance must be non-negative");
        }
    }

    /**
     * @brief Construct box constraint with per-asset bounds.
     *
     * @param lowerBounds Vector of lower bounds for each asset
     * @param upperBounds Vector of upper bounds for each asset
     * @throws std::invalid_argument if sizes don't match or any lower[i] > upper[i]
     */
    BoxConstraint(const std::vector<double>& lowerBounds, const std::vector<double>& upperBounds)
        : lowerBounds_(lowerBounds), upperBounds_(upperBounds), uniformLower_(0.0),
          uniformUpper_(0.0), uniformBounds_(false), tolerance_(core::EPSILON) {
        validate();
    }

    /**
     * @brief Construct box constraint with per-asset bounds and custom tolerance.
     *
     * @param lowerBounds Vector of lower bounds for each asset
     * @param upperBounds Vector of upper bounds for each asset
     * @param tolerance Tolerance for bound checking
     * @throws std::invalid_argument if sizes don't match, any lower[i] > upper[i], or tolerance < 0
     */
    BoxConstraint(const std::vector<double>& lowerBounds, const std::vector<double>& upperBounds,
                  double tolerance)
        : lowerBounds_(lowerBounds), upperBounds_(upperBounds), uniformLower_(0.0),
          uniformUpper_(0.0), uniformBounds_(false), tolerance_(tolerance) {
        if (tolerance < 0.0) {
            throw std::invalid_argument("Tolerance must be non-negative");
        }
        validate();
    }

    /**
     * @brief Check if all weights are within their respective bounds.
     *
     * @param weights Portfolio weights vector
     * @return true if all weights[i] in [lower[i] - tolerance, upper[i] + tolerance]
     */
    bool isFeasible(const core::Vector& weights) const override {
        if (weights.empty()) {
            return false;
        }

        if (uniformBounds_) {
            // Check uniform bounds for all assets
            for (size_t i = 0; i < weights.size(); ++i) {
                if (weights[i] < uniformLower_ - tolerance_ ||
                    weights[i] > uniformUpper_ + tolerance_) {
                    return false;
                }
            }
        } else {
            // Check per-asset bounds
            if (weights.size() != lowerBounds_.size()) {
                return false;
            }
            for (size_t i = 0; i < weights.size(); ++i) {
                if (weights[i] < lowerBounds_[i] - tolerance_ ||
                    weights[i] > upperBounds_[i] + tolerance_) {
                    return false;
                }
            }
        }

        return true;
    }

    /**
     * @brief Get the name of this constraint.
     *
     * @return "BoxConstraint"
     */
    std::string getName() const override { return "BoxConstraint"; }

    /**
     * @brief Get the description of this constraint.
     *
     * @return Detailed description of bounds
     */
    std::string getDescription() const override {
        if (uniformBounds_) {
            return "All weights must be in [" + std::to_string(uniformLower_) + ", " +
                   std::to_string(uniformUpper_) + "]";
        } else {
            return "Weights must satisfy per-asset bounds";
        }
    }

    /**
     * @brief Get the tolerance for this constraint.
     *
     * @return Tolerance value
     */
    double getTolerance() const { return tolerance_; }

    /**
     * @brief Check if using uniform bounds.
     *
     * @return true if uniform bounds, false if per-asset bounds
     */
    bool hasUniformBounds() const { return uniformBounds_; }

    /**
     * @brief Get uniform lower bound (only valid if hasUniformBounds() is true).
     *
     * @return Uniform lower bound
     */
    double getUniformLower() const { return uniformLower_; }

    /**
     * @brief Get uniform upper bound (only valid if hasUniformBounds() is true).
     *
     * @return Uniform upper bound
     */
    double getUniformUpper() const { return uniformUpper_; }

    /**
     * @brief Get per-asset lower bounds (only valid if hasUniformBounds() is false).
     *
     * @return Vector of lower bounds
     */
    const std::vector<double>& getLowerBounds() const { return lowerBounds_; }

    /**
     * @brief Get per-asset upper bounds (only valid if hasUniformBounds() is false).
     *
     * @return Vector of upper bounds
     */
    const std::vector<double>& getUpperBounds() const { return upperBounds_; }

private:
    std::vector<double> lowerBounds_;
    std::vector<double> upperBounds_;
    double uniformLower_;
    double uniformUpper_;
    bool uniformBounds_;
    double tolerance_;

    /**
     * @brief Validate per-asset bounds.
     *
     * @throws std::invalid_argument if validation fails
     */
    void validate() const {
        if (lowerBounds_.size() != upperBounds_.size()) {
            throw std::invalid_argument("Lower and upper bounds must have the same size");
        }
        if (lowerBounds_.empty()) {
            throw std::invalid_argument("Bounds vectors cannot be empty");
        }
        for (size_t i = 0; i < lowerBounds_.size(); ++i) {
            if (lowerBounds_[i] > upperBounds_[i]) {
                throw std::invalid_argument("Lower bound must be <= upper bound for all assets");
            }
        }
    }
};

/**
 * @brief Container for managing multiple portfolio constraints.
 *
 * This class allows composing multiple constraints and checking them
 * collectively. It provides utilities for:
 * - Adding and removing constraints
 * - Checking if a portfolio satisfies all constraints
 * - Detecting infeasible constraint combinations
 *
 * Example:
 *   ConstraintSet constraints;
 *   constraints.add(std::make_shared<FullyInvestedConstraint>());
 *   constraints.add(std::make_shared<LongOnlyConstraint>());
 *   constraints.add(std::make_shared<BoxConstraint>(0.0, 0.4));
 *
 *   Vector weights({0.3, 0.35, 0.35});
 *   if (constraints.isFeasible(weights)) {
 *       // All constraints satisfied
 *   }
 */
class ConstraintSet {
public:
    /**
     * @brief Construct an empty constraint set.
     */
    ConstraintSet() = default;

    /**
     * @brief Add a constraint to the set.
     *
     * @param constraint Shared pointer to constraint
     * @throws std::invalid_argument if constraint is nullptr
     */
    void add(std::shared_ptr<Constraint> constraint) {
        if (!constraint) {
            throw std::invalid_argument("Cannot add null constraint");
        }
        constraints_.push_back(constraint);
    }

    /**
     * @brief Get the number of constraints in the set.
     *
     * @return Number of constraints
     */
    size_t size() const { return constraints_.size(); }

    /**
     * @brief Check if the set is empty.
     *
     * @return true if no constraints, false otherwise
     */
    bool empty() const { return constraints_.empty(); }

    /**
     * @brief Clear all constraints from the set.
     */
    void clear() { constraints_.clear(); }

    /**
     * @brief Check if a portfolio satisfies all constraints.
     *
     * @param weights Portfolio weights vector
     * @return true if all constraints are satisfied
     */
    bool isFeasible(const core::Vector& weights) const {
        for (const auto& constraint : constraints_) {
            if (!constraint->isFeasible(weights)) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Get all constraints in the set.
     *
     * @return Vector of constraint shared pointers
     */
    const std::vector<std::shared_ptr<Constraint>>& getConstraints() const { return constraints_; }

    /**
     * @brief Detect if the constraint set contains obviously infeasible combinations.
     *
     * Performs basic feasibility checks:
     * - Fully invested + long-only with n assets requires each weight <= 1.0
     * - Fully invested + box constraints with sum(upper bounds) < 1.0 is infeasible
     * - Fully invested + box constraints with sum(lower bounds) > 1.0 is infeasible
     *
     * @param numAssets Number of assets in the portfolio
     * @return true if an infeasible combination is detected
     * @throws std::invalid_argument if numAssets is 0
     */
    bool hasInfeasibleCombination(size_t numAssets) const {
        if (numAssets == 0) {
            throw std::invalid_argument("Number of assets must be positive");
        }

        bool hasFullyInvested = false;
        bool hasLongOnly = false;
        const BoxConstraint* boxConstraint = nullptr;

        // Identify constraints
        for (const auto& constraint : constraints_) {
            if (dynamic_cast<const FullyInvestedConstraint*>(constraint.get())) {
                hasFullyInvested = true;
            } else if (dynamic_cast<const LongOnlyConstraint*>(constraint.get())) {
                hasLongOnly = true;
            } else if (auto* box = dynamic_cast<const BoxConstraint*>(constraint.get())) {
                boxConstraint = box;
            }
        }

        // Check fully invested + box constraint compatibility
        if (hasFullyInvested && boxConstraint) {
            if (boxConstraint->hasUniformBounds()) {
                double lower = boxConstraint->getUniformLower();
                double upper = boxConstraint->getUniformUpper();

                // Check if sum of lower bounds exceeds 1.0
                if (lower * numAssets > 1.0 + core::EPSILON) {
                    return true;
                }

                // Check if sum of upper bounds is less than 1.0
                if (upper * numAssets < 1.0 - core::EPSILON) {
                    return true;
                }
            } else {
                // Per-asset bounds
                const auto& lowerBounds = boxConstraint->getLowerBounds();
                const auto& upperBounds = boxConstraint->getUpperBounds();

                if (lowerBounds.size() != numAssets) {
                    return true;  // Size mismatch is infeasible
                }

                double sumLower = 0.0;
                double sumUpper = 0.0;
                for (size_t i = 0; i < numAssets; ++i) {
                    sumLower += lowerBounds[i];
                    sumUpper += upperBounds[i];
                }

                // Check if sum of lower bounds exceeds 1.0
                if (sumLower > 1.0 + core::EPSILON) {
                    return true;
                }

                // Check if sum of upper bounds is less than 1.0
                if (sumUpper < 1.0 - core::EPSILON) {
                    return true;
                }
            }
        }

        // Check long-only + box constraint compatibility
        if (hasLongOnly && boxConstraint) {
            if (boxConstraint->hasUniformBounds()) {
                // If uniform upper bound is negative, incompatible with long-only
                if (boxConstraint->getUniformUpper() < -core::EPSILON) {
                    return true;
                }
            } else {
                // Check per-asset bounds
                const auto& upperBounds = boxConstraint->getUpperBounds();
                for (double upper : upperBounds) {
                    if (upper < -core::EPSILON) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

private:
    std::vector<std::shared_ptr<Constraint>> constraints_;
};

}  // namespace optimizer
}  // namespace orbat
