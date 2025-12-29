#include "orbat/core/matrix.hpp"
#include "orbat/core/vector.hpp"

#include <iomanip>
#include <iostream>

using orbat::core::Matrix;
using orbat::core::Vector;

void printVector(const std::string& name, const Vector& v) {
    std::cout << name << " = [";
    for (size_t i = 0; i < v.size(); ++i) {
        std::cout << std::fixed << std::setprecision(4) << v[i];
        if (i < v.size() - 1)
            std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

void printMatrix(const std::string& name, const Matrix& m) {
    std::cout << name << " = " << std::endl;
    for (size_t i = 0; i < m.rows(); ++i) {
        std::cout << "  [";
        for (size_t j = 0; j < m.cols(); ++j) {
            std::cout << std::fixed << std::setprecision(4) << std::setw(8) << m(i, j);
            if (j < m.cols() - 1)
                std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
}

int main() {
    std::cout << "=== Vector Operations ===" << std::endl << std::endl;

    // Create vectors
    Vector returns1({0.10, 0.12, 0.15});  // Asset 1 returns
    Vector returns2({0.08, 0.11, 0.13});  // Asset 2 returns
    Vector weights({0.6, 0.4});           // Portfolio weights

    printVector("Asset 1 returns", returns1);
    printVector("Asset 2 returns", returns2);
    std::cout << std::endl;

    // Compute dot product
    double correlation = returns1.dot(returns2);
    std::cout << "Dot product (returns correlation): " << std::fixed << std::setprecision(6)
              << correlation << std::endl;

    // Vector operations
    Vector sum = returns1 + returns2;
    printVector("Sum of returns", sum);

    Vector scaled = returns1 * 2.0;
    printVector("Scaled returns (x2)", scaled);

    double norm = returns1.norm();
    std::cout << "L2 norm of returns1: " << std::fixed << std::setprecision(6) << norm << std::endl;

    std::cout << std::endl;
    std::cout << "=== Matrix Operations ===" << std::endl << std::endl;

    // Create a covariance matrix (3 assets)
    Matrix covariance({{0.0400, 0.0100, 0.0050},    // Asset 1: 20% std dev
                       {0.0100, 0.0225, 0.0080},    // Asset 2: 15% std dev
                       {0.0050, 0.0080, 0.0100}});  // Asset 3: 10% std dev

    printMatrix("Covariance Matrix", covariance);
    std::cout << std::endl;

    // Transpose
    Matrix covT = covariance.transpose();
    std::cout << "Is symmetric (cov == cov^T)? " << (covT(0, 1) == covariance(1, 0) ? "Yes" : "No")
              << std::endl;
    std::cout << std::endl;

    // Matrix multiplication
    Vector portfolioWeights({0.5, 0.3, 0.2});
    printVector("Portfolio weights", portfolioWeights);

    Vector covTimesWeights = covariance * portfolioWeights;
    printVector("Covariance * weights", covTimesWeights);

    // Portfolio variance = w^T * Cov * w
    double portfolioVariance = portfolioWeights.dot(covTimesWeights);
    double portfolioStdDev = std::sqrt(portfolioVariance);

    std::cout << "Portfolio variance: " << std::fixed << std::setprecision(6) << portfolioVariance
              << std::endl;
    std::cout << "Portfolio std dev: " << std::fixed << std::setprecision(4)
              << portfolioStdDev * 100 << "%" << std::endl;
    std::cout << std::endl;

    // Matrix inversion using Cholesky decomposition
    std::cout << "=== Matrix Inversion (Cholesky) ===" << std::endl << std::endl;

    Matrix invCov = covariance.inverse();
    printMatrix("Inverse Covariance Matrix", invCov);
    std::cout << std::endl;

    // Verify: Cov * Cov^-1 = I
    Matrix identity = covariance * invCov;
    printMatrix("Covariance * Inverse (should be Identity)", identity);
    std::cout << std::endl;

    // Check if result is close to identity
    bool isIdentity = true;
    for (size_t i = 0; i < identity.rows(); ++i) {
        for (size_t j = 0; j < identity.cols(); ++j) {
            double expected = (i == j) ? 1.0 : 0.0;
            if (std::abs(identity(i, j) - expected) > 1e-9) {
                isIdentity = false;
            }
        }
    }
    std::cout << "Verification: " << (isIdentity ? "PASSED" : "FAILED") << std::endl;

    std::cout << std::endl;
    std::cout << "=== Cholesky Decomposition ===" << std::endl << std::endl;

    Matrix L = covariance.cholesky();
    printMatrix("Cholesky factor L", L);
    std::cout << std::endl;

    // Verify: L * L^T = Covariance
    Matrix LT = L.transpose();
    Matrix reconstructed = L * LT;
    printMatrix("L * L^T (should equal Covariance)", reconstructed);
    std::cout << std::endl;

    return 0;
}
