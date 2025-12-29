#include "orbat/core/vector.hpp"

#include <cmath>

#include <gtest/gtest.h>

using orbat::core::Vector;

// Test construction
TEST(VectorTest, DefaultConstructor) {
    Vector v;
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0);
}

TEST(VectorTest, SizeConstructor) {
    Vector v(5);
    EXPECT_EQ(v.size(), 5);
    for (size_t i = 0; i < v.size(); ++i) {
        EXPECT_DOUBLE_EQ(v[i], 0.0);
    }
}

TEST(VectorTest, SizeValueConstructor) {
    Vector v(3, 2.5);
    EXPECT_EQ(v.size(), 3);
    for (size_t i = 0; i < v.size(); ++i) {
        EXPECT_DOUBLE_EQ(v[i], 2.5);
    }
}

TEST(VectorTest, InitializerListConstructor) {
    Vector v({1.0, 2.0, 3.0});
    EXPECT_EQ(v.size(), 3);
    EXPECT_DOUBLE_EQ(v[0], 1.0);
    EXPECT_DOUBLE_EQ(v[1], 2.0);
    EXPECT_DOUBLE_EQ(v[2], 3.0);
}

TEST(VectorTest, StdVectorConstructor) {
    std::vector<double> data = {4.0, 5.0, 6.0};
    Vector v(data);
    EXPECT_EQ(v.size(), 3);
    EXPECT_DOUBLE_EQ(v[0], 4.0);
    EXPECT_DOUBLE_EQ(v[1], 5.0);
    EXPECT_DOUBLE_EQ(v[2], 6.0);
}

// Test access
TEST(VectorTest, BracketAccess) {
    Vector v({1.0, 2.0, 3.0});
    EXPECT_DOUBLE_EQ(v[0], 1.0);
    EXPECT_DOUBLE_EQ(v[1], 2.0);
    EXPECT_DOUBLE_EQ(v[2], 3.0);

    v[1] = 5.0;
    EXPECT_DOUBLE_EQ(v[1], 5.0);
}

TEST(VectorTest, AtAccess) {
    Vector v({1.0, 2.0, 3.0});
    EXPECT_DOUBLE_EQ(v.at(0), 1.0);
    EXPECT_DOUBLE_EQ(v.at(1), 2.0);
    EXPECT_DOUBLE_EQ(v.at(2), 3.0);

    EXPECT_THROW(v.at(3), std::out_of_range);
}

// Test dot product
TEST(VectorTest, DotProduct) {
    Vector v1({1.0, 2.0, 3.0});
    Vector v2({4.0, 5.0, 6.0});

    // 1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32
    EXPECT_DOUBLE_EQ(v1.dot(v2), 32.0);
}

TEST(VectorTest, DotProductSelf) {
    Vector v({2.0, 3.0, 4.0});
    // 2*2 + 3*3 + 4*4 = 4 + 9 + 16 = 29
    EXPECT_DOUBLE_EQ(v.dot(v), 29.0);
}

TEST(VectorTest, DotProductSizeMismatch) {
    Vector v1({1.0, 2.0, 3.0});
    Vector v2({4.0, 5.0});

    EXPECT_THROW(v1.dot(v2), std::invalid_argument);
}

// Test norm
TEST(VectorTest, Norm) {
    Vector v({3.0, 4.0});
    // sqrt(3^2 + 4^2) = sqrt(9 + 16) = sqrt(25) = 5.0
    EXPECT_DOUBLE_EQ(v.norm(), 5.0);
}

TEST(VectorTest, NormUnitVector) {
    Vector v({1.0, 0.0, 0.0});
    EXPECT_DOUBLE_EQ(v.norm(), 1.0);
}

// Test sum
TEST(VectorTest, Sum) {
    Vector v({1.0, 2.0, 3.0, 4.0});
    EXPECT_DOUBLE_EQ(v.sum(), 10.0);
}

// Test addition
TEST(VectorTest, Addition) {
    Vector v1({1.0, 2.0, 3.0});
    Vector v2({4.0, 5.0, 6.0});
    Vector result = v1 + v2;

    EXPECT_EQ(result.size(), 3);
    EXPECT_DOUBLE_EQ(result[0], 5.0);
    EXPECT_DOUBLE_EQ(result[1], 7.0);
    EXPECT_DOUBLE_EQ(result[2], 9.0);
}

TEST(VectorTest, AdditionSizeMismatch) {
    Vector v1({1.0, 2.0, 3.0});
    Vector v2({4.0, 5.0});

    EXPECT_THROW(v1 + v2, std::invalid_argument);
}

