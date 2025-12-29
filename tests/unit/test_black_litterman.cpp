#include "orbat/optimizer/black_litterman.hpp"

#include <cmath>

#include <gtest/gtest.h>

using orbat::core::Matrix;
using orbat::core::Vector;
using orbat::optimizer::BlackLittermanOptimizer;
using orbat::optimizer::CovarianceMatrix;
using orbat::optimizer::View;

// Helper function to check if vectors are approximately equal
bool vectorsEqual(const Vector& v1, const Vector& v2, double tol = 1e-6) {
    if (v1.size() != v2.size()) {
        return false;
    }
    for (size_t i = 0; i < v1.size(); ++i) {
        if (std::abs(v1[i] - v2[i]) > tol) {
            return false;
        }
    }
    return true;
}

// Test construction
TEST(BlackLittermanOptimizerTest, BasicConstruction) {
    Vector marketWeights({0.4, 0.3, 0.3});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});
    double riskAversion = 2.5;

    EXPECT_NO_THROW(BlackLittermanOptimizer bl(marketWeights, cov, riskAversion));
}

TEST(BlackLittermanOptimizerTest, ConstructionWithCustomTau) {
    Vector marketWeights({0.5, 0.5});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});
    double riskAversion = 2.5;
    double tau = 0.05;

    EXPECT_NO_THROW(BlackLittermanOptimizer bl(marketWeights, cov, riskAversion, tau));
}

TEST(BlackLittermanOptimizerTest, EmptyMarketWeights) {
    Vector marketWeights(0);
    CovarianceMatrix cov({{0.04}});
    double riskAversion = 2.5;

    EXPECT_THROW(BlackLittermanOptimizer bl(marketWeights, cov, riskAversion),
                 std::invalid_argument);
}

TEST(BlackLittermanOptimizerTest, DimensionMismatch) {
    Vector marketWeights({0.5, 0.5});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});
    double riskAversion = 2.5;

    EXPECT_THROW(BlackLittermanOptimizer bl(marketWeights, cov, riskAversion),
                 std::invalid_argument);
}

TEST(BlackLittermanOptimizerTest, InvalidRiskAversion) {
    Vector marketWeights({0.5, 0.5});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});

    EXPECT_THROW(BlackLittermanOptimizer bl(marketWeights, cov, 0.0), std::invalid_argument);
    EXPECT_THROW(BlackLittermanOptimizer bl(marketWeights, cov, -1.0), std::invalid_argument);
}

TEST(BlackLittermanOptimizerTest, InvalidTau) {
    Vector marketWeights({0.5, 0.5});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});
    double riskAversion = 2.5;

    EXPECT_THROW(BlackLittermanOptimizer bl(marketWeights, cov, riskAversion, 0.0),
                 std::invalid_argument);
    EXPECT_THROW(BlackLittermanOptimizer bl(marketWeights, cov, riskAversion, -0.01),
                 std::invalid_argument);
}

TEST(BlackLittermanOptimizerTest, MarketWeightsDontSumToOne) {
    Vector marketWeights({0.3, 0.3});  // Sums to 0.6
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});
    double riskAversion = 2.5;

    EXPECT_THROW(BlackLittermanOptimizer bl(marketWeights, cov, riskAversion),
                 std::invalid_argument);
}

TEST(BlackLittermanOptimizerTest, NegativeMarketWeights) {
    Vector marketWeights({0.6, 0.5, -0.1});  // Negative weight
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});
    double riskAversion = 2.5;

    EXPECT_THROW(BlackLittermanOptimizer bl(marketWeights, cov, riskAversion),
                 std::invalid_argument);
}

// Test equilibrium returns
TEST(BlackLittermanOptimizerTest, EquilibriumReturnsCalculation) {
    Vector marketWeights({0.5, 0.5});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});
    double riskAversion = 2.5;

    BlackLittermanOptimizer bl(marketWeights, cov, riskAversion);

    // Equilibrium returns: Π = λ * Σ * w
    // Σ * w = [0.04*0.5 + 0.01*0.5, 0.01*0.5 + 0.0225*0.5] = [0.025, 0.01625]
    // Π = 2.5 * [0.025, 0.01625] = [0.0625, 0.040625]
    const auto& pi = bl.equilibriumReturns();
    EXPECT_NEAR(pi[0], 0.0625, 1e-6);
    EXPECT_NEAR(pi[1], 0.040625, 1e-6);
}

