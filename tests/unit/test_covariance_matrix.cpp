#include "orbat/optimizer/covariance_matrix.hpp"

#include <cmath>
#include <limits>

#include <gtest/gtest.h>

using orbat::core::Matrix;
using orbat::optimizer::CovarianceMatrix;

// Test construction
TEST(CovarianceMatrixTest, DefaultConstructor) {
    CovarianceMatrix cov;
    EXPECT_TRUE(cov.empty());
    EXPECT_EQ(cov.size(), 0);
}

TEST(CovarianceMatrixTest, MatrixConstructor) {
    Matrix m({{0.04, 0.01}, {0.01, 0.0225}});
    CovarianceMatrix cov(m);

    EXPECT_EQ(cov.size(), 2);
    EXPECT_DOUBLE_EQ(cov(0, 0), 0.04);
    EXPECT_DOUBLE_EQ(cov(0, 1), 0.01);
    EXPECT_DOUBLE_EQ(cov(1, 0), 0.01);
    EXPECT_DOUBLE_EQ(cov(1, 1), 0.0225);
}

TEST(CovarianceMatrixTest, InitializerListConstructor) {
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    EXPECT_EQ(cov.size(), 3);
    EXPECT_DOUBLE_EQ(cov(0, 0), 0.04);
    EXPECT_DOUBLE_EQ(cov(1, 1), 0.0225);
    EXPECT_DOUBLE_EQ(cov(2, 2), 0.01);
}

// Test validation - square matrix
TEST(CovarianceMatrixTest, ValidateSquare) {
    Matrix m(2, 3);  // Not square
    EXPECT_THROW({ CovarianceMatrix cov(m); }, std::invalid_argument);
}

// Test validation - symmetry
TEST(CovarianceMatrixTest, ValidateSymmetry) {
    Matrix m({{0.04, 0.01}, {0.02, 0.0225}});  // Not symmetric
    EXPECT_THROW({ CovarianceMatrix cov(m); }, std::invalid_argument);
}

TEST(CovarianceMatrixTest, ValidateSymmetryWithinTolerance) {
    // Slightly asymmetric but within numerical tolerance
    Matrix m({{0.04, 0.01}, {0.01 + 1e-16, 0.0225}});
    EXPECT_NO_THROW({ CovarianceMatrix cov(m); });
}

// Test validation - positive diagonal
TEST(CovarianceMatrixTest, ValidatePositiveDiagonal) {
    Matrix m({{0.04, 0.01}, {0.01, 0.0}});  // Zero diagonal
    EXPECT_THROW({ CovarianceMatrix cov(m); }, std::invalid_argument);
}

TEST(CovarianceMatrixTest, ValidateNegativeDiagonal) {
    Matrix m({{0.04, 0.01}, {0.01, -0.0225}});  // Negative diagonal
    EXPECT_THROW({ CovarianceMatrix cov(m); }, std::invalid_argument);
}

// Test validation - finite values
TEST(CovarianceMatrixTest, ValidateFiniteValues) {
    Matrix m({{0.04, std::numeric_limits<double>::quiet_NaN()},
              {std::numeric_limits<double>::quiet_NaN(), 0.0225}});
    EXPECT_THROW({ CovarianceMatrix cov(m); }, std::invalid_argument);
}

TEST(CovarianceMatrixTest, ValidateInfinity) {
    Matrix m({{0.04, 0.01}, {0.01, std::numeric_limits<double>::infinity()}});
    EXPECT_THROW({ CovarianceMatrix cov(m); }, std::invalid_argument);
}

// Test CSV loading
TEST(CovarianceMatrixTest, LoadFromCSV) {
    CovarianceMatrix cov = CovarianceMatrix::fromCSV("data/covariance.csv");

    EXPECT_EQ(cov.size(), 3);
    EXPECT_DOUBLE_EQ(cov(0, 0), 0.04);
    EXPECT_DOUBLE_EQ(cov(0, 1), 0.01);
    EXPECT_DOUBLE_EQ(cov(0, 2), 0.005);
    EXPECT_DOUBLE_EQ(cov(1, 0), 0.01);
    EXPECT_DOUBLE_EQ(cov(1, 1), 0.0225);
    EXPECT_DOUBLE_EQ(cov(1, 2), 0.008);
    EXPECT_DOUBLE_EQ(cov(2, 0), 0.005);
    EXPECT_DOUBLE_EQ(cov(2, 1), 0.008);
    EXPECT_DOUBLE_EQ(cov(2, 2), 0.01);
}

