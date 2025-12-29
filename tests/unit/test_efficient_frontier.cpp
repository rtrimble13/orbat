#include "orbat/optimizer/efficient_frontier.hpp"

#include <cmath>
#include <fstream>
#include <sstream>

#include <gtest/gtest.h>

using orbat::core::Matrix;
using orbat::core::Vector;
using orbat::optimizer::CovarianceMatrix;
using orbat::optimizer::ExpectedReturns;
using orbat::optimizer::exportFrontierToCSV;
using orbat::optimizer::exportFrontierToJSON;
using orbat::optimizer::frontierToJSONString;
using orbat::optimizer::MarkowitzOptimizer;

// Helper to read file contents
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

// Helper to count lines in file
size_t countLines(const std::string& filename) {
    std::ifstream file(filename);
    size_t count = 0;
    std::string line;
    while (std::getline(file, line)) {
        count++;
    }
    return count;
}

// Test CSV export
TEST(EfficientFrontierExportTest, CSVExportBasic) {
    ExpectedReturns returns({0.08, 0.12, 0.16});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto frontier = optimizer.efficientFrontier(10);

    ASSERT_FALSE(frontier.empty());

    std::string filename = "/tmp/test_frontier.csv";
    EXPECT_NO_THROW(exportFrontierToCSV(frontier, filename));

    // Check file exists and has content
    std::ifstream file(filename);
    ASSERT_TRUE(file.is_open());

    // Read header
    std::string header;
    std::getline(file, header);
    EXPECT_TRUE(header.find("return") != std::string::npos);
    EXPECT_TRUE(header.find("volatility") != std::string::npos);
    EXPECT_TRUE(header.find("weight_") != std::string::npos);

    // Check data rows exist
    size_t lineCount = countLines(filename);
    EXPECT_EQ(lineCount, frontier.size() + 1);  // header + data rows

    file.close();
    std::remove(filename.c_str());
}

TEST(EfficientFrontierExportTest, CSVExportWithLabels) {
    ExpectedReturns returns({0.08, 0.12, 0.16});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto frontier = optimizer.efficientFrontier(5);

    std::vector<std::string> labels = {"Bonds", "Stocks", "Real Estate"};
    std::string filename = "/tmp/test_frontier_labels.csv";
    EXPECT_NO_THROW(exportFrontierToCSV(frontier, filename, labels));

    std::string content = readFile(filename);
    EXPECT_TRUE(content.find("Bonds") != std::string::npos);
    EXPECT_TRUE(content.find("Stocks") != std::string::npos);
    EXPECT_TRUE(content.find("Real Estate") != std::string::npos);

    std::remove(filename.c_str());
}

TEST(EfficientFrontierExportTest, CSVExportEmptyFrontier) {
    std::vector<orbat::optimizer::MarkowitzResult> emptyFrontier;
    std::string filename = "/tmp/test_empty.csv";

    EXPECT_THROW(exportFrontierToCSV(emptyFrontier, filename), std::invalid_argument);
}

TEST(EfficientFrontierExportTest, CSVExportInvalidPath) {
    ExpectedReturns returns({0.08, 0.12});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto frontier = optimizer.efficientFrontier(5);

    EXPECT_THROW(exportFrontierToCSV(frontier, "/invalid/path/file.csv"), std::runtime_error);
}

// Test JSON export
TEST(EfficientFrontierExportTest, JSONExportBasic) {
    ExpectedReturns returns({0.08, 0.12, 0.16});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto frontier = optimizer.efficientFrontier(10);

    ASSERT_FALSE(frontier.empty());

    std::string filename = "/tmp/test_frontier.json";
    EXPECT_NO_THROW(exportFrontierToJSON(frontier, filename));

    // Check file exists and has valid JSON structure
    std::string content = readFile(filename);
    EXPECT_TRUE(content.find("\"frontier\"") != std::string::npos);
    EXPECT_TRUE(content.find("\"return\"") != std::string::npos);
    EXPECT_TRUE(content.find("\"volatility\"") != std::string::npos);
    EXPECT_TRUE(content.find("\"weights\"") != std::string::npos);
    EXPECT_TRUE(content.find("{") != std::string::npos);
    EXPECT_TRUE(content.find("}") != std::string::npos);
    EXPECT_TRUE(content.find("[") != std::string::npos);
    EXPECT_TRUE(content.find("]") != std::string::npos);

    std::remove(filename.c_str());
}

