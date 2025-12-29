#pragma once

#include "orbat/core/constants.hpp"
#include "orbat/core/matrix.hpp"
#include "orbat/core/vector.hpp"
#include "orbat/optimizer/covariance_matrix.hpp"
#include "orbat/optimizer/expected_returns.hpp"
#include "orbat/optimizer/markowitz.hpp"

#include <cmath>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace orbat {
namespace optimizer {

/**
 * @brief A view on expected asset returns.
 *
 * Represents an investor's belief about the future return of a portfolio
 * of assets. The view is expressed as a linear combination of asset returns:
 *   P * returns = Q ± uncertainty
 *
 * where P is the view vector (weights on assets), Q is the expected return,
 * and uncertainty is captured in the confidence parameter.
 */
struct View {
    core::Vector assets;    // View vector P (one row of the P matrix)
    double expectedReturn;  // Expected return Q for this view
    double confidence;      // Confidence level (0 = no confidence, 1 = full confidence)

    /**
     * @brief Construct a view.
     * @param assets View vector (weights on assets)
     * @param expectedReturn Expected return for this view
     * @param confidence Confidence level (0 to 1, default 0.5)
     */
    View(const core::Vector& assets, double expectedReturn, double confidence = 0.5)
        : assets(assets), expectedReturn(expectedReturn), confidence(confidence) {
        if (confidence < 0.0 || confidence > 1.0) {
            throw std::invalid_argument("Confidence must be between 0 and 1");
        }
    }
};

/**
 * @brief Black-Litterman portfolio optimizer.
 *
 * Implements the Black-Litterman model for portfolio optimization, which
 * combines market equilibrium returns with investor views to produce
 * posterior expected returns. These posterior returns are then fed into
 * the Markowitz optimizer for final portfolio construction.
 *
 * The Black-Litterman model formula:
 *   μ_BL = [(τΣ)^(-1) + P'Ω^(-1)P]^(-1) [(τΣ)^(-1)Π + P'Ω^(-1)Q]
 *
 * Where:
 * - Π = equilibrium returns (implied from market weights)
 * - Σ = covariance matrix
 * - τ = scalar uncertainty parameter
 * - P = view matrix (K×N where K is number of views, N is number of assets)
 * - Q = view returns (K×1 vector)
 * - Ω = view uncertainty matrix (K×K diagonal matrix)
 *
 * Key properties:
 * - With zero views, returns the market portfolio
 * - High-confidence views dominate the prior
 * - Low-confidence views have little impact
 * - Smoothly interpolates between equilibrium and views
 *
 * Example:
 *   // Create Black-Litterman optimizer
 *   BlackLittermanOptimizer bl(marketWeights, covariance, riskAversion);
 *
 *   // Add a view: Asset 0 will return 12%
 *   View view1({1.0, 0.0, 0.0}, 0.12, 0.8);  // 80% confidence
 *   bl.addView(view1);
 *
 *   // Add a relative view: Asset 1 will outperform Asset 2 by 3%
 *   View view2({0.0, 1.0, -1.0}, 0.03, 0.6);  // 60% confidence
 *   bl.addView(view2);
 *
 *   // Compute posterior returns and optimize portfolio
 *   auto posteriorReturns = bl.computePosteriorReturns();
 *   auto result = bl.optimize();
 */
class BlackLittermanOptimizer {
public:
    /**
     * @brief Construct a Black-Litterman optimizer.
     *
     * @param marketWeights Market capitalization weights (equilibrium portfolio)
     * @param covariance Covariance matrix of asset returns
     * @param riskAversion Market risk aversion parameter (typically 2.0-4.0)
     * @param tau Uncertainty in prior (typically 0.01-0.05, default 0.025)
     * @throws std::invalid_argument if dimensions don't match or parameters are invalid
     */
    BlackLittermanOptimizer(const core::Vector& marketWeights, const CovarianceMatrix& covariance,
                            double riskAversion, double tau = 0.025)
        : marketWeights_(marketWeights), covariance_(covariance), riskAversion_(riskAversion),
          tau_(tau) {
        validate();
        computeEquilibriumReturns();
    }