TEST(CovarianceMatrixTest, LoadFromCSVFileNotFound) {
    EXPECT_THROW(CovarianceMatrix::fromCSV("nonexistent_file.csv"), std::runtime_error);
}

TEST(CovarianceMatrixTest, LoadFromCSVNonSquare) {
    EXPECT_THROW(CovarianceMatrix::fromCSV("data/invalid_nonsquare_cov.csv"),
                 std::invalid_argument);
}

TEST(CovarianceMatrixTest, LoadFromCSVAsymmetric) {
    EXPECT_THROW(CovarianceMatrix::fromCSV("data/invalid_asymmetric_cov.csv"),
                 std::invalid_argument);
}

TEST(CovarianceMatrixTest, LoadFromCSVWithLabels) {
    CovarianceMatrix cov = CovarianceMatrix::fromCSV("data/covariance_with_labels.csv");

    EXPECT_EQ(cov.size(), 3);
    EXPECT_DOUBLE_EQ(cov(0, 0), 0.04);
    EXPECT_DOUBLE_EQ(cov(0, 1), 0.01);
    EXPECT_DOUBLE_EQ(cov(0, 2), 0.005);
    EXPECT_DOUBLE_EQ(cov(1, 0), 0.01);
    EXPECT_DOUBLE_EQ(cov(1, 1), 0.0225);
    EXPECT_DOUBLE_EQ(cov(1, 2), 0.008);
    EXPECT_DOUBLE_EQ(cov(2, 0), 0.005);
    EXPECT_DOUBLE_EQ(cov(2, 1), 0.008);
    EXPECT_DOUBLE_EQ(cov(2, 2), 0.01);

    EXPECT_EQ(cov.labels().size(), 3);
    EXPECT_EQ(cov.labels()[0], "Stock A");
    EXPECT_EQ(cov.labels()[1], "Stock B");
    EXPECT_EQ(cov.labels()[2], "Stock C");
}

// Test JSON loading
TEST(CovarianceMatrixTest, LoadFromJSON) {
    CovarianceMatrix cov = CovarianceMatrix::fromJSON("data/covariance.json");

    EXPECT_EQ(cov.size(), 3);
    EXPECT_DOUBLE_EQ(cov(0, 0), 0.04);
    EXPECT_DOUBLE_EQ(cov(0, 1), 0.01);
    EXPECT_DOUBLE_EQ(cov(0, 2), 0.005);
    EXPECT_DOUBLE_EQ(cov(1, 0), 0.01);
    EXPECT_DOUBLE_EQ(cov(1, 1), 0.0225);
    EXPECT_DOUBLE_EQ(cov(1, 2), 0.008);
    EXPECT_DOUBLE_EQ(cov(2, 0), 0.005);
    EXPECT_DOUBLE_EQ(cov(2, 1), 0.008);
    EXPECT_DOUBLE_EQ(cov(2, 2), 0.01);
}

TEST(CovarianceMatrixTest, LoadFromJSONFileNotFound) {
    EXPECT_THROW(CovarianceMatrix::fromJSON("nonexistent_file.json"), std::runtime_error);
}

TEST(CovarianceMatrixTest, LoadFromJSONStringValid) {
    std::string json = "[[0.04, 0.01], [0.01, 0.0225]]";
    CovarianceMatrix cov = CovarianceMatrix::fromJSONString(json);

    EXPECT_EQ(cov.size(), 2);
    EXPECT_DOUBLE_EQ(cov(0, 0), 0.04);
    EXPECT_DOUBLE_EQ(cov(0, 1), 0.01);
    EXPECT_DOUBLE_EQ(cov(1, 0), 0.01);
    EXPECT_DOUBLE_EQ(cov(1, 1), 0.0225);
}