TEST(EfficientFrontierExportTest, JSONExportWithLabels) {
    ExpectedReturns returns({0.08, 0.12, 0.16});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto frontier = optimizer.efficientFrontier(5);

    std::vector<std::string> labels = {"Bonds", "Stocks", "Real Estate"};
    std::string filename = "/tmp/test_frontier_labels.json";
    EXPECT_NO_THROW(exportFrontierToJSON(frontier, filename, labels));

    std::string content = readFile(filename);
    EXPECT_TRUE(content.find("\"assets\"") != std::string::npos);
    EXPECT_TRUE(content.find("\"Bonds\"") != std::string::npos);
    EXPECT_TRUE(content.find("\"Stocks\"") != std::string::npos);
    EXPECT_TRUE(content.find("\"Real Estate\"") != std::string::npos);

    std::remove(filename.c_str());
}

TEST(EfficientFrontierExportTest, JSONExportEmptyFrontier) {
    std::vector<orbat::optimizer::MarkowitzResult> emptyFrontier;
    std::string filename = "/tmp/test_empty.json";

    EXPECT_THROW(exportFrontierToJSON(emptyFrontier, filename), std::invalid_argument);
}

TEST(EfficientFrontierExportTest, JSONExportInvalidPath) {
    ExpectedReturns returns({0.08, 0.12});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto frontier = optimizer.efficientFrontier(5);

    EXPECT_THROW(exportFrontierToJSON(frontier, "/invalid/path/file.json"), std::runtime_error);
}

TEST(EfficientFrontierExportTest, JSONStringConversion) {
    ExpectedReturns returns({0.08, 0.12});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto frontier = optimizer.efficientFrontier(5);

    std::string jsonStr;
    EXPECT_NO_THROW(jsonStr = frontierToJSONString(frontier));

    EXPECT_TRUE(jsonStr.find("\"frontier\"") != std::string::npos);
    EXPECT_TRUE(jsonStr.find("\"return\"") != std::string::npos);
    EXPECT_TRUE(jsonStr.find("\"volatility\"") != std::string::npos);
}

TEST(EfficientFrontierExportTest, JSONStringWithLabels) {
    ExpectedReturns returns({0.08, 0.12});
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto frontier = optimizer.efficientFrontier(5);

    std::vector<std::string> labels = {"Asset A", "Asset B"};
    std::string jsonStr = frontierToJSONString(frontier, labels);

    EXPECT_TRUE(jsonStr.find("\"assets\"") != std::string::npos);
    EXPECT_TRUE(jsonStr.find("\"Asset A\"") != std::string::npos);
    EXPECT_TRUE(jsonStr.find("\"Asset B\"") != std::string::npos);
}

// Test frontier properties
TEST(EfficientFrontierTest, FrontierStrictlyIncreasingReturn) {
    // Test that returns are strictly increasing (or equal for numerical precision)
    ExpectedReturns returns({0.08, 0.12, 0.16});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto frontier = optimizer.efficientFrontier(50);

    ASSERT_GE(frontier.size(), 2);

    for (size_t i = 1; i < frontier.size(); ++i) {
        // Returns should be non-decreasing (allow small tolerance for numerical issues)
        EXPECT_GE(frontier[i].expectedReturn, frontier[i - 1].expectedReturn - 1e-6)
            << "Return not increasing at index " << i;
    }

    // First and last should be strictly different
    EXPECT_GT(frontier.back().expectedReturn, frontier.front().expectedReturn);
}

TEST(EfficientFrontierTest, FrontierConvexInVariance) {
    // Test that frontier is convex in variance
    // For a convex frontier: the middle variance should be <= linear interpolation
    ExpectedReturns returns({0.08, 0.12, 0.16});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto frontier = optimizer.efficientFrontier(50);

    ASSERT_GE(frontier.size(), 3);

    // Check convexity by testing that variance grows at a decreasing or constant rate
    // (second derivative <= 0)
    for (size_t i = 1; i < frontier.size() - 1; ++i) {
        double var_prev = frontier[i - 1].risk * frontier[i - 1].risk;
        double var_curr = frontier[i].risk * frontier[i].risk;
        double var_next = frontier[i + 1].risk * frontier[i + 1].risk;

        // Check that the curvature is convex (second derivative of variance <= 0)
        // This means: var_curr <= (var_prev + var_next) / 2 for evenly spaced points
        // More generally, check that variance doesn't increase faster than linearly

        double return_prev = frontier[i - 1].expectedReturn;
        double return_curr = frontier[i].expectedReturn;
        double return_next = frontier[i + 1].expectedReturn;

        // Skip if returns are too close (numerical precision issue)
        if (std::abs(return_next - return_prev) < 1e-8) {
            continue;
        }

        // Linear interpolation weight
        double t = (return_curr - return_prev) / (return_next - return_prev);

        // Linearly interpolated variance
        double var_linear = var_prev + t * (var_next - var_prev);

        // For a convex frontier, actual variance should be <= linear interpolation
        // Allow small tolerance for numerical errors
        EXPECT_LE(var_curr, var_linear + 1e-4)
            << "Frontier not convex at index " << i << " (variance: " << var_curr
            << " vs linear: " << var_linear << ")";
    }
}