// Test views
TEST(BlackLittermanOptimizerTest, AddView) {
    Vector marketWeights({0.4, 0.3, 0.3});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});
    double riskAversion = 2.5;

    BlackLittermanOptimizer bl(marketWeights, cov, riskAversion);

    View view({1.0, 0.0, 0.0}, 0.12, 0.8);
    EXPECT_NO_THROW(bl.addView(view));
    EXPECT_EQ(bl.numViews(), 1);
}

TEST(BlackLittermanOptimizerTest, AddMultipleViews) {
    Vector marketWeights({0.4, 0.3, 0.3});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});
    double riskAversion = 2.5;

    BlackLittermanOptimizer bl(marketWeights, cov, riskAversion);

    View view1({1.0, 0.0, 0.0}, 0.12, 0.8);
    View view2({0.0, 1.0, -1.0}, 0.03, 0.6);

    bl.addView(view1);
    bl.addView(view2);

    EXPECT_EQ(bl.numViews(), 2);
}

TEST(BlackLittermanOptimizerTest, ClearViews) {
    Vector marketWeights({0.4, 0.3, 0.3});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});
    double riskAversion = 2.5;

    BlackLittermanOptimizer bl(marketWeights, cov, riskAversion);

    View view1({1.0, 0.0, 0.0}, 0.12, 0.8);
    View view2({0.0, 1.0, -1.0}, 0.03, 0.6);

    bl.addView(view1);
    bl.addView(view2);
    EXPECT_EQ(bl.numViews(), 2);

    bl.clearViews();
    EXPECT_EQ(bl.numViews(), 0);
}

TEST(BlackLittermanOptimizerTest, ViewDimensionMismatch) {
    Vector marketWeights({0.5, 0.5});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});
    double riskAversion = 2.5;

    BlackLittermanOptimizer bl(marketWeights, cov, riskAversion);

    // View has 3 assets but portfolio has 2
    View view({1.0, 0.0, 0.0}, 0.12, 0.8);
    EXPECT_THROW(bl.addView(view), std::invalid_argument);
}

TEST(BlackLittermanOptimizerTest, ViewInvalidConfidence) {
    Vector marketWeights({0.5, 0.5});

    // Confidence > 1
    EXPECT_THROW(View({1.0, 0.0}, 0.12, 1.5), std::invalid_argument);

    // Confidence < 0
    EXPECT_THROW(View({1.0, 0.0}, 0.12, -0.1), std::invalid_argument);
}

// Test posterior returns - zero views case
TEST(BlackLittermanOptimizerTest, ZeroViewsEqualsEquilibrium) {
    Vector marketWeights({0.5, 0.5});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});
    double riskAversion = 2.5;

    BlackLittermanOptimizer bl(marketWeights, cov, riskAversion);

    // No views added - posterior should equal equilibrium
    auto posteriorReturns = bl.computePosteriorReturns();
    const auto& equilibriumReturns = bl.equilibriumReturns();

    EXPECT_TRUE(vectorsEqual(posteriorReturns.data(), equilibriumReturns, 1e-6));
}

// Test posterior returns - single view with high confidence
TEST(BlackLittermanOptimizerTest, HighConfidenceViewDominates) {
    Vector marketWeights({0.5, 0.5});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});
    double riskAversion = 2.5;

    BlackLittermanOptimizer bl(marketWeights, cov, riskAversion);

    // Add high confidence view: Asset 0 will return 10%
    View view({1.0, 0.0}, 0.10, 0.99);  // 99% confidence
    bl.addView(view);

    auto posteriorReturns = bl.computePosteriorReturns();

    // With very high confidence, posterior should be close to view
    EXPECT_NEAR(posteriorReturns[0], 0.10, 1e-3);  // Close to view return
}

// Test posterior returns - single view with low confidence
TEST(BlackLittermanOptimizerTest, LowConfidenceViewHasLittleImpact) {
    Vector marketWeights({0.5, 0.5});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});
    double riskAversion = 2.5;

    BlackLittermanOptimizer bl(marketWeights, cov, riskAversion);

    const auto& equilibriumReturns = bl.equilibriumReturns();

    // Add low confidence view: Asset 0 will return 10%
    View view({1.0, 0.0}, 0.10, 0.01);  // 1% confidence
    bl.addView(view);

    auto posteriorReturns = bl.computePosteriorReturns();

    // With very low confidence, posterior should be close to equilibrium
    EXPECT_NEAR(posteriorReturns[0], equilibriumReturns[0], 1e-3);
    EXPECT_NEAR(posteriorReturns[1], equilibriumReturns[1], 1e-3);
}