// Test subtraction
TEST(VectorTest, Subtraction) {
    Vector v1({5.0, 7.0, 9.0});
    Vector v2({1.0, 2.0, 3.0});
    Vector result = v1 - v2;

    EXPECT_EQ(result.size(), 3);
    EXPECT_DOUBLE_EQ(result[0], 4.0);
    EXPECT_DOUBLE_EQ(result[1], 5.0);
    EXPECT_DOUBLE_EQ(result[2], 6.0);
}

// Test scalar multiplication
TEST(VectorTest, ScalarMultiplication) {
    Vector v({1.0, 2.0, 3.0});
    Vector result = v * 2.0;

    EXPECT_EQ(result.size(), 3);
    EXPECT_DOUBLE_EQ(result[0], 2.0);
    EXPECT_DOUBLE_EQ(result[1], 4.0);
    EXPECT_DOUBLE_EQ(result[2], 6.0);
}

TEST(VectorTest, ScalarMultiplicationCommutative) {
    Vector v({1.0, 2.0, 3.0});
    Vector result1 = v * 2.0;
    Vector result2 = 2.0 * v;

    EXPECT_EQ(result1.size(), result2.size());
    for (size_t i = 0; i < result1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result1[i], result2[i]);
    }
}

// Test scalar division
TEST(VectorTest, ScalarDivision) {
    Vector v({2.0, 4.0, 6.0});
    Vector result = v / 2.0;

    EXPECT_EQ(result.size(), 3);
    EXPECT_DOUBLE_EQ(result[0], 1.0);
    EXPECT_DOUBLE_EQ(result[1], 2.0);
    EXPECT_DOUBLE_EQ(result[2], 3.0);
}

TEST(VectorTest, ScalarDivisionByZero) {
    Vector v({1.0, 2.0, 3.0});
    EXPECT_THROW(v / 0.0, std::invalid_argument);
}

// Test in-place operations
TEST(VectorTest, InPlaceAddition) {
    Vector v1({1.0, 2.0, 3.0});
    Vector v2({4.0, 5.0, 6.0});
    v1 += v2;

    EXPECT_DOUBLE_EQ(v1[0], 5.0);
    EXPECT_DOUBLE_EQ(v1[1], 7.0);
    EXPECT_DOUBLE_EQ(v1[2], 9.0);
}

TEST(VectorTest, InPlaceSubtraction) {
    Vector v1({5.0, 7.0, 9.0});
    Vector v2({1.0, 2.0, 3.0});
    v1 -= v2;

    EXPECT_DOUBLE_EQ(v1[0], 4.0);
    EXPECT_DOUBLE_EQ(v1[1], 5.0);
    EXPECT_DOUBLE_EQ(v1[2], 6.0);
}

TEST(VectorTest, InPlaceScalarMultiplication) {
    Vector v({1.0, 2.0, 3.0});
    v *= 2.0;

    EXPECT_DOUBLE_EQ(v[0], 2.0);
    EXPECT_DOUBLE_EQ(v[1], 4.0);
    EXPECT_DOUBLE_EQ(v[2], 6.0);
}

TEST(VectorTest, InPlaceScalarDivision) {
    Vector v({2.0, 4.0, 6.0});
    v /= 2.0;

    EXPECT_DOUBLE_EQ(v[0], 1.0);
    EXPECT_DOUBLE_EQ(v[1], 2.0);
    EXPECT_DOUBLE_EQ(v[2], 3.0);
}

// Test resize
TEST(VectorTest, Resize) {
    Vector v({1.0, 2.0, 3.0});
    v.resize(5, 0.0);

    EXPECT_EQ(v.size(), 5);
    EXPECT_DOUBLE_EQ(v[0], 1.0);
    EXPECT_DOUBLE_EQ(v[1], 2.0);
    EXPECT_DOUBLE_EQ(v[2], 3.0);
    EXPECT_DOUBLE_EQ(v[3], 0.0);
    EXPECT_DOUBLE_EQ(v[4], 0.0);
}

// Test numerical stability with small values
TEST(VectorTest, NumericalStabilitySmallValues) {
    Vector v({1e-10, 2e-10, 3e-10});
    double expectedNorm = std::sqrt(1e-20 + 4e-20 + 9e-20);
    EXPECT_NEAR(v.norm(), expectedNorm, 1e-25);
}

// Test numerical stability with large values
TEST(VectorTest, NumericalStabilityLargeValues) {
    Vector v({1e10, 2e10, 3e10});
    double expectedNorm = std::sqrt(1e20 + 4e20 + 9e20);
    EXPECT_NEAR(v.norm(), expectedNorm, 1e5);
}