    /**
     * @brief Add an investor view.
     *
     * @param view The view to add
     * @throws std::invalid_argument if view dimensions don't match
     */
    void addView(const View& view) {
        if (view.assets.size() != marketWeights_.size()) {
            throw std::invalid_argument("View dimensions must match number of assets");
        }
        views_.push_back(view);
    }

    /**
     * @brief Clear all views.
     */
    void clearViews() { views_.clear(); }

    /**
     * @brief Get the number of views.
     * @return Number of views
     */
    size_t numViews() const { return views_.size(); }

    /**
     * @brief Get equilibrium returns (implied from market weights).
     *
     * Equilibrium returns are computed using reverse optimization:
     *   Π = λ * Σ * w_market
     *
     * where λ is the market risk aversion parameter.
     *
     * @return Equilibrium returns vector
     */
    const core::Vector& equilibriumReturns() const { return equilibriumReturns_; }

    /**
     * @brief Compute posterior returns using Black-Litterman formula.
     *
     * If no views are present, returns the equilibrium returns.
     * Otherwise, computes the Black-Litterman posterior returns that
     * blend equilibrium returns with investor views.
     *
     * Formula:
     *   μ_BL = [(τΣ)^(-1) + P'Ω^(-1)P]^(-1) [(τΣ)^(-1)Π + P'Ω^(-1)Q]
     *
     * @return Posterior expected returns
     */
    ExpectedReturns computePosteriorReturns() const {
        if (views_.empty()) {
            // No views: return equilibrium returns
            return ExpectedReturns(equilibriumReturns_);
        }

        const size_t n = marketWeights_.size();
        const size_t k = views_.size();

        // Build view matrix P (K×N)
        core::Matrix P(k, n);
        core::Vector Q(k);
        core::Matrix Omega(k, k, 0.0);

        for (size_t i = 0; i < k; ++i) {
            const auto& view = views_[i];
            Q[i] = view.expectedReturn;

            // Copy view assets to P matrix
            for (size_t j = 0; j < n; ++j) {
                P(i, j) = view.assets[j];
            }

            // Set view uncertainty in Omega (diagonal matrix)
            // Ω_ii = (1/confidence - 1) * P_i * (τΣ) * P_i'
            // Higher confidence → lower uncertainty
            double viewVariance = 0.0;
            core::Vector PRow = view.assets;
            core::Vector tauSigmaPRow = covariance_.data() * PRow * tau_;
            viewVariance = PRow.dot(tauSigmaPRow);

            // Scale by confidence: high confidence = low uncertainty
            double confidenceFactor = (1.0 / view.confidence - 1.0);
            Omega(i, i) = viewVariance * confidenceFactor;

            // Ensure Omega is positive
            if (Omega(i, i) < core::EPSILON) {
                Omega(i, i) = core::EPSILON;
            }
        }

        // Compute τΣ
        core::Matrix tauSigma = covariance_.data() * tau_;

        // Compute (τΣ)^(-1)
        core::Matrix tauSigmaInv = tauSigma.inverse();

        // Compute Ω^(-1)
        core::Matrix OmegaInv = Omega.inverse();

        // Compute P'Ω^(-1)P
        core::Matrix Pt = P.transpose();
        core::Matrix PtOmegaInv = Pt * OmegaInv;
        core::Matrix PtOmegaInvP = PtOmegaInv * P;

        // Compute posterior precision: (τΣ)^(-1) + P'Ω^(-1)P
        core::Matrix posteriorPrecision = tauSigmaInv + PtOmegaInvP;

        // Compute posterior covariance: [(τΣ)^(-1) + P'Ω^(-1)P]^(-1)
        core::Matrix posteriorCovariance = posteriorPrecision.inverse();

        // Compute (τΣ)^(-1)Π
        core::Vector tauSigmaInvPi = tauSigmaInv * equilibriumReturns_;

        // Compute P'Ω^(-1)Q
        core::Vector PtOmegaInvQ = PtOmegaInv * Q;

        // Compute posterior mean: [(τΣ)^(-1) + P'Ω^(-1)P]^(-1) [(τΣ)^(-1)Π + P'Ω^(-1)Q]
        core::Vector posteriorMean = posteriorCovariance * (tauSigmaInvPi + PtOmegaInvQ);

        return ExpectedReturns(posteriorMean);
    }

