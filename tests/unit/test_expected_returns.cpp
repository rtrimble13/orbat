#include "orbat/optimizer/expected_returns.hpp"

#include <cmath>
#include <limits>

#include <gtest/gtest.h>

using orbat::core::Vector;
using orbat::optimizer::ExpectedReturns;

// Test construction
TEST(ExpectedReturnsTest, DefaultConstructor) {
    ExpectedReturns returns;
    EXPECT_TRUE(returns.empty());
    EXPECT_EQ(returns.size(), 0);
}

TEST(ExpectedReturnsTest, VectorConstructor) {
    Vector v({0.08, 0.12, 0.10});
    ExpectedReturns returns(v);

    EXPECT_EQ(returns.size(), 3);
    EXPECT_DOUBLE_EQ(returns[0], 0.08);
    EXPECT_DOUBLE_EQ(returns[1], 0.12);
    EXPECT_DOUBLE_EQ(returns[2], 0.10);
}

TEST(ExpectedReturnsTest, InitializerListConstructor) {
    ExpectedReturns returns({0.08, 0.12, 0.10});

    EXPECT_EQ(returns.size(), 3);
    EXPECT_DOUBLE_EQ(returns[0], 0.08);
    EXPECT_DOUBLE_EQ(returns[1], 0.12);
    EXPECT_DOUBLE_EQ(returns[2], 0.10);
}

TEST(ExpectedReturnsTest, EmptyVectorThrows) {
    Vector v;
    EXPECT_THROW({ ExpectedReturns returns(v); }, std::invalid_argument);
}

// Test validation
TEST(ExpectedReturnsTest, ValidateFiniteValues) {
    Vector v({0.08, std::numeric_limits<double>::quiet_NaN(), 0.10});
    EXPECT_THROW({ ExpectedReturns returns(v); }, std::invalid_argument);
}

TEST(ExpectedReturnsTest, ValidateInfinity) {
    Vector v({0.08, std::numeric_limits<double>::infinity(), 0.10});
    EXPECT_THROW({ ExpectedReturns returns(v); }, std::invalid_argument);
}

// Test CSV loading
TEST(ExpectedReturnsTest, LoadFromCSV) {
    ExpectedReturns returns = ExpectedReturns::fromCSV("data/expected_returns.csv");

    EXPECT_EQ(returns.size(), 3);
    EXPECT_DOUBLE_EQ(returns[0], 0.08);
    EXPECT_DOUBLE_EQ(returns[1], 0.12);
    EXPECT_DOUBLE_EQ(returns[2], 0.10);
}

TEST(ExpectedReturnsTest, LoadFromCSVWithHeader) {
    ExpectedReturns returns = ExpectedReturns::fromCSV("data/expected_returns_with_header.csv");

    EXPECT_EQ(returns.size(), 3);
    EXPECT_DOUBLE_EQ(returns[0], 0.08);
    EXPECT_DOUBLE_EQ(returns[1], 0.12);
    EXPECT_DOUBLE_EQ(returns[2], 0.10);
}

TEST(ExpectedReturnsTest, LoadFromCSVFileNotFound) {
    EXPECT_THROW(ExpectedReturns::fromCSV("nonexistent_file.csv"), std::runtime_error);
}

TEST(ExpectedReturnsTest, LoadFromCSVInvalidData) {
    EXPECT_THROW(ExpectedReturns::fromCSV("data/invalid_returns.csv"), std::runtime_error);
}

// Test JSON loading
TEST(ExpectedReturnsTest, LoadFromJSON) {
    ExpectedReturns returns = ExpectedReturns::fromJSON("data/expected_returns.json");

    EXPECT_EQ(returns.size(), 3);
    EXPECT_DOUBLE_EQ(returns[0], 0.08);
    EXPECT_DOUBLE_EQ(returns[1], 0.12);
    EXPECT_DOUBLE_EQ(returns[2], 0.10);
}

TEST(ExpectedReturnsTest, LoadFromJSONFileNotFound) {
    EXPECT_THROW(ExpectedReturns::fromJSON("nonexistent_file.json"), std::runtime_error);
}

