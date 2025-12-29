#include "orbat/optimizer/markowitz.hpp"

#include <cmath>
#include <sstream>

#include <gtest/gtest.h>

using orbat::core::Vector;
using orbat::optimizer::CovarianceMatrix;
using orbat::optimizer::ExpectedReturns;
using orbat::optimizer::MarkowitzOptimizer;
using orbat::optimizer::MarkowitzResult;

// Test basic structure
TEST(OptimizationResultTest, BasicStructure) {
    Vector weights({0.3, 0.5, 0.2});
    MarkowitzResult result{weights, 0.12, 0.15, 0.8, true, "Success"};

    EXPECT_EQ(result.weights.size(), 3);
    EXPECT_DOUBLE_EQ(result.weights[0], 0.3);
    EXPECT_DOUBLE_EQ(result.weights[1], 0.5);
    EXPECT_DOUBLE_EQ(result.weights[2], 0.2);
    EXPECT_DOUBLE_EQ(result.expectedReturn, 0.12);
    EXPECT_DOUBLE_EQ(result.risk, 0.15);
    EXPECT_DOUBLE_EQ(result.sharpeRatio, 0.8);
    EXPECT_TRUE(result.converged);
    EXPECT_EQ(result.message, "Success");
    EXPECT_TRUE(result.success());
}

// Test Sharpe ratio calculation
TEST(OptimizationResultTest, SharpeRatioCalculation) {
    ExpectedReturns returns({0.10, 0.12, 0.15});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto result = optimizer.minimumVariance();

    ASSERT_TRUE(result.success());

    // Sharpe ratio should be expectedReturn / risk (assuming rf = 0)
    double expectedSharpe = result.expectedReturn / result.risk;
    EXPECT_NEAR(result.sharpeRatio, expectedSharpe, 1e-6);
}

// Test JSON serialization
TEST(OptimizationResultTest, JSONSerialization) {
    Vector weights({0.3, 0.5, 0.2});
    MarkowitzResult result{weights, 0.12, 0.15, 0.8, true, "Test message"};

    std::string json = result.toJSON();

    // Check that JSON contains expected fields
    EXPECT_TRUE(json.find("\"converged\": true") != std::string::npos);
    EXPECT_TRUE(json.find("\"message\": \"Test message\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"expectedReturn\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"risk\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"sharpeRatio\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"weights\"") != std::string::npos);
    EXPECT_TRUE(json.find("0.30000000") != std::string::npos);
    EXPECT_TRUE(json.find("0.50000000") != std::string::npos);
    EXPECT_TRUE(json.find("0.20000000") != std::string::npos);
}

// Test JSON deserialization
TEST(OptimizationResultTest, JSONDeserialization) {
    Vector weights({0.3, 0.5, 0.2});
    MarkowitzResult original{weights, 0.12, 0.15, 0.8, true, "Test message"};

    std::string json = original.toJSON();
    MarkowitzResult deserialized = MarkowitzResult::fromJSON(json);

    EXPECT_EQ(deserialized.converged, original.converged);
    EXPECT_EQ(deserialized.message, original.message);
    EXPECT_NEAR(deserialized.expectedReturn, original.expectedReturn, 1e-6);
    EXPECT_NEAR(deserialized.risk, original.risk, 1e-6);
    EXPECT_NEAR(deserialized.sharpeRatio, original.sharpeRatio, 1e-6);
    EXPECT_EQ(deserialized.weights.size(), original.weights.size());
    for (size_t i = 0; i < original.weights.size(); ++i) {
        EXPECT_NEAR(deserialized.weights[i], original.weights[i], 1e-6);
    }
}

