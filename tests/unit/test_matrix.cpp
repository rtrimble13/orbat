#include "orbat/core/matrix.hpp"

#include <cmath>

#include <gtest/gtest.h>

using orbat::core::Matrix;
using orbat::core::Vector;

// Test construction
TEST(MatrixTest, DefaultConstructor) {
    Matrix m;
    EXPECT_TRUE(m.empty());
    EXPECT_EQ(m.rows(), 0);
    EXPECT_EQ(m.cols(), 0);
}

TEST(MatrixTest, SizeConstructor) {
    Matrix m(3, 4);
    EXPECT_EQ(m.rows(), 3);
    EXPECT_EQ(m.cols(), 4);
    EXPECT_EQ(m.size(), 12);

    for (size_t i = 0; i < m.rows(); ++i) {
        for (size_t j = 0; j < m.cols(); ++j) {
            EXPECT_DOUBLE_EQ(m(i, j), 0.0);
        }
    }
}

TEST(MatrixTest, SizeValueConstructor) {
    Matrix m(2, 3, 5.0);
    EXPECT_EQ(m.rows(), 2);
    EXPECT_EQ(m.cols(), 3);

    for (size_t i = 0; i < m.rows(); ++i) {
        for (size_t j = 0; j < m.cols(); ++j) {
            EXPECT_DOUBLE_EQ(m(i, j), 5.0);
        }
    }
}