// Test posterior returns - absolute view
TEST(BlackLittermanOptimizerTest, AbsoluteView) {
    Vector marketWeights({0.4, 0.3, 0.3});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});
    double riskAversion = 2.5;

    BlackLittermanOptimizer bl(marketWeights, cov, riskAversion);

    // Absolute view: Asset 0 will return 12%
    View view({1.0, 0.0, 0.0}, 0.12, 0.5);
    bl.addView(view);

    auto posteriorReturns = bl.computePosteriorReturns();
    const auto& equilibriumReturns = bl.equilibriumReturns();

    // Posterior should be between equilibrium and view
    EXPECT_GT(posteriorReturns[0], equilibriumReturns[0]);  // Moved toward view
    EXPECT_LT(posteriorReturns[0], 0.12);                   // Not fully at view (50% confidence)
}

// Test posterior returns - relative view
TEST(BlackLittermanOptimizerTest, RelativeView) {
    Vector marketWeights({0.5, 0.5});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});
    double riskAversion = 2.5;

    BlackLittermanOptimizer bl(marketWeights, cov, riskAversion);

    // Relative view: Asset 0 will outperform Asset 1 by 3%
    View view({1.0, -1.0}, 0.03, 0.7);
    bl.addView(view);

    auto posteriorReturns = bl.computePosteriorReturns();
    const auto& equilibriumReturns = bl.equilibriumReturns();

    // Check that the view moved returns in the right direction
    double equilibriumSpread = equilibriumReturns[0] - equilibriumReturns[1];
    double posteriorSpread = posteriorReturns[0] - posteriorReturns[1];

    // Posterior spread should be closer to 3% than equilibrium spread
    EXPECT_GT(posteriorSpread, equilibriumSpread);
}

// Test posterior returns - multiple views
TEST(BlackLittermanOptimizerTest, MultipleViews) {
    Vector marketWeights({0.4, 0.3, 0.3});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});
    double riskAversion = 2.5;

    BlackLittermanOptimizer bl(marketWeights, cov, riskAversion);

    // View 1: Asset 0 will return 12%
    View view1({1.0, 0.0, 0.0}, 0.12, 0.8);
    bl.addView(view1);

    // View 2: Asset 1 will outperform Asset 2 by 3%
    View view2({0.0, 1.0, -1.0}, 0.03, 0.6);
    bl.addView(view2);

    auto posteriorReturns = bl.computePosteriorReturns();

    // Posterior should incorporate both views
    EXPECT_EQ(posteriorReturns.size(), 3);

    // All returns should be finite
    for (size_t i = 0; i < posteriorReturns.size(); ++i) {
        EXPECT_TRUE(std::isfinite(posteriorReturns[i]));
    }
}

// Test optimization with zero views
TEST(BlackLittermanOptimizerTest, OptimizeWithZeroViewsUsesEquilibrium) {
    Vector marketWeights({0.5, 0.5});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});
    double riskAversion = 2.5;

    BlackLittermanOptimizer bl(marketWeights, cov, riskAversion);

    // With no views, posterior returns should equal equilibrium
    auto posteriorReturns = bl.computePosteriorReturns();
    const auto& equilibriumReturns = bl.equilibriumReturns();

    EXPECT_TRUE(vectorsEqual(posteriorReturns.data(), equilibriumReturns, 1e-6));

    // Optimize with equilibrium returns
    auto result = bl.optimize();
    EXPECT_TRUE(result.success());

    // The optimization should succeed and produce valid weights
    EXPECT_EQ(result.weights.size(), 2);
    EXPECT_TRUE(std::isfinite(result.expectedReturn));
    EXPECT_TRUE(std::isfinite(result.risk));
}

// Test optimization with views
TEST(BlackLittermanOptimizerTest, OptimizeWithViews) {
    Vector marketWeights({0.5, 0.5});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});
    double riskAversion = 2.5;

    BlackLittermanOptimizer bl(marketWeights, cov, riskAversion);

    // Add a strong view that Asset 0 is better
    View view({1.0, 0.0}, 0.15, 0.9);
    bl.addView(view);

    auto result = bl.optimize();

    EXPECT_TRUE(result.success());
    EXPECT_EQ(result.weights.size(), 2);

    // Should tilt toward Asset 0
    EXPECT_GT(result.weights[0], marketWeights[0]);
    EXPECT_LT(result.weights[1], marketWeights[1]);
}

