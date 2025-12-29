#include "orbat/optimizer/markowitz.hpp"

#include <cmath>

#include <gtest/gtest.h>

using orbat::core::Matrix;
using orbat::core::Vector;
using orbat::optimizer::CovarianceMatrix;
using orbat::optimizer::ExpectedReturns;
using orbat::optimizer::MarkowitzOptimizer;

// Helper function to check if weights are approximately equal
bool weightsEqual(const Vector& w1, const Vector& w2, double tol = 1e-6) {
    if (w1.size() != w2.size()) {
        return false;
    }
    for (size_t i = 0; i < w1.size(); ++i) {
        if (std::abs(w1[i] - w2[i]) > tol) {
            return false;
        }
    }
    return true;
}

// Test construction
TEST(MarkowitzOptimizerTest, BasicConstruction) {
    ExpectedReturns returns({0.10, 0.12, 0.15});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    EXPECT_NO_THROW(MarkowitzOptimizer optimizer(returns, cov));
}

TEST(MarkowitzOptimizerTest, ConstructionWithConstraints) {
    ExpectedReturns returns({0.10, 0.12, 0.15});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    orbat::optimizer::ConstraintSet constraints;
    constraints.add(std::make_shared<orbat::optimizer::FullyInvestedConstraint>());
    constraints.add(std::make_shared<orbat::optimizer::LongOnlyConstraint>());

    EXPECT_NO_THROW(MarkowitzOptimizer optimizer(returns, cov, constraints));
}

TEST(MarkowitzOptimizerTest, EmptyReturns) {
    // ExpectedReturns itself will throw on empty vector
    EXPECT_THROW(
        {
            ExpectedReturns returns(Vector(0));
            CovarianceMatrix cov({{0.04}});
            MarkowitzOptimizer optimizer(returns, cov);
        },
        std::invalid_argument);
}

TEST(MarkowitzOptimizerTest, DimensionMismatch) {
    ExpectedReturns returns({0.10, 0.12});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    EXPECT_THROW(MarkowitzOptimizer optimizer(returns, cov), std::invalid_argument);
}

// Test setters
TEST(MarkowitzOptimizerTest, SetMaxIterations) {
    ExpectedReturns returns({0.10, 0.12});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});

    MarkowitzOptimizer optimizer(returns, cov);
    EXPECT_NO_THROW(optimizer.setMaxIterations(100));
    EXPECT_THROW(optimizer.setMaxIterations(0), std::invalid_argument);
}

TEST(MarkowitzOptimizerTest, SetTolerance) {
    ExpectedReturns returns({0.10, 0.12});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});

    MarkowitzOptimizer optimizer(returns, cov);
    EXPECT_NO_THROW(optimizer.setTolerance(1e-10));
    EXPECT_THROW(optimizer.setTolerance(0.0), std::invalid_argument);
    EXPECT_THROW(optimizer.setTolerance(-1e-6), std::invalid_argument);
}

// Test minimum variance portfolio
TEST(MarkowitzOptimizerTest, MinimumVarianceTwoAssets) {
    // Simple two-asset case with known analytical solution
    ExpectedReturns returns({0.10, 0.12});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto result = optimizer.minimumVariance();

    EXPECT_TRUE(result.success());
    EXPECT_EQ(result.weights.size(), 2);

    // Check fully invested constraint
    EXPECT_NEAR(result.weights.sum(), 1.0, 1e-6);

    // For minimum variance with these parameters:
    // σ1² = 0.04, σ2² = 0.0225, σ12 = 0.01
    // w1 = (σ2² - σ12) / (σ1² + σ2² - 2σ12) = (0.0225 - 0.01) / (0.04 + 0.0225 - 0.02)
    //    = 0.0125 / 0.0425 ≈ 0.294
    // w2 = 1 - w1 ≈ 0.706

    EXPECT_NEAR(result.weights[0], 0.294, 0.01);
    EXPECT_NEAR(result.weights[1], 0.706, 0.01);

    // Verify portfolio statistics
    EXPECT_GT(result.risk, 0.0);
    EXPECT_GT(result.expectedReturn, 0.0);
}

TEST(MarkowitzOptimizerTest, MinimumVarianceThreeAssets) {
    ExpectedReturns returns({0.08, 0.12, 0.15});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto result = optimizer.minimumVariance();

    EXPECT_TRUE(result.success());
    EXPECT_EQ(result.weights.size(), 3);

    // Check fully invested constraint
    EXPECT_NEAR(result.weights.sum(), 1.0, 1e-6);

    // Check that risk is positive
    EXPECT_GT(result.risk, 0.0);
}