TEST(CovarianceMatrixTest, LoadFromJSONStringWithWhitespace) {
    std::string json = "  [  [  0.04  ,  0.01  ]  ,  [  0.01  ,  0.0225  ]  ]  ";
    CovarianceMatrix cov = CovarianceMatrix::fromJSONString(json);

    EXPECT_EQ(cov.size(), 2);
    EXPECT_DOUBLE_EQ(cov(0, 0), 0.04);
    EXPECT_DOUBLE_EQ(cov(0, 1), 0.01);
}

TEST(CovarianceMatrixTest, LoadFromJSONStringInvalidFormat) {
    std::string json = "not a matrix";
    EXPECT_THROW(CovarianceMatrix::fromJSONString(json), std::runtime_error);
}

TEST(CovarianceMatrixTest, LoadFromJSONStringUnclosed) {
    std::string json = "[[0.04, 0.01";  // No closing brackets at all
    EXPECT_THROW(CovarianceMatrix::fromJSONString(json), std::runtime_error);
}

TEST(CovarianceMatrixTest, LoadFromJSONStringNonSquare) {
    std::string json = "[[0.04, 0.01], [0.01, 0.0225, 0.01]]";
    EXPECT_THROW(CovarianceMatrix::fromJSONString(json), std::invalid_argument);
}

TEST(CovarianceMatrixTest, LoadFromJSONStringEmpty) {
    std::string json = "[]";
    EXPECT_THROW(CovarianceMatrix::fromJSONString(json), std::runtime_error);
}

// Test dimension matching
TEST(CovarianceMatrixTest, DimensionsMatch) {
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    EXPECT_TRUE(cov.dimensionsMatch(3));
    EXPECT_FALSE(cov.dimensionsMatch(2));
    EXPECT_FALSE(cov.dimensionsMatch(4));
}

// Test data access
TEST(CovarianceMatrixTest, DataAccess) {
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});

    const Matrix& data = cov.data();
    EXPECT_EQ(data.rows(), 2);
    EXPECT_EQ(data.cols(), 2);
    EXPECT_DOUBLE_EQ(data(0, 0), 0.04);
    EXPECT_DOUBLE_EQ(data(0, 1), 0.01);
}

TEST(CovarianceMatrixTest, DataMutation) {
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});

    cov(0, 1) = 0.015;
    EXPECT_DOUBLE_EQ(cov(0, 1), 0.015);
}

// Test with realistic portfolio data
TEST(CovarianceMatrixTest, RealisticPortfolio) {
    // 3 assets with realistic covariance matrix
    // Asset 1: 20% std dev
    // Asset 2: 15% std dev
    // Asset 3: 10% std dev
    // with various correlations
    CovarianceMatrix cov({{0.04, 0.01, 0.005},     // Asset 1
                          {0.01, 0.0225, 0.008},   // Asset 2
                          {0.005, 0.008, 0.01}});  // Asset 3

    EXPECT_EQ(cov.size(), 3);
    EXPECT_FALSE(cov.empty());

    // Verify diagonal elements (variances) are positive
    for (size_t i = 0; i < cov.size(); ++i) {
        EXPECT_GT(cov(i, i), 0.0);
    }

    // Verify symmetry
    for (size_t i = 0; i < cov.size(); ++i) {
        for (size_t j = i + 1; j < cov.size(); ++j) {
            EXPECT_DOUBLE_EQ(cov(i, j), cov(j, i));
        }
    }
}

// Test edge cases
TEST(CovarianceMatrixTest, SingleAsset) {
    CovarianceMatrix cov({{0.04}});

    EXPECT_EQ(cov.size(), 1);
    EXPECT_DOUBLE_EQ(cov(0, 0), 0.04);
}