// Test JSON round-trip
TEST(OptimizationResultTest, JSONRoundTrip) {
    ExpectedReturns returns({0.10, 0.12, 0.15});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto original = optimizer.optimize(0.5);

    ASSERT_TRUE(original.success());

    // Serialize and deserialize
    std::string json = original.toJSON();
    MarkowitzResult roundtrip = MarkowitzResult::fromJSON(json);

    // Verify all fields match
    EXPECT_EQ(roundtrip.converged, original.converged);
    EXPECT_EQ(roundtrip.message, original.message);
    EXPECT_NEAR(roundtrip.expectedReturn, original.expectedReturn, 1e-6);
    EXPECT_NEAR(roundtrip.risk, original.risk, 1e-6);
    EXPECT_NEAR(roundtrip.sharpeRatio, original.sharpeRatio, 1e-6);
    EXPECT_EQ(roundtrip.weights.size(), original.weights.size());
    for (size_t i = 0; i < original.weights.size(); ++i) {
        EXPECT_NEAR(roundtrip.weights[i], original.weights[i], 1e-6)
            << "Weight mismatch at index " << i;
    }
}

// Test CSV serialization
TEST(OptimizationResultTest, CSVSerialization) {
    Vector weights({0.3, 0.5, 0.2});
    MarkowitzResult result{weights, 0.12, 0.15, 0.8, true, "Test message"};

    std::string csv = result.toCSV(true);

    // Check that CSV contains header
    EXPECT_TRUE(csv.find("converged,message,expectedReturn,risk,sharpeRatio") != std::string::npos);
    EXPECT_TRUE(csv.find("weight_0") != std::string::npos);
    EXPECT_TRUE(csv.find("weight_1") != std::string::npos);
    EXPECT_TRUE(csv.find("weight_2") != std::string::npos);

    // Check data row
    EXPECT_TRUE(csv.find("true") != std::string::npos);
    EXPECT_TRUE(csv.find("Test message") != std::string::npos);
}

// Test CSV without header
TEST(OptimizationResultTest, CSVWithoutHeader) {
    Vector weights({0.3, 0.5, 0.2});
    MarkowitzResult result{weights, 0.12, 0.15, 0.8, true, "Test"};

    std::string csv = result.toCSV(false);

    // Should not contain header
    EXPECT_TRUE(csv.find("converged,message") == std::string::npos);

    // But should contain data
    EXPECT_TRUE(csv.find("true") != std::string::npos);
    EXPECT_TRUE(csv.find("0.30000000") != std::string::npos);
}

// Test optimizer result consistency
TEST(OptimizationResultTest, OptimizerInternalConsistency) {
    ExpectedReturns returns({0.10, 0.12, 0.15});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);

    // Test minimum variance
    auto minVar = optimizer.minimumVariance();
    ASSERT_TRUE(minVar.success());

    // Verify weights sum to 1
    EXPECT_NEAR(minVar.weights.sum(), 1.0, 1e-6);

    // Verify expected return matches weighted sum of returns
    double calculatedReturn = 0.0;
    for (size_t i = 0; i < minVar.weights.size(); ++i) {
        calculatedReturn += minVar.weights[i] * returns[i];
    }
    EXPECT_NEAR(minVar.expectedReturn, calculatedReturn, 1e-6);

    // Verify Sharpe ratio
    double calculatedSharpe = minVar.expectedReturn / minVar.risk;
    EXPECT_NEAR(minVar.sharpeRatio, calculatedSharpe, 1e-6);
}

// Test optimizer with risk aversion consistency
TEST(OptimizationResultTest, OptimizerRiskAversionConsistency) {
    ExpectedReturns returns({0.10, 0.12, 0.15});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);

    auto result = optimizer.optimize(1.0);
    ASSERT_TRUE(result.success());

    // Verify consistency
    EXPECT_NEAR(result.weights.sum(), 1.0, 1e-6);

    double calculatedReturn = 0.0;
    for (size_t i = 0; i < result.weights.size(); ++i) {
        calculatedReturn += result.weights[i] * returns[i];
    }
    EXPECT_NEAR(result.expectedReturn, calculatedReturn, 1e-6);

    double calculatedSharpe = result.expectedReturn / result.risk;
    EXPECT_NEAR(result.sharpeRatio, calculatedSharpe, 1e-6);
}

// Test optimizer with target return consistency
TEST(OptimizationResultTest, OptimizerTargetReturnConsistency) {
    ExpectedReturns returns({0.10, 0.12, 0.15});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);

    double targetReturn = 0.12;
    auto result = optimizer.targetReturn(targetReturn);
    ASSERT_TRUE(result.success());

    // Verify consistency
    EXPECT_NEAR(result.weights.sum(), 1.0, 1e-6);
    EXPECT_NEAR(result.expectedReturn, targetReturn, 1e-5);

    double calculatedSharpe = result.expectedReturn / result.risk;
    EXPECT_NEAR(result.sharpeRatio, calculatedSharpe, 1e-6);
}