TEST(MarkowitzOptimizerTest, MinimumVarianceUncorrelatedAssets) {
    // Uncorrelated assets with different variances
    ExpectedReturns returns({0.10, 0.10, 0.10});
    CovarianceMatrix cov({{0.04, 0.0, 0.0}, {0.0, 0.09, 0.0}, {0.0, 0.0, 0.16}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto result = optimizer.minimumVariance();

    EXPECT_TRUE(result.success());

    // With uncorrelated assets, minimum variance portfolio should allocate more to lower variance
    // assets For equal returns, optimal weights are inversely proportional to variances
    // w_i ∝ 1/σ_i²
    // w1 ∝ 1/0.04 = 25, w2 ∝ 1/0.09 ≈ 11.11, w3 ∝ 1/0.16 = 6.25

    EXPECT_GT(result.weights[0], result.weights[1]);
    EXPECT_GT(result.weights[1], result.weights[2]);
    EXPECT_NEAR(result.weights.sum(), 1.0, 1e-6);
}

TEST(MarkowitzOptimizerTest, MinimumVarianceEqualAssets) {
    // All assets identical - should result in equal weights
    ExpectedReturns returns({0.10, 0.10, 0.10});
    CovarianceMatrix cov({{0.04, 0.02, 0.02}, {0.02, 0.04, 0.02}, {0.02, 0.02, 0.04}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto result = optimizer.minimumVariance();

    EXPECT_TRUE(result.success());

    // Should have approximately equal weights due to symmetry
    EXPECT_NEAR(result.weights[0], 1.0 / 3.0, 1e-4);
    EXPECT_NEAR(result.weights[1], 1.0 / 3.0, 1e-4);
    EXPECT_NEAR(result.weights[2], 1.0 / 3.0, 1e-4);
}

// Test mean-variance optimization with risk aversion
TEST(MarkowitzOptimizerTest, OptimizeZeroLambda) {
    // Lambda = 0 should give minimum variance
    ExpectedReturns returns({0.10, 0.12});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto result1 = optimizer.minimumVariance();
    auto result2 = optimizer.optimize(0.0);

    EXPECT_TRUE(result2.success());
    EXPECT_TRUE(weightsEqual(result1.weights, result2.weights, 1e-4));
}

TEST(MarkowitzOptimizerTest, OptimizeNegativeLambda) {
    ExpectedReturns returns({0.10, 0.12});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});

    MarkowitzOptimizer optimizer(returns, cov);
    EXPECT_THROW(optimizer.optimize(-0.5), std::invalid_argument);
}

TEST(MarkowitzOptimizerTest, OptimizeIncreasingLambda) {
    // Higher lambda should shift towards higher return assets
    ExpectedReturns returns({0.08, 0.12, 0.15});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);

    auto result1 = optimizer.optimize(0.1);
    auto result2 = optimizer.optimize(1.0);
    auto result3 = optimizer.optimize(10.0);

    EXPECT_TRUE(result1.success());
    EXPECT_TRUE(result2.success());
    EXPECT_TRUE(result3.success());

    // Expected return should increase with lambda
    EXPECT_LT(result1.expectedReturn, result2.expectedReturn);
    EXPECT_LT(result2.expectedReturn, result3.expectedReturn);

    // Risk should also increase (generally)
    // Note: This may not be strictly monotonic for all parameter combinations
    EXPECT_GT(result3.risk, result1.risk);
}

TEST(MarkowitzOptimizerTest, OptimizeLargeLambda) {
    // Very large lambda should put most weight on highest return asset
    ExpectedReturns returns({0.08, 0.12, 0.18});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto result = optimizer.optimize(1000.0);

    EXPECT_TRUE(result.success());

    // Should allocate heavily to asset with highest return (index 2)
    EXPECT_GT(result.weights[2], 0.5);
}

// Test target return optimization
TEST(MarkowitzOptimizerTest, TargetReturnBasic) {
    ExpectedReturns returns({0.10, 0.15});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});

    MarkowitzOptimizer optimizer(returns, cov);

    // Target return between min and max should succeed
    auto result = optimizer.targetReturn(0.12);

    EXPECT_TRUE(result.success());
    EXPECT_NEAR(result.expectedReturn, 0.12, 1e-4);
    EXPECT_NEAR(result.weights.sum(), 1.0, 1e-6);
}

TEST(MarkowitzOptimizerTest, TargetReturnMinimum) {
    ExpectedReturns returns({0.10, 0.15});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});

    MarkowitzOptimizer optimizer(returns, cov);

    // Get minimum variance portfolio return
    auto minVar = optimizer.minimumVariance();
    EXPECT_TRUE(minVar.success());

    // Target return at minimum variance should give same portfolio
    auto result = optimizer.targetReturn(minVar.expectedReturn);

    EXPECT_TRUE(result.success());
    EXPECT_NEAR(result.expectedReturn, minVar.expectedReturn, 1e-4);
    EXPECT_NEAR(result.risk, minVar.risk, 1e-4);
}

