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

TEST(ExpectedReturnsTest, LoadFromCSVWithLabels) {
    ExpectedReturns returns = ExpectedReturns::fromCSV("data/expected_returns_with_labels.csv");

    EXPECT_EQ(returns.size(), 3);
    EXPECT_DOUBLE_EQ(returns[0], 0.08);
    EXPECT_DOUBLE_EQ(returns[1], 0.12);
    EXPECT_DOUBLE_EQ(returns[2], 0.10);

    EXPECT_EQ(returns.labels().size(), 3);
    EXPECT_EQ(returns.labels()[0], "Stock A");
    EXPECT_EQ(returns.labels()[1], "Stock B");
    EXPECT_EQ(returns.labels()[2], "Stock C");
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

// Test asset labels
TEST(ExpectedReturnsTest, ConstructWithLabels) {
    Vector v({0.08, 0.12, 0.10});
    std::vector<std::string> labels = {"Stock A", "Stock B", "Stock C"};
    ExpectedReturns returns(v, labels);

    EXPECT_EQ(returns.size(), 3);
    EXPECT_EQ(returns.labels().size(), 3);
    EXPECT_EQ(returns.labels()[0], "Stock A");
    EXPECT_EQ(returns.labels()[1], "Stock B");
    EXPECT_EQ(returns.labels()[2], "Stock C");
}

TEST(ExpectedReturnsTest, LabelsAccessors) {
    ExpectedReturns returns({0.08, 0.12, 0.10});

    // Initially no labels
    EXPECT_TRUE(returns.labels().empty());
    EXPECT_FALSE(returns.hasLabel(0));
    EXPECT_EQ(returns.getLabel(0), "Asset 0");
    EXPECT_EQ(returns.getLabel(1), "Asset 1");

    // Set labels
    std::vector<std::string> labels = {"Stock A", "Stock B", "Stock C"};
    returns.setLabels(labels);

    EXPECT_TRUE(returns.hasLabel(0));
    EXPECT_EQ(returns.getLabel(0), "Stock A");
    EXPECT_EQ(returns.getLabel(1), "Stock B");
}

TEST(ExpectedReturnsTest, LabelsSizeMismatch) {
    Vector v({0.08, 0.12, 0.10});
    std::vector<std::string> labels = {"Stock A", "Stock B"};  // Wrong size

    EXPECT_THROW({ ExpectedReturns returns(v, labels); }, std::invalid_argument);
}

TEST(ExpectedReturnsTest, SetLabelsSizeMismatch) {
    ExpectedReturns returns({0.08, 0.12, 0.10});
    std::vector<std::string> labels = {"Stock A", "Stock B"};  // Wrong size

    EXPECT_THROW(returns.setLabels(labels), std::invalid_argument);
}

// Test JSON object format
TEST(ExpectedReturnsTest, LoadFromJSONObject) {
    ExpectedReturns returns = ExpectedReturns::fromJSON("data/expected_returns_object.json");

    EXPECT_EQ(returns.size(), 3);
    EXPECT_DOUBLE_EQ(returns[0], 0.08);
    EXPECT_DOUBLE_EQ(returns[1], 0.12);
    EXPECT_DOUBLE_EQ(returns[2], 0.10);
}

TEST(ExpectedReturnsTest, LoadFromJSONWithLabels) {
    ExpectedReturns returns = ExpectedReturns::fromJSON("data/expected_returns_with_labels.json");

    EXPECT_EQ(returns.size(), 3);
    EXPECT_DOUBLE_EQ(returns[0], 0.08);
    EXPECT_DOUBLE_EQ(returns[1], 0.12);
    EXPECT_DOUBLE_EQ(returns[2], 0.10);

    EXPECT_EQ(returns.labels().size(), 3);
    EXPECT_EQ(returns.labels()[0], "Stock A");
    EXPECT_EQ(returns.labels()[1], "Stock B");
    EXPECT_EQ(returns.labels()[2], "Stock C");
}

TEST(ExpectedReturnsTest, LoadFromJSONStringObject) {
    std::string json = R"({"returns": [0.08, 0.12, 0.10]})";
    ExpectedReturns returns = ExpectedReturns::fromJSONString(json);

    EXPECT_EQ(returns.size(), 3);
    EXPECT_DOUBLE_EQ(returns[0], 0.08);
    EXPECT_DOUBLE_EQ(returns[1], 0.12);
    EXPECT_DOUBLE_EQ(returns[2], 0.10);
}

TEST(ExpectedReturnsTest, LoadFromJSONStringWithLabels) {
    std::string json = R"({"returns": [0.08, 0.12], "labels": ["Stock A", "Stock B"]})";
    ExpectedReturns returns = ExpectedReturns::fromJSONString(json);

    EXPECT_EQ(returns.size(), 2);
    EXPECT_DOUBLE_EQ(returns[0], 0.08);
    EXPECT_DOUBLE_EQ(returns[1], 0.12);

    EXPECT_EQ(returns.labels().size(), 2);
    EXPECT_EQ(returns.labels()[0], "Stock A");
    EXPECT_EQ(returns.labels()[1], "Stock B");
}