// Test uniform output across optimization methods
TEST(OptimizationResultTest, UniformOutputAcrossMethods) {
    ExpectedReturns returns({0.10, 0.12, 0.15});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);

    // Get results from different methods
    auto minVar = optimizer.minimumVariance();
    auto riskAversion = optimizer.optimize(0.5);
    auto targetRet = optimizer.targetReturn(0.12);

    // All should have the same structure
    ASSERT_TRUE(minVar.success());
    ASSERT_TRUE(riskAversion.success());
    ASSERT_TRUE(targetRet.success());

    // All should have same number of weights
    EXPECT_EQ(minVar.weights.size(), 3);
    EXPECT_EQ(riskAversion.weights.size(), 3);
    EXPECT_EQ(targetRet.weights.size(), 3);

    // All should have sharpe ratio calculated
    EXPECT_GT(minVar.sharpeRatio, 0.0);
    EXPECT_GT(riskAversion.sharpeRatio, 0.0);
    EXPECT_GT(targetRet.sharpeRatio, 0.0);

    // All should serialize to JSON with same structure
    std::string json1 = minVar.toJSON();
    std::string json2 = riskAversion.toJSON();
    std::string json3 = targetRet.toJSON();

    EXPECT_TRUE(json1.find("\"sharpeRatio\"") != std::string::npos);
    EXPECT_TRUE(json2.find("\"sharpeRatio\"") != std::string::npos);
    EXPECT_TRUE(json3.find("\"sharpeRatio\"") != std::string::npos);
}

// Test failed optimization result
TEST(OptimizationResultTest, FailedOptimization) {
    Vector emptyWeights;
    MarkowitzResult result{emptyWeights, 0.0, 0.0, 0.0, false, "Optimization failed"};

    EXPECT_FALSE(result.success());
    EXPECT_FALSE(result.converged);

    // Should still serialize
    std::string json = result.toJSON();
    EXPECT_TRUE(json.find("\"converged\": false") != std::string::npos);
    EXPECT_TRUE(json.find("Optimization failed") != std::string::npos);

    std::string csv = result.toCSV();
    EXPECT_TRUE(csv.find("false") != std::string::npos);
}

// Test JSON with special characters in message
TEST(OptimizationResultTest, JSONWithSpecialCharacters) {
    Vector weights({0.5, 0.5});
    MarkowitzResult result{weights, 0.1, 0.1, 1.0, true, "Success: optimization complete"};

    std::string json = result.toJSON();
    MarkowitzResult deserialized = MarkowitzResult::fromJSON(json);

    EXPECT_EQ(deserialized.message, "Success: optimization complete");
}

// Test empty weights
TEST(OptimizationResultTest, EmptyWeights) {
    Vector emptyWeights;
    MarkowitzResult result{emptyWeights, 0.0, 0.0, 0.0, false, "No solution"};

    std::string json = result.toJSON();
    EXPECT_TRUE(json.find("\"weights\": []") != std::string::npos);

    MarkowitzResult deserialized = MarkowitzResult::fromJSON(json);
    EXPECT_EQ(deserialized.weights.size(), 0);
}

// Test zero risk edge case
TEST(OptimizationResultTest, ZeroRiskEdgeCase) {
    Vector weights({1.0, 0.0, 0.0});
    MarkowitzResult result{weights, 0.1, 0.0, 0.0, true, "Zero risk"};

    EXPECT_DOUBLE_EQ(result.sharpeRatio, 0.0);

    std::string json = result.toJSON();
    MarkowitzResult deserialized = MarkowitzResult::fromJSON(json);

    EXPECT_DOUBLE_EQ(deserialized.sharpeRatio, 0.0);
}