TEST(MarkowitzOptimizerTest, TargetReturnTooLow) {
    ExpectedReturns returns({0.10, 0.15});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});

    MarkowitzOptimizer optimizer(returns, cov);

    // Target return below minimum should fail
    auto result = optimizer.targetReturn(0.05);

    EXPECT_FALSE(result.success());
}

TEST(MarkowitzOptimizerTest, TargetReturnTooHigh) {
    ExpectedReturns returns({0.10, 0.15});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});

    MarkowitzOptimizer optimizer(returns, cov);

    // Target return above maximum should fail
    auto result = optimizer.targetReturn(0.20);

    EXPECT_FALSE(result.success());
}

TEST(MarkowitzOptimizerTest, TargetReturnInterpolation) {
    // Test multiple target returns along the frontier
    ExpectedReturns returns({0.08, 0.12, 0.16});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);

    auto result1 = optimizer.targetReturn(0.10);
    auto result2 = optimizer.targetReturn(0.12);
    auto result3 = optimizer.targetReturn(0.14);

    EXPECT_TRUE(result1.success());
    EXPECT_TRUE(result2.success());
    EXPECT_TRUE(result3.success());

    // Returns should match targets
    EXPECT_NEAR(result1.expectedReturn, 0.10, 1e-4);
    EXPECT_NEAR(result2.expectedReturn, 0.12, 1e-4);
    EXPECT_NEAR(result3.expectedReturn, 0.14, 1e-4);

    // Weights should sum to 1
    EXPECT_NEAR(result1.weights.sum(), 1.0, 1e-6);
    EXPECT_NEAR(result2.weights.sum(), 1.0, 1e-6);
    EXPECT_NEAR(result3.weights.sum(), 1.0, 1e-6);
}

// Test efficient frontier
TEST(MarkowitzOptimizerTest, EfficientFrontierBasic) {
    ExpectedReturns returns({0.08, 0.12, 0.16});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto frontier = optimizer.efficientFrontier(10);

    EXPECT_EQ(frontier.size(), 10);

    // All portfolios should be successful
    for (const auto& result : frontier) {
        EXPECT_TRUE(result.success());
        EXPECT_NEAR(result.weights.sum(), 1.0, 1e-6);
        EXPECT_GT(result.risk, 0.0);
    }
}

TEST(MarkowitzOptimizerTest, EfficientFrontierMonotonicity) {
    // Expected return should increase along the frontier
    ExpectedReturns returns({0.08, 0.12, 0.16});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto frontier = optimizer.efficientFrontier(20);

    EXPECT_GE(frontier.size(), 2);

    // Check that expected return increases (or stays same)
    for (size_t i = 1; i < frontier.size(); ++i) {
        EXPECT_GE(frontier[i].expectedReturn, frontier[i - 1].expectedReturn - 1e-6);
    }

    // Risk should generally increase (not strictly monotonic at very beginning)
    // Check last is greater than first
    EXPECT_GT(frontier.back().risk, frontier.front().risk);
}

TEST(MarkowitzOptimizerTest, EfficientFrontierTooFewPoints) {
    ExpectedReturns returns({0.08, 0.12});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});

    MarkowitzOptimizer optimizer(returns, cov);

    EXPECT_THROW(optimizer.efficientFrontier(0), std::invalid_argument);
    EXPECT_THROW(optimizer.efficientFrontier(1), std::invalid_argument);
}

TEST(MarkowitzOptimizerTest, EfficientFrontierLargeNumber) {
    ExpectedReturns returns({0.08, 0.12, 0.16});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto frontier = optimizer.efficientFrontier(100);

    EXPECT_EQ(frontier.size(), 100);
}

