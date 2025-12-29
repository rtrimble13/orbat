#include "orbat/optimizer/covariance_matrix.hpp"
#include "orbat/optimizer/expected_returns.hpp"

#include <iostream>

/**
 * @brief Example demonstrating the usage of CMA input interfaces.
 *
 * This example shows how to:
 * 1. Load expected returns from CSV and JSON files
 * 2. Load covariance matrices from CSV and JSON files
 * 3. Validate dimensions match between returns and covariance
 * 4. Access the underlying data for use in optimization
 */
int main() {
    using orbat::optimizer::CovarianceMatrix;
    using orbat::optimizer::ExpectedReturns;

    std::cout << "=== CMA Input Interfaces Demo ===" << std::endl << std::endl;

    // Example 1: Creating inputs programmatically
    std::cout << "1. Creating inputs programmatically:" << std::endl;
    ExpectedReturns returns({0.08, 0.12, 0.10});
    CovarianceMatrix cov({{0.04, 0.01, 0.005}, {0.01, 0.0225, 0.008}, {0.005, 0.008, 0.01}});

    std::cout << "   Expected returns for " << returns.size() << " assets:" << std::endl;
    for (size_t i = 0; i < returns.size(); ++i) {
        std::cout << "   Asset " << i << ": " << returns[i] * 100 << "%" << std::endl;
    }

    std::cout << "   Covariance matrix (" << cov.size() << "x" << cov.size() << "):" << std::endl;
    for (size_t i = 0; i < cov.size(); ++i) {
        std::cout << "   ";
        for (size_t j = 0; j < cov.size(); ++j) {
            std::cout << cov(i, j) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    // Example 2: Loading from CSV files
    std::cout << "2. Loading from CSV files:" << std::endl;
    try {
        ExpectedReturns returns_csv = ExpectedReturns::fromCSV("data/expected_returns.csv");
        CovarianceMatrix cov_csv = CovarianceMatrix::fromCSV("data/covariance.csv");

        std::cout << "   Successfully loaded " << returns_csv.size() << " expected returns from CSV"
                  << std::endl;
        std::cout << "   Successfully loaded " << cov_csv.size() << "x" << cov_csv.size()
                  << " covariance matrix from CSV" << std::endl;

        // Validate dimensions match
        if (cov_csv.dimensionsMatch(returns_csv.size())) {
            std::cout << "   ✓ Dimensions match - ready for optimization" << std::endl;
        } else {
            std::cout << "   ✗ Dimension mismatch!" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "   Error: " << e.what() << std::endl;
        std::cout << "   (This is expected if running from a different directory)" << std::endl;
    }
    std::cout << std::endl;

    // Example 3: Loading from JSON files
    std::cout << "3. Loading from JSON files:" << std::endl;
    try {
        ExpectedReturns returns_json = ExpectedReturns::fromJSON("data/expected_returns.json");
        CovarianceMatrix cov_json = CovarianceMatrix::fromJSON("data/covariance.json");

        std::cout << "   Successfully loaded " << returns_json.size()
                  << " expected returns from JSON" << std::endl;
        std::cout << "   Successfully loaded " << cov_json.size() << "x" << cov_json.size()
                  << " covariance matrix from JSON" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "   Error: " << e.what() << std::endl;
        std::cout << "   (This is expected if running from a different directory)" << std::endl;
    }
    std::cout << std::endl;

    // Example 4: Validation errors
    std::cout << "4. Validation examples:" << std::endl;

    // Empty returns - should fail
    try {
        ExpectedReturns empty_returns(orbat::core::Vector{});
        std::cout << "   ✗ Empty returns should have been rejected" << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cout << "   ✓ Empty returns correctly rejected: " << e.what() << std::endl;
    }

    // Non-square covariance - should fail
    try {
        CovarianceMatrix bad_cov(orbat::core::Matrix(2, 3));
        std::cout << "   ✗ Non-square matrix should have been rejected" << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cout << "   ✓ Non-square matrix correctly rejected: " << e.what() << std::endl;
    }

    // Asymmetric covariance - should fail
    try {
        CovarianceMatrix asymmetric({{0.04, 0.01}, {0.02, 0.0225}});
        std::cout << "   ✗ Asymmetric matrix should have been rejected" << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cout << "   ✓ Asymmetric matrix correctly rejected: " << e.what() << std::endl;
    }

    // Negative diagonal - should fail
    try {
        CovarianceMatrix negative_var({{-0.04, 0.01}, {0.01, 0.0225}});
        std::cout << "   ✗ Negative variance should have been rejected" << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cout << "   ✓ Negative variance correctly rejected: " << e.what() << std::endl;
    }

    std::cout << std::endl;

    // Example 5: Accessing data for optimization
    std::cout << "5. Accessing data for optimization algorithms:" << std::endl;
    const auto& returns_data = returns.data();
    const auto& cov_data = cov.data();

    std::cout << "   Returns vector has " << returns_data.size() << " elements" << std::endl;
    std::cout << "   Covariance matrix is " << cov_data.rows() << "x" << cov_data.cols()
              << std::endl;
    std::cout << "   Data can now be passed to portfolio optimizer algorithms" << std::endl;

    std::cout << std::endl << "=== Demo Complete ===" << std::endl;

    return 0;
}