TEST(ExpectedReturnsTest, LoadFromJSONStringValid) {
    std::string json = "[0.08, 0.12, 0.10]";
    ExpectedReturns returns = ExpectedReturns::fromJSONString(json);

    EXPECT_EQ(returns.size(), 3);
    EXPECT_DOUBLE_EQ(returns[0], 0.08);
    EXPECT_DOUBLE_EQ(returns[1], 0.12);
    EXPECT_DOUBLE_EQ(returns[2], 0.10);
}

TEST(ExpectedReturnsTest, LoadFromJSONStringWithWhitespace) {
    std::string json = "  [  0.08  ,  0.12  ,  0.10  ]  ";
    ExpectedReturns returns = ExpectedReturns::fromJSONString(json);

    EXPECT_EQ(returns.size(), 3);
    EXPECT_DOUBLE_EQ(returns[0], 0.08);
    EXPECT_DOUBLE_EQ(returns[1], 0.12);
    EXPECT_DOUBLE_EQ(returns[2], 0.10);
}

TEST(ExpectedReturnsTest, LoadFromJSONStringInvalidFormat) {
    std::string json = "not an array";
    EXPECT_THROW(ExpectedReturns::fromJSONString(json), std::runtime_error);
}

TEST(ExpectedReturnsTest, LoadFromJSONStringUnclosed) {
    std::string json = "[0.08, 0.12, 0.10";
    EXPECT_THROW(ExpectedReturns::fromJSONString(json), std::runtime_error);
}

TEST(ExpectedReturnsTest, LoadFromJSONStringInvalidValue) {
    std::string json = "[0.08, abc, 0.10]";
    EXPECT_THROW(ExpectedReturns::fromJSONString(json), std::runtime_error);
}

TEST(ExpectedReturnsTest, LoadFromJSONStringEmpty) {
    std::string json = "[]";
    EXPECT_THROW(ExpectedReturns::fromJSONString(json), std::runtime_error);
}

// Test data access
TEST(ExpectedReturnsTest, DataAccess) {
    ExpectedReturns returns({0.08, 0.12, 0.10});

    const Vector& data = returns.data();
    EXPECT_EQ(data.size(), 3);
    EXPECT_DOUBLE_EQ(data[0], 0.08);
    EXPECT_DOUBLE_EQ(data[1], 0.12);
    EXPECT_DOUBLE_EQ(data[2], 0.10);
}

TEST(ExpectedReturnsTest, DataMutation) {
    ExpectedReturns returns({0.08, 0.12, 0.10});

    returns[1] = 0.15;
    EXPECT_DOUBLE_EQ(returns[1], 0.15);
}

// Test with realistic portfolio data
TEST(ExpectedReturnsTest, RealisticPortfolio) {
    // 5 assets with various expected returns
    ExpectedReturns returns({0.08, 0.12, 0.10, 0.06, 0.15});

    EXPECT_EQ(returns.size(), 5);
    EXPECT_FALSE(returns.empty());

    // Verify all returns are accessible
    for (size_t i = 0; i < returns.size(); ++i) {
        EXPECT_TRUE(std::isfinite(returns[i]));
    }
}

// Test edge cases
TEST(ExpectedReturnsTest, NegativeReturns) {
    // Negative returns are valid (losses are expected)
    ExpectedReturns returns({-0.05, 0.08, -0.02});

    EXPECT_EQ(returns.size(), 3);
    EXPECT_DOUBLE_EQ(returns[0], -0.05);
    EXPECT_DOUBLE_EQ(returns[1], 0.08);
    EXPECT_DOUBLE_EQ(returns[2], -0.02);
}

TEST(ExpectedReturnsTest, ZeroReturns) {
    // Zero returns are valid
    ExpectedReturns returns({0.0, 0.0, 0.0});

    EXPECT_EQ(returns.size(), 3);
    EXPECT_DOUBLE_EQ(returns[0], 0.0);
    EXPECT_DOUBLE_EQ(returns[1], 0.0);
    EXPECT_DOUBLE_EQ(returns[2], 0.0);
}

TEST(ExpectedReturnsTest, SingleAsset) {
    ExpectedReturns returns({0.08});

    EXPECT_EQ(returns.size(), 1);
    EXPECT_DOUBLE_EQ(returns[0], 0.08);
}