// Test analytical solutions for special cases
TEST(MarkowitzOptimizerTest, AnalyticalTwoAssetCase) {
    // Two uncorrelated assets with equal variance
    // Optimal weights should be 50-50 for minimum variance
    ExpectedReturns returns({0.10, 0.12});
    CovarianceMatrix cov({{0.04, 0.0}, {0.0, 0.04}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto result = optimizer.minimumVariance();

    EXPECT_TRUE(result.success());
    EXPECT_NEAR(result.weights[0], 0.5, 1e-6);
    EXPECT_NEAR(result.weights[1], 0.5, 1e-6);
}

TEST(MarkowitzOptimizerTest, AnalyticalPerfectlyCorrelated) {
    // Two perfectly correlated assets
    // σ1 = 0.2, σ2 = 0.3, ρ = 1.0
    // This creates a singular covariance matrix, which should fail
    ExpectedReturns returns({0.10, 0.10});
    CovarianceMatrix cov({{0.04, 0.06}, {0.06, 0.09}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto result = optimizer.minimumVariance();

    // Perfectly correlated assets create a singular covariance matrix
    // The optimizer should fail gracefully
    EXPECT_FALSE(result.success());
}

// Test with constraints
TEST(MarkowitzOptimizerTest, LongOnlyConstraint) {
    ExpectedReturns returns({0.10, 0.12, 0.08});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    orbat::optimizer::ConstraintSet constraints;
    constraints.add(std::make_shared<orbat::optimizer::LongOnlyConstraint>());

    MarkowitzOptimizer optimizer(returns, cov, constraints);
    auto result = optimizer.minimumVariance();

    EXPECT_TRUE(result.success());

    // All weights should be non-negative
    for (size_t i = 0; i < result.weights.size(); ++i) {
        EXPECT_GE(result.weights[i], -1e-6);
    }
}

// Test numerical stability
TEST(MarkowitzOptimizerTest, NumericalStabilitySmallVariances) {
    // Small but reasonable variances
    ExpectedReturns returns({0.05, 0.06, 0.07});
    CovarianceMatrix cov(
        {{0.0001, 0.00005, 0.00003}, {0.00005, 0.00009, 0.00004}, {0.00003, 0.00004, 0.00008}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto result = optimizer.minimumVariance();

    EXPECT_TRUE(result.success());
    EXPECT_NEAR(result.weights.sum(), 1.0, 1e-6);
    EXPECT_GT(result.risk, 0.0);
}

TEST(MarkowitzOptimizerTest, NumericalStabilityLargeVariances) {
    // Large variances
    ExpectedReturns returns({0.20, 0.25, 0.30});
    CovarianceMatrix cov({{1.0, 0.3, 0.2}, {0.3, 1.5, 0.4}, {0.2, 0.4, 2.0}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto result = optimizer.minimumVariance();

    EXPECT_TRUE(result.success());
    EXPECT_NEAR(result.weights.sum(), 1.0, 1e-6);
    EXPECT_GT(result.risk, 0.0);
}

// Test edge cases
TEST(MarkowitzOptimizerTest, SingleAsset) {
    ExpectedReturns returns({0.10});
    CovarianceMatrix cov({{0.04}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto result = optimizer.minimumVariance();

    EXPECT_TRUE(result.success());
    EXPECT_NEAR(result.weights[0], 1.0, 1e-6);
    EXPECT_NEAR(result.expectedReturn, 0.10, 1e-6);
    EXPECT_NEAR(result.risk, 0.2, 1e-6);  // sqrt(0.04) = 0.2
}

TEST(MarkowitzOptimizerTest, ManyAssets) {
    // Test with 10 assets
    std::vector<double> returnsVec(10);
    std::vector<std::vector<double>> covVec(10, std::vector<double>(10));

    for (size_t i = 0; i < 10; ++i) {
        returnsVec[i] = 0.08 + 0.01 * i;  // Returns from 8% to 17%
        for (size_t j = 0; j < 10; ++j) {
            if (i == j) {
                covVec[i][j] = 0.04 + 0.005 * i;  // Diagonal variances
            } else {
                covVec[i][j] = 0.005;  // Small correlation
            }
        }
    }

    Vector returnsVector(returnsVec);
    ExpectedReturns returns(returnsVector);
    Matrix covMat(10, 10);
    for (size_t i = 0; i < 10; ++i) {
        for (size_t j = 0; j < 10; ++j) {
            covMat(i, j) = covVec[i][j];
        }
    }
    CovarianceMatrix cov(covMat);

    MarkowitzOptimizer optimizer(returns, cov);
    auto result = optimizer.minimumVariance();

    EXPECT_TRUE(result.success());
    EXPECT_NEAR(result.weights.sum(), 1.0, 1e-6);
    EXPECT_EQ(result.weights.size(), 10);
}

// Test result structure
TEST(MarkowitzOptimizerTest, ResultStructure) {
    ExpectedReturns returns({0.10, 0.12});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto result = optimizer.minimumVariance();

    EXPECT_TRUE(result.success());
    EXPECT_TRUE(result.converged);
    EXPECT_FALSE(result.message.empty());
    EXPECT_EQ(result.weights.size(), 2);
    EXPECT_TRUE(std::isfinite(result.expectedReturn));
    EXPECT_TRUE(std::isfinite(result.risk));
}