// Test custom risk-free rate
TEST(OptimizationResultTest, CustomRiskFreeRate) {
    Vector weights({0.3, 0.5, 0.2});
    double expectedReturn = 0.12;
    double risk = 0.15;
    double defaultSharpe = expectedReturn / risk;

    MarkowitzResult result{weights, expectedReturn, risk, defaultSharpe, true, "Success"};

    // Default Sharpe ratio (rf = 0)
    EXPECT_NEAR(result.sharpeRatio, 0.8, 1e-6);

    // Calculate Sharpe with rf = 0.02 (2%)
    double sharpeWith2Pct = result.calculateSharpeRatio(0.02);
    double expectedSharpe = (0.12 - 0.02) / 0.15;
    EXPECT_NEAR(sharpeWith2Pct, expectedSharpe, 1e-6);
    EXPECT_NEAR(sharpeWith2Pct, 0.6666666667, 1e-6);

    // Original sharpeRatio field should not change
    EXPECT_NEAR(result.sharpeRatio, 0.8, 1e-6);

    // Calculate Sharpe with rf = 0.05 (5%)
    double sharpeWith5Pct = result.calculateSharpeRatio(0.05);
    expectedSharpe = (0.12 - 0.05) / 0.15;
    EXPECT_NEAR(sharpeWith5Pct, expectedSharpe, 1e-6);
    EXPECT_NEAR(sharpeWith5Pct, 0.4666666667, 1e-6);
}

// Test setRiskFreeRate method
TEST(OptimizationResultTest, SetRiskFreeRate) {
    Vector weights({0.3, 0.5, 0.2});
    double expectedReturn = 0.12;
    double risk = 0.15;
    double defaultSharpe = expectedReturn / risk;

    MarkowitzResult result{weights, expectedReturn, risk, defaultSharpe, true, "Success"};

    // Default Sharpe ratio (rf = 0)
    EXPECT_NEAR(result.sharpeRatio, 0.8, 1e-6);

    // Update to use rf = 0.03 (3%)
    result.setRiskFreeRate(0.03);
    double expectedSharpe = (0.12 - 0.03) / 0.15;
    EXPECT_NEAR(result.sharpeRatio, expectedSharpe, 1e-6);
    EXPECT_NEAR(result.sharpeRatio, 0.6, 1e-6);

    // Verify all other fields remain unchanged
    EXPECT_DOUBLE_EQ(result.expectedReturn, 0.12);
    EXPECT_DOUBLE_EQ(result.risk, 0.15);
    EXPECT_EQ(result.weights.size(), 3);
}

// Test risk-free rate with optimizer results
TEST(OptimizationResultTest, RiskFreeRateWithOptimizerResults) {
    ExpectedReturns returns({0.10, 0.12, 0.15});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto result = optimizer.minimumVariance();

    ASSERT_TRUE(result.success());

    // Default Sharpe (rf = 0)
    double defaultSharpe = result.sharpeRatio;
    EXPECT_GT(defaultSharpe, 0.0);

    // Calculate with different risk-free rates
    double sharpeWith2Pct = result.calculateSharpeRatio(0.02);
    double sharpeWith5Pct = result.calculateSharpeRatio(0.05);

    // Higher risk-free rate should give lower Sharpe ratio
    EXPECT_LT(sharpeWith2Pct, defaultSharpe);
    EXPECT_LT(sharpeWith5Pct, sharpeWith2Pct);

    // Verify calculation
    double expectedSharpe2 = (result.expectedReturn - 0.02) / result.risk;
    EXPECT_NEAR(sharpeWith2Pct, expectedSharpe2, 1e-6);
}

// Test zero risk with custom risk-free rate
TEST(OptimizationResultTest, ZeroRiskWithCustomRiskFreeRate) {
    Vector weights({1.0, 0.0, 0.0});
    MarkowitzResult result{weights, 0.1, 0.0, 0.0, true, "Zero risk"};

    // Should return 0 for any risk-free rate
    EXPECT_DOUBLE_EQ(result.calculateSharpeRatio(0.0), 0.0);
    EXPECT_DOUBLE_EQ(result.calculateSharpeRatio(0.02), 0.0);
    EXPECT_DOUBLE_EQ(result.calculateSharpeRatio(0.05), 0.0);

    result.setRiskFreeRate(0.03);
    EXPECT_DOUBLE_EQ(result.sharpeRatio, 0.0);
}