TEST(EfficientFrontierTest, FrontierStableAcrossDifferentInputs) {
    // Test that frontier is stable across different but similar covariance matrices
    ExpectedReturns returns({0.08, 0.12, 0.16});

    // Original covariance
    CovarianceMatrix cov1({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    // Slightly perturbed covariance (1% change)
    CovarianceMatrix cov2(
        {{0.0404, 0.0101, 0.00505}, {0.0101, 0.0227, 0.00808}, {0.00505, 0.00808, 0.0101}});

    MarkowitzOptimizer optimizer1(returns, cov1);
    MarkowitzOptimizer optimizer2(returns, cov2);

    auto frontier1 = optimizer1.efficientFrontier(20);
    auto frontier2 = optimizer2.efficientFrontier(20);

    ASSERT_EQ(frontier1.size(), frontier2.size());

    // Check that portfolios are similar (within reasonable tolerance)
    for (size_t i = 0; i < frontier1.size(); ++i) {
        // Returns should be close (allowing for numerical differences from perturbation)
        // The 1% perturbation can cause small differences in optimal target returns
        EXPECT_NEAR(frontier1[i].expectedReturn, frontier2[i].expectedReturn, 5e-6);

        // Risk should be close (within 5% relative error)
        double rel_error = std::abs(frontier1[i].risk - frontier2[i].risk) / frontier1[i].risk;
        EXPECT_LT(rel_error, 0.05) << "Risk unstable at index " << i;
    }
}

TEST(EfficientFrontierTest, FrontierConsistentWithDifferentNumPoints) {
    // Test that generating frontiers with different numbers of points
    // produces consistent results for overlapping points
    ExpectedReturns returns({0.08, 0.12, 0.16});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);

    auto frontier10 = optimizer.efficientFrontier(10);
    auto frontier50 = optimizer.efficientFrontier(50);

    ASSERT_EQ(frontier10.size(), 10);
    ASSERT_EQ(frontier50.size(), 50);

    // Check that endpoints match (first and last portfolios)
    EXPECT_NEAR(frontier10.front().expectedReturn, frontier50.front().expectedReturn, 1e-6);
    EXPECT_NEAR(frontier10.front().risk, frontier50.front().risk, 1e-6);

    EXPECT_NEAR(frontier10.back().expectedReturn, frontier50.back().expectedReturn, 1e-6);
    EXPECT_NEAR(frontier10.back().risk, frontier50.back().risk, 1e-6);
}

TEST(EfficientFrontierTest, LargeNumberOfPoints) {
    // Test generating a large frontier (100 points)
    ExpectedReturns returns({0.08, 0.12, 0.16});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto frontier = optimizer.efficientFrontier(100);

    EXPECT_EQ(frontier.size(), 100);

    // All portfolios should be successful
    for (const auto& result : frontier) {
        EXPECT_TRUE(result.success());
        EXPECT_GT(result.risk, 0.0);
        EXPECT_NEAR(result.weights.sum(), 1.0, 1e-6);
    }
}

TEST(EfficientFrontierTest, FrontierWithConstraints) {
    // Test frontier generation with long-only constraint
    ExpectedReturns returns({0.08, 0.12, 0.16});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    orbat::optimizer::ConstraintSet constraints;
    constraints.add(std::make_shared<orbat::optimizer::LongOnlyConstraint>());

    MarkowitzOptimizer optimizer(returns, cov, constraints);
    auto frontier = optimizer.efficientFrontier(20);

    ASSERT_FALSE(frontier.empty());

    // All weights should be non-negative
    for (const auto& result : frontier) {
        if (!result.success()) {
            continue;
        }
        for (size_t i = 0; i < result.weights.size(); ++i) {
            EXPECT_GE(result.weights[i], -1e-6)
                << "Weight " << i << " is negative: " << result.weights[i];
        }
    }
}

TEST(EfficientFrontierTest, ExportedDataMatchesFrontier) {
    // Test that exported data matches the frontier data
    ExpectedReturns returns({0.08, 0.12, 0.16});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    MarkowitzOptimizer optimizer(returns, cov);
    auto frontier = optimizer.efficientFrontier(5);

    // Export to CSV
    std::string csvFilename = "/tmp/test_export_match.csv";
    exportFrontierToCSV(frontier, csvFilename);

    // Read and verify CSV content
    std::ifstream file(csvFilename);
    std::string header;
    std::getline(file, header);  // Skip header

    for (const auto& result : frontier) {
        std::string line;
        std::getline(file, line);

        std::istringstream iss(line);
        double returnVal, riskVal;
        char comma;

        iss >> returnVal >> comma >> riskVal;

        EXPECT_NEAR(returnVal, result.expectedReturn, 1e-6);
        EXPECT_NEAR(riskVal, result.risk, 1e-6);
    }

    file.close();
    std::remove(csvFilename.c_str());
}