TEST(CovarianceMatrixTest, IdentityCovarianceMatrix) {
    // All assets have variance 1, no correlation
    CovarianceMatrix cov({{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}});

    EXPECT_EQ(cov.size(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_DOUBLE_EQ(cov(i, i), 1.0);
        for (size_t j = 0; j < 3; ++j) {
            if (i != j) {
                EXPECT_DOUBLE_EQ(cov(i, j), 0.0);
            }
        }
    }
}

TEST(CovarianceMatrixTest, HighCorrelation) {
    // Two assets with high positive correlation
    double var1 = 0.04;
    double var2 = 0.0225;
    double corr = 0.9;
    double cov12 = corr * std::sqrt(var1 * var2);

    CovarianceMatrix cov({{var1, cov12}, {cov12, var2}});

    EXPECT_EQ(cov.size(), 2);
    EXPECT_DOUBLE_EQ(cov(0, 0), var1);
    EXPECT_DOUBLE_EQ(cov(1, 1), var2);
    EXPECT_NEAR(cov(0, 1), cov12, 1e-10);
    EXPECT_NEAR(cov(1, 0), cov12, 1e-10);
}

TEST(CovarianceMatrixTest, NegativeCorrelation) {
    // Two assets with negative correlation
    double var1 = 0.04;
    double var2 = 0.0225;
    double corr = -0.5;
    double cov12 = corr * std::sqrt(var1 * var2);

    CovarianceMatrix cov({{var1, cov12}, {cov12, var2}});

    EXPECT_EQ(cov.size(), 2);
    EXPECT_DOUBLE_EQ(cov(0, 0), var1);
    EXPECT_DOUBLE_EQ(cov(1, 1), var2);
    EXPECT_NEAR(cov(0, 1), cov12, 1e-10);
    EXPECT_NEAR(cov(1, 0), cov12, 1e-10);
}

// Test asset labels
TEST(CovarianceMatrixTest, ConstructWithLabels) {
    Matrix m({{0.04, 0.01}, {0.01, 0.0225}});
    std::vector<std::string> labels = {"Stock A", "Stock B"};
    CovarianceMatrix cov(m, labels);

    EXPECT_EQ(cov.size(), 2);
    EXPECT_EQ(cov.labels().size(), 2);
    EXPECT_EQ(cov.labels()[0], "Stock A");
    EXPECT_EQ(cov.labels()[1], "Stock B");
}

TEST(CovarianceMatrixTest, LabelsAccessors) {
    CovarianceMatrix cov({{0.04, 0.01}, {0.01, 0.0225}});

    EXPECT_TRUE(cov.labels().empty());
    EXPECT_FALSE(cov.hasLabel(0));
    EXPECT_EQ(cov.getLabel(0), "Asset 0");

    std::vector<std::string> labels = {"Stock A", "Stock B"};
    cov.setLabels(labels);

    EXPECT_TRUE(cov.hasLabel(0));
    EXPECT_EQ(cov.getLabel(0), "Stock A");
}

TEST(CovarianceMatrixTest, LabelsSizeMismatch) {
    Matrix m({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});
    std::vector<std::string> labels = {"Stock A", "Stock B"};

    EXPECT_THROW({ CovarianceMatrix cov(m, labels); }, std::invalid_argument);
}

TEST(CovarianceMatrixTest, LoadFromJSONObject) {
    CovarianceMatrix cov = CovarianceMatrix::fromJSON("data/covariance_object.json");
    EXPECT_EQ(cov.size(), 3);
    EXPECT_DOUBLE_EQ(cov(0, 0), 0.04);
}

TEST(CovarianceMatrixTest, LoadFromJSONWithLabels) {
    CovarianceMatrix cov = CovarianceMatrix::fromJSON("data/covariance_with_labels.json");
    EXPECT_EQ(cov.size(), 3);
    EXPECT_EQ(cov.labels().size(), 3);
    EXPECT_EQ(cov.labels()[0], "Stock A");
}

TEST(CovarianceMatrixTest, LoadFromJSONStringObject) {
    std::string json = R"({"covariance": [[0.04, 0.01], [0.01, 0.0225]]})";
    CovarianceMatrix cov = CovarianceMatrix::fromJSONString(json);
    EXPECT_EQ(cov.size(), 2);
    EXPECT_DOUBLE_EQ(cov(0, 0), 0.04);
}

TEST(CovarianceMatrixTest, LoadFromJSONStringWithLabels) {
    std::string json =
        R"({"covariance": [[0.04, 0.01], [0.01, 0.0225]], "labels": ["Stock A", "Stock B"]})";
    CovarianceMatrix cov = CovarianceMatrix::fromJSONString(json);
    EXPECT_EQ(cov.size(), 2);
    EXPECT_EQ(cov.labels().size(), 2);
    EXPECT_EQ(cov.labels()[0], "Stock A");
}