    /**
     * @brief Optimize portfolio using Black-Litterman posterior returns.
     *
     * Computes posterior returns and uses Markowitz optimization to
     * find the optimal portfolio.
     *
     * @param lambda Risk aversion parameter for optimization (default: use market risk aversion)
     * @return Markowitz optimization result
     */
    MarkowitzResult optimize(double lambda = -1.0) const {
        // Use market risk aversion if not specified
        if (lambda < 0.0) {
            lambda = riskAversion_;
        }

        // Compute posterior returns
        ExpectedReturns posteriorReturns = computePosteriorReturns();

        // Create Markowitz optimizer with posterior returns
        MarkowitzOptimizer markowitz(posteriorReturns, covariance_);

        // Optimize with given risk aversion
        return markowitz.optimize(lambda);
    }

    /**
     * @brief Get market weights.
     * @return Market capitalization weights
     */
    const core::Vector& marketWeights() const { return marketWeights_; }

    /**
     * @brief Get covariance matrix.
     * @return Covariance matrix
     */
    const CovarianceMatrix& covariance() const { return covariance_; }

    /**
     * @brief Get risk aversion parameter.
     * @return Market risk aversion
     */
    double riskAversion() const { return riskAversion_; }

    /**
     * @brief Get tau parameter.
     * @return Uncertainty in prior
     */
    double tau() const { return tau_; }

private:
    core::Vector marketWeights_;       // Market equilibrium weights
    CovarianceMatrix covariance_;      // Covariance matrix
    double riskAversion_;              // Market risk aversion parameter
    double tau_;                       // Uncertainty in prior
    core::Vector equilibriumReturns_;  // Implied equilibrium returns
    std::vector<View> views_;          // Investor views

    /**
     * @brief Validate optimizer inputs.
     * @throws std::invalid_argument if validation fails
     */
    void validate() const {
        if (marketWeights_.empty()) {
            throw std::invalid_argument("Market weights cannot be empty");
        }
        if (covariance_.empty()) {
            throw std::invalid_argument("Covariance matrix cannot be empty");
        }
        if (marketWeights_.size() != covariance_.size()) {
            throw std::invalid_argument("Market weights and covariance dimensions must match");
        }
        if (riskAversion_ <= 0.0) {
            throw std::invalid_argument("Risk aversion must be positive");
        }
        if (tau_ <= 0.0) {
            throw std::invalid_argument("Tau must be positive");
        }

        // Check that market weights sum to approximately 1
        double sum = marketWeights_.sum();
        if (std::abs(sum - 1.0) > 1e-6) {
            throw std::invalid_argument("Market weights must sum to 1.0");
        }

        // Check that market weights are non-negative
        for (size_t i = 0; i < marketWeights_.size(); ++i) {
            if (marketWeights_[i] < -core::EPSILON) {
                throw std::invalid_argument("Market weights must be non-negative");
            }
        }
    }

    /**
     * @brief Compute equilibrium returns using reverse optimization.
     *
     * Formula: Π = λ * Σ * w_market
     *
     * This represents the expected returns implied by the market portfolio,
     * assuming investors are mean-variance optimizers.
     */
    void computeEquilibriumReturns() {
        // Π = λ * Σ * w
        equilibriumReturns_ = covariance_.data() * marketWeights_ * riskAversion_;
    }
};

}  // namespace optimizer
}  // namespace orbat