// Test optimization with custom lambda
TEST(BlackLittermanOptimizerTest, OptimizeWithCustomLambda) {
    Vector marketWeights({0.5, 0.5});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});
    double riskAversion = 2.5;

    BlackLittermanOptimizer bl(marketWeights, cov, riskAversion);

    View view({1.0, 0.0}, 0.15, 0.9);
    bl.addView(view);

    auto result1 = bl.optimize(1.0);   // Low risk aversion
    auto result2 = bl.optimize(10.0);  // High risk aversion

    EXPECT_TRUE(result1.success());
    EXPECT_TRUE(result2.success());

    // Higher risk aversion should lead to more aggressive pursuit of returns
    EXPECT_GT(result2.expectedReturn, result1.expectedReturn);
}

// Test getters
TEST(BlackLittermanOptimizerTest, Getters) {
    Vector marketWeights({0.5, 0.5});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});
    double riskAversion = 2.5;
    double tau = 0.03;

    BlackLittermanOptimizer bl(marketWeights, cov, riskAversion, tau);

    EXPECT_TRUE(vectorsEqual(bl.marketWeights(), marketWeights));
    EXPECT_EQ(bl.riskAversion(), riskAversion);
    EXPECT_EQ(bl.tau(), tau);
}

// Test typical use case
TEST(BlackLittermanOptimizerTest, TypicalUseCase) {
    // Three-asset portfolio: stocks, bonds, commodities
    Vector marketWeights({0.5, 0.3, 0.2});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});
    double riskAversion = 2.5;

    BlackLittermanOptimizer bl(marketWeights, cov, riskAversion);

    // View 1: Stocks will return 10%
    View view1({1.0, 0.0, 0.0}, 0.10, 0.7);
    bl.addView(view1);

    // View 2: Bonds will outperform commodities by 2%
    View view2({0.0, 1.0, -1.0}, 0.02, 0.5);
    bl.addView(view2);

    // Compute posterior returns
    auto posteriorReturns = bl.computePosteriorReturns();
    EXPECT_EQ(posteriorReturns.size(), 3);

    // Optimize portfolio
    auto result = bl.optimize();
    EXPECT_TRUE(result.success());
    EXPECT_EQ(result.weights.size(), 3);

    // Weights should sum to 1
    EXPECT_NEAR(result.weights.sum(), 1.0, 1e-6);

    // Portfolio should have finite return and risk
    EXPECT_TRUE(std::isfinite(result.expectedReturn));
    EXPECT_TRUE(std::isfinite(result.risk));
    EXPECT_GE(result.risk, 0.0);
}

// Test numerical stability
TEST(BlackLittermanOptimizerTest, NumericalStability) {
    Vector marketWeights({0.25, 0.25, 0.25, 0.25});
    CovarianceMatrix cov({{0.01, 0.002, 0.001, 0.0005},
                          {0.002, 0.015, 0.003, 0.001},
                          {0.001, 0.003, 0.02, 0.002},
                          {0.0005, 0.001, 0.002, 0.012}});
    double riskAversion = 3.0;

    BlackLittermanOptimizer bl(marketWeights, cov, riskAversion);

    // Add multiple views
    View view1({1.0, 0.0, 0.0, 0.0}, 0.08, 0.6);
    View view2({0.0, 1.0, 0.0, 0.0}, 0.06, 0.5);
    View view3({0.0, 0.0, 1.0, -1.0}, 0.03, 0.7);

    bl.addView(view1);
    bl.addView(view2);
    bl.addView(view3);

    auto posteriorReturns = bl.computePosteriorReturns();
    auto result = bl.optimize();

    EXPECT_TRUE(result.success());

    // All values should be finite
    for (size_t i = 0; i < result.weights.size(); ++i) {
        EXPECT_TRUE(std::isfinite(result.weights[i]));
    }
    EXPECT_TRUE(std::isfinite(result.expectedReturn));
    EXPECT_TRUE(std::isfinite(result.risk));
}

// Test edge case: single asset
TEST(BlackLittermanOptimizerTest, SingleAsset) {
    Vector marketWeights({1.0});
    CovarianceMatrix cov({{0.04}});
    double riskAversion = 2.5;

    BlackLittermanOptimizer bl(marketWeights, cov, riskAversion);

    // Equilibrium return
    EXPECT_NEAR(bl.equilibriumReturns()[0], 0.1, 1e-6);  // 2.5 * 0.04 * 1.0

    // Optimize with no views
    auto result = bl.optimize();
    EXPECT_TRUE(result.success());
    EXPECT_NEAR(result.weights[0], 1.0, 1e-6);
}