TEST(MatrixTest, InitializerListConstructor) {
    Matrix m({{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}});

    EXPECT_EQ(m.rows(), 2);
    EXPECT_EQ(m.cols(), 3);
    EXPECT_DOUBLE_EQ(m(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(m(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(m(0, 2), 3.0);
    EXPECT_DOUBLE_EQ(m(1, 0), 4.0);
    EXPECT_DOUBLE_EQ(m(1, 1), 5.0);
    EXPECT_DOUBLE_EQ(m(1, 2), 6.0);
}

TEST(MatrixTest, InitializerListUnequalRows) {
    EXPECT_THROW(Matrix({{1.0, 2.0}, {3.0, 4.0, 5.0}}), std::invalid_argument);
}

// Test access
TEST(MatrixTest, ParenthesisAccess) {
    Matrix m({{1.0, 2.0}, {3.0, 4.0}});

    EXPECT_DOUBLE_EQ(m(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(m(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(m(1, 0), 3.0);
    EXPECT_DOUBLE_EQ(m(1, 1), 4.0);

    m(0, 1) = 7.0;
    EXPECT_DOUBLE_EQ(m(0, 1), 7.0);
}

TEST(MatrixTest, AtAccess) {
    Matrix m({{1.0, 2.0}, {3.0, 4.0}});

    EXPECT_DOUBLE_EQ(m.at(0, 0), 1.0);
    EXPECT_THROW(m.at(2, 0), std::out_of_range);
    EXPECT_THROW(m.at(0, 2), std::out_of_range);
}

// Test properties
TEST(MatrixTest, IsSquare) {
    Matrix m1(3, 3);
    Matrix m2(3, 4);

    EXPECT_TRUE(m1.isSquare());
    EXPECT_FALSE(m2.isSquare());
}

// Test row/column operations
TEST(MatrixTest, GetRow) {
    Matrix m({{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}});

    Vector row0 = m.getRow(0);
    EXPECT_EQ(row0.size(), 3);
    EXPECT_DOUBLE_EQ(row0[0], 1.0);
    EXPECT_DOUBLE_EQ(row0[1], 2.0);
    EXPECT_DOUBLE_EQ(row0[2], 3.0);

    Vector row1 = m.getRow(1);
    EXPECT_DOUBLE_EQ(row1[0], 4.0);
    EXPECT_DOUBLE_EQ(row1[1], 5.0);
    EXPECT_DOUBLE_EQ(row1[2], 6.0);
}

TEST(MatrixTest, GetColumn) {
    Matrix m({{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}});

    Vector col0 = m.getColumn(0);
    EXPECT_EQ(col0.size(), 2);
    EXPECT_DOUBLE_EQ(col0[0], 1.0);
    EXPECT_DOUBLE_EQ(col0[1], 4.0);

    Vector col1 = m.getColumn(1);
    EXPECT_DOUBLE_EQ(col1[0], 2.0);
    EXPECT_DOUBLE_EQ(col1[1], 5.0);
}

TEST(MatrixTest, SetRow) {
    Matrix m(2, 3);
    Vector row({1.0, 2.0, 3.0});
    m.setRow(0, row);

    EXPECT_DOUBLE_EQ(m(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(m(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(m(0, 2), 3.0);
}

TEST(MatrixTest, SetColumn) {
    Matrix m(2, 3);
    Vector col({1.0, 2.0});
    m.setColumn(1, col);

    EXPECT_DOUBLE_EQ(m(0, 1), 1.0);
    EXPECT_DOUBLE_EQ(m(1, 1), 2.0);
}

// Test transpose
TEST(MatrixTest, Transpose) {
    Matrix m({{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}});
    Matrix mT = m.transpose();

    EXPECT_EQ(mT.rows(), 3);
    EXPECT_EQ(mT.cols(), 2);
    EXPECT_DOUBLE_EQ(mT(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(mT(0, 1), 4.0);
    EXPECT_DOUBLE_EQ(mT(1, 0), 2.0);
    EXPECT_DOUBLE_EQ(mT(1, 1), 5.0);
    EXPECT_DOUBLE_EQ(mT(2, 0), 3.0);
    EXPECT_DOUBLE_EQ(mT(2, 1), 6.0);
}

TEST(MatrixTest, TransposeSymmetric) {
    Matrix m({{1.0, 2.0}, {2.0, 3.0}});
    Matrix mT = m.transpose();

    EXPECT_EQ(mT.rows(), 2);
    EXPECT_EQ(mT.cols(), 2);
    EXPECT_DOUBLE_EQ(mT(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(mT(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(mT(1, 0), 2.0);
    EXPECT_DOUBLE_EQ(mT(1, 1), 3.0);
}

// Test matrix multiplication
TEST(MatrixTest, MatrixMultiplication) {
    Matrix A({{1.0, 2.0}, {3.0, 4.0}});
    Matrix B({{5.0, 6.0}, {7.0, 8.0}});
    Matrix C = A * B;

    EXPECT_EQ(C.rows(), 2);
    EXPECT_EQ(C.cols(), 2);
    // C(0,0) = 1*5 + 2*7 = 5 + 14 = 19
    EXPECT_DOUBLE_EQ(C(0, 0), 19.0);
    // C(0,1) = 1*6 + 2*8 = 6 + 16 = 22
    EXPECT_DOUBLE_EQ(C(0, 1), 22.0);
    // C(1,0) = 3*5 + 4*7 = 15 + 28 = 43
    EXPECT_DOUBLE_EQ(C(1, 0), 43.0);
    // C(1,1) = 3*6 + 4*8 = 18 + 32 = 50
    EXPECT_DOUBLE_EQ(C(1, 1), 50.0);
}

TEST(MatrixTest, MatrixMultiplicationNonSquare) {
    Matrix A({{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}});       // 2x3
    Matrix B({{7.0, 8.0}, {9.0, 10.0}, {11.0, 12.0}});  // 3x2
    Matrix C = A * B;

    EXPECT_EQ(C.rows(), 2);
    EXPECT_EQ(C.cols(), 2);
    // C(0,0) = 1*7 + 2*9 + 3*11 = 7 + 18 + 33 = 58
    EXPECT_DOUBLE_EQ(C(0, 0), 58.0);
    // C(0,1) = 1*8 + 2*10 + 3*12 = 8 + 20 + 36 = 64
    EXPECT_DOUBLE_EQ(C(0, 1), 64.0);
    // C(1,0) = 4*7 + 5*9 + 6*11 = 28 + 45 + 66 = 139
    EXPECT_DOUBLE_EQ(C(1, 0), 139.0);
    // C(1,1) = 4*8 + 5*10 + 6*12 = 32 + 50 + 72 = 154
    EXPECT_DOUBLE_EQ(C(1, 1), 154.0);
}

TEST(MatrixTest, MatrixMultiplicationIncompatible) {
    Matrix A(2, 3);
    Matrix B(2, 2);
    EXPECT_THROW(A * B, std::invalid_argument);
}

// Test matrix-vector multiplication
TEST(MatrixTest, MatrixVectorMultiplication) {
    Matrix A({{1.0, 2.0}, {3.0, 4.0}});
    Vector v({5.0, 6.0});
    Vector result = A * v;

    EXPECT_EQ(result.size(), 2);
    // result[0] = 1*5 + 2*6 = 5 + 12 = 17
    EXPECT_DOUBLE_EQ(result[0], 17.0);
    // result[1] = 3*5 + 4*6 = 15 + 24 = 39
    EXPECT_DOUBLE_EQ(result[1], 39.0);
}

// Test matrix addition
TEST(MatrixTest, MatrixAddition) {
    Matrix A({{1.0, 2.0}, {3.0, 4.0}});
    Matrix B({{5.0, 6.0}, {7.0, 8.0}});
    Matrix C = A + B;

    EXPECT_EQ(C.rows(), 2);
    EXPECT_EQ(C.cols(), 2);
    EXPECT_DOUBLE_EQ(C(0, 0), 6.0);
    EXPECT_DOUBLE_EQ(C(0, 1), 8.0);
    EXPECT_DOUBLE_EQ(C(1, 0), 10.0);
    EXPECT_DOUBLE_EQ(C(1, 1), 12.0);
}

// Test matrix subtraction
TEST(MatrixTest, MatrixSubtraction) {
    Matrix A({{5.0, 6.0}, {7.0, 8.0}});
    Matrix B({{1.0, 2.0}, {3.0, 4.0}});
    Matrix C = A - B;

    EXPECT_EQ(C.rows(), 2);
    EXPECT_EQ(C.cols(), 2);
    EXPECT_DOUBLE_EQ(C(0, 0), 4.0);
    EXPECT_DOUBLE_EQ(C(0, 1), 4.0);
    EXPECT_DOUBLE_EQ(C(1, 0), 4.0);
    EXPECT_DOUBLE_EQ(C(1, 1), 4.0);
}

// Test scalar operations
TEST(MatrixTest, ScalarMultiplication) {
    Matrix A({{1.0, 2.0}, {3.0, 4.0}});
    Matrix B = A * 2.0;

    EXPECT_DOUBLE_EQ(B(0, 0), 2.0);
    EXPECT_DOUBLE_EQ(B(0, 1), 4.0);
    EXPECT_DOUBLE_EQ(B(1, 0), 6.0);
    EXPECT_DOUBLE_EQ(B(1, 1), 8.0);
}

TEST(MatrixTest, ScalarMultiplicationCommutative) {
    Matrix A({{1.0, 2.0}, {3.0, 4.0}});
    Matrix B1 = A * 2.0;
    Matrix B2 = 2.0 * A;

    for (size_t i = 0; i < A.rows(); ++i) {
        for (size_t j = 0; j < A.cols(); ++j) {
            EXPECT_DOUBLE_EQ(B1(i, j), B2(i, j));
        }
    }
}

// Test identity matrix
TEST(MatrixTest, IdentityMatrix) {
    Matrix I = Matrix::identity(3);

    EXPECT_EQ(I.rows(), 3);
    EXPECT_EQ(I.cols(), 3);
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            if (i == j) {
                EXPECT_DOUBLE_EQ(I(i, j), 1.0);
            } else {
                EXPECT_DOUBLE_EQ(I(i, j), 0.0);
            }
        }
    }
}

// Test Cholesky decomposition
TEST(MatrixTest, CholeskyDecomposition2x2) {
    // Simple 2x2 positive-definite matrix
    Matrix A({{4.0, 2.0}, {2.0, 3.0}});
    Matrix L = A.cholesky();

    // Expected L = [[2, 0], [1, sqrt(2)]]
    EXPECT_NEAR(L(0, 0), 2.0, 1e-10);
    EXPECT_NEAR(L(0, 1), 0.0, 1e-10);
    EXPECT_NEAR(L(1, 0), 1.0, 1e-10);
    EXPECT_NEAR(L(1, 1), std::sqrt(2.0), 1e-10);

    // Verify A = L * L^T
    Matrix LT = L.transpose();
    Matrix reconstructed = L * LT;

    for (size_t i = 0; i < A.rows(); ++i) {
        for (size_t j = 0; j < A.cols(); ++j) {
            EXPECT_NEAR(reconstructed(i, j), A(i, j), 1e-10);
        }
    }
}

TEST(MatrixTest, CholeskyDecomposition3x3) {
    // 3x3 positive-definite matrix (covariance-like)
    Matrix A({{4.0, 1.0, 0.5}, {1.0, 3.0, 0.8}, {0.5, 0.8, 2.0}});
    Matrix L = A.cholesky();

    // Verify A = L * L^T
    Matrix LT = L.transpose();
    Matrix reconstructed = L * LT;

    for (size_t i = 0; i < A.rows(); ++i) {
        for (size_t j = 0; j < A.cols(); ++j) {
            EXPECT_NEAR(reconstructed(i, j), A(i, j), 1e-10);
        }
    }
}

TEST(MatrixTest, CholeskyNonSquare) {
    Matrix A(2, 3);
    EXPECT_THROW(A.cholesky(), std::invalid_argument);
}

TEST(MatrixTest, CholeskyNotPositiveDefinite) {
    // Not positive-definite (has negative eigenvalue)
    Matrix A({{1.0, 2.0}, {2.0, 1.0}});
    EXPECT_THROW(A.cholesky(), std::runtime_error);
}

// Test matrix inversion via Cholesky
TEST(MatrixTest, InverseIdentity) {
    Matrix I = Matrix::identity(3);
    Matrix invI = I.inverse();

    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            if (i == j) {
                EXPECT_NEAR(invI(i, j), 1.0, 1e-10);
            } else {
                EXPECT_NEAR(invI(i, j), 0.0, 1e-10);
            }
        }
    }
}

TEST(MatrixTest, Inverse2x2) {
    Matrix A({{4.0, 2.0}, {2.0, 3.0}});
    Matrix invA = A.inverse();

    // Verify A * A^-1 = I
    Matrix I = A * invA;

    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 2; ++j) {
            if (i == j) {
                EXPECT_NEAR(I(i, j), 1.0, 1e-10);
            } else {
                EXPECT_NEAR(I(i, j), 0.0, 1e-10);
            }
        }
    }
}

TEST(MatrixTest, InverseCovarianceMatrix) {
    // Realistic covariance matrix (3 assets)
    // Variances on diagonal, covariances off-diagonal
    Matrix cov({{0.04, 0.01, 0.005},     // Asset 1: 20% std dev
                {0.01, 0.0225, 0.008},   // Asset 2: 15% std dev
                {0.005, 0.008, 0.01}});  // Asset 3: 10% std dev

    Matrix invCov = cov.inverse();

    // Verify cov * invCov = I
    Matrix I = cov * invCov;

    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            if (i == j) {
                EXPECT_NEAR(I(i, j), 1.0, 1e-9);
            } else {
                EXPECT_NEAR(I(i, j), 0.0, 1e-9);
            }
        }
    }
}

// Test numerical stability
TEST(MatrixTest, NumericalStabilityLargeValues) {
    // Large values but well-conditioned
    Matrix A({{1e6, 0.0}, {0.0, 1e6}});
    Matrix invA = A.inverse();

    Matrix I = A * invA;
    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 2; ++j) {
            if (i == j) {
                EXPECT_NEAR(I(i, j), 1.0, 1e-6);
            } else {
                EXPECT_NEAR(I(i, j), 0.0, 1e-6);
            }
        }
    }
}

TEST(MatrixTest, NumericalStabilitySmallValues) {
    // Small values but well-conditioned
    Matrix A({{1e-6, 0.0}, {0.0, 1e-6}});
    Matrix invA = A.inverse();

    Matrix I = A * invA;
    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 2; ++j) {
            if (i == j) {
                EXPECT_NEAR(I(i, j), 1.0, 1e-6);
            } else {
                EXPECT_NEAR(I(i, j), 0.0, 1e-6);
            }
        }
    }
}

// Test triangular system solvers
TEST(MatrixTest, SolveLowerTriangular) {
    // Lower triangular matrix
    Matrix L({{2.0, 0.0}, {1.0, std::sqrt(2.0)}});
    Vector b({4.0, 3.0});
    Vector x = L.solveLower(b);

    // Verify L * x = b
    Vector result = L * x;
    for (size_t i = 0; i < b.size(); ++i) {
        EXPECT_NEAR(result[i], b[i], 1e-10);
    }
}

TEST(MatrixTest, SolveUpperTriangular) {
    // Upper triangular matrix
    Matrix U({{2.0, 1.0}, {0.0, std::sqrt(2.0)}});
    Vector b({5.0, std::sqrt(2.0)});
    Vector x = U.solveUpper(b);

    // Verify U * x = b
    Vector result = U * x;
    for (size_t i = 0; i < b.size(); ++i) {
        EXPECT_NEAR(result[i], b[i], 1e-10);
    }
}

// Test positive-definiteness check
TEST(MatrixTest, IsPositiveDefiniteValidMatrix) {
    // Valid positive-definite covariance matrix
    Matrix m({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});
    EXPECT_TRUE(m.isPositiveDefinite());
}

TEST(MatrixTest, IsPositiveDefiniteIdentityMatrix) {
    // Identity matrix is positive-definite
    Matrix I({{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}});
    EXPECT_TRUE(I.isPositiveDefinite());
}

TEST(MatrixTest, IsPositiveDefiniteNonSquare) {
    // Non-square matrix should return false
    Matrix m(2, 3);
    EXPECT_FALSE(m.isPositiveDefinite());
}

TEST(MatrixTest, IsPositiveDefiniteZeroDiagonal) {
    // Matrix with zero diagonal element should return false
    Matrix m({{1.0, 0.1}, {0.1, 0.0}});
    EXPECT_FALSE(m.isPositiveDefinite());
}

TEST(MatrixTest, IsPositiveDefiniteNegativeDiagonal) {
    // Matrix with negative diagonal element should return false
    Matrix m({{1.0, 0.1}, {0.1, -1.0}});
    EXPECT_FALSE(m.isPositiveDefinite());
}

TEST(MatrixTest, IsPositiveDefiniteSingularMatrix) {
    // Singular symmetric matrix (rank-deficient) is not positive-definite
    // This represents a covariance matrix with two perfectly correlated assets
    // where the second asset has exactly twice the variance of the first
    Matrix m({{1.0, 2.0}, {2.0, 4.0}});
    EXPECT_FALSE(m.isPositiveDefinite());
}

TEST(MatrixTest, IsPositiveDefiniteHighCorrelation) {
    // Matrix with very high correlation (0.99) - still positive-definite but close to singular
    Matrix m({{1.0, 0.99, 0.99}, {0.99, 1.0, 0.99}, {0.99, 0.99, 1.0}});
    // This should still be positive-definite (though numerically challenging)
    EXPECT_TRUE(m.isPositiveDefinite());
}

TEST(MatrixTest, IsPositiveDefinitePerfectCorrelation) {
    // Matrix with perfect correlation (rank-deficient) - not positive-definite
    Matrix m({{1.0, 1.0}, {1.0, 1.0}});
    EXPECT_FALSE(m.isPositiveDefinite());
}

TEST(MatrixTest, IsPositiveDefiniteReasonableCorrelation) {
    // Matrix with reasonable correlations should pass
    double var1 = 0.04;
    double var2 = 0.0225;
    double corr = 0.5;
    double cov12 = corr * std::sqrt(var1 * var2);
    Matrix m({{var1, cov12}, {cov12, var2}});
    EXPECT_TRUE(m.isPositiveDefinite());
}

TEST(MatrixTest, IsPositiveDefiniteNegativeSemiDefinite) {
    // Negative semi-definite matrix should fail
    Matrix m({{-1.0, 0.0}, {0.0, -1.0}});
    EXPECT_FALSE(m.isPositiveDefinite());
}
