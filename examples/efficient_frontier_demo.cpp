#include "orbat/optimizer/efficient_frontier.hpp"
#include "orbat/optimizer/markowitz.hpp"

#include <iomanip>
#include <iostream>

using namespace orbat::optimizer;
using namespace orbat::core;

int main() {
    std::cout << "=== Efficient Frontier Demo ===" << std::endl;
    std::cout << std::endl;

    // Define three assets with different risk-return profiles
    ExpectedReturns returns({0.08, 0.12, 0.16});
    std::vector<std::string> assetLabels = {"Bonds", "Balanced Fund", "Growth Stocks"};

    CovarianceMatrix cov({
        {0.0100, 0.0030, 0.0020},  // Bonds: σ = 10%
        {0.0030, 0.0225, 0.0080},  // Balanced: σ = 15%
        {0.0020, 0.0080, 0.0400}   // Stocks: σ = 20%
    });

    // Create optimizer
    MarkowitzOptimizer optimizer(returns, cov);

    std::cout << "Portfolio Universe:" << std::endl;
    std::cout << "  " << assetLabels[0] << ":        E[R] = " << returns[0] * 100
              << "%, σ = " << std::sqrt(cov.data()(0, 0)) * 100 << "%" << std::endl;
    std::cout << "  " << assetLabels[1] << ": E[R] = " << returns[1] * 100
              << "%, σ = " << std::sqrt(cov.data()(1, 1)) * 100 << "%" << std::endl;
    std::cout << "  " << assetLabels[2] << ":  E[R] = " << returns[2] * 100
              << "%, σ = " << std::sqrt(cov.data()(2, 2)) * 100 << "%" << std::endl;
    std::cout << std::endl;

    // Generate efficient frontier with 50 portfolios
    std::cout << "Generating efficient frontier with 50 portfolios..." << std::endl;
    auto frontier = optimizer.efficientFrontier(50);
    std::cout << "Generated " << frontier.size() << " efficient portfolios" << std::endl;
    std::cout << std::endl;

    // Display sample portfolios
    std::cout << "Sample Efficient Portfolios:" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << " Portfolio    Return    Risk   Sharpe    Bonds  Balanced  Stocks" << std::endl;
    std::cout << std::string(80, '-') << std::endl;

    // Show every 5th portfolio
    for (size_t i = 0; i < frontier.size(); i += 5) {
        const auto& p = frontier[i];
        double sharpe = p.expectedReturn / p.risk;  // Assuming rf = 0
        std::cout << std::setw(10) << (i + 1) << std::setw(10) << p.expectedReturn * 100 << "%"
                  << std::setw(8) << p.risk * 100 << "%" << std::setw(8) << sharpe << "   ";

        for (size_t j = 0; j < p.weights.size(); ++j) {
            std::cout << std::setw(7) << static_cast<int>(p.weights[j] * 100) << "%";
            if (j < p.weights.size() - 1) {
                std::cout << "  ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    // Export to CSV
    std::string csvFilename = "efficient_frontier.csv";
    std::cout << "Exporting frontier to CSV: " << csvFilename << std::endl;
    try {
        exportFrontierToCSV(frontier, csvFilename, assetLabels);
        std::cout << "✓ CSV export successful" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "✗ CSV export failed: " << e.what() << std::endl;
    }
    std::cout << std::endl;

    // Export to JSON
    std::string jsonFilename = "efficient_frontier.json";
    std::cout << "Exporting frontier to JSON: " << jsonFilename << std::endl;
    try {
        exportFrontierToJSON(frontier, jsonFilename, assetLabels);
        std::cout << "✓ JSON export successful" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "✗ JSON export failed: " << e.what() << std::endl;
    }
    std::cout << std::endl;

    // Frontier properties
    std::cout << "Efficient Frontier Properties:" << std::endl;
    std::cout << std::string(80, '-') << std::endl;

    // Minimum variance portfolio
    std::cout << "Minimum Variance Portfolio:" << std::endl;
    std::cout << "  Return:     " << frontier.front().expectedReturn * 100 << "%" << std::endl;
    std::cout << "  Risk:       " << frontier.front().risk * 100 << "%" << std::endl;
    std::cout << std::endl;

    // Maximum return portfolio
    std::cout << "Maximum Return Portfolio:" << std::endl;
    std::cout << "  Return:     " << frontier.back().expectedReturn * 100 << "%" << std::endl;
    std::cout << "  Risk:       " << frontier.back().risk * 100 << "%" << std::endl;
    std::cout << std::endl;

    // Verify monotonicity
    bool isMonotonic = true;
    for (size_t i = 1; i < frontier.size(); ++i) {
        if (frontier[i].expectedReturn < frontier[i - 1].expectedReturn - 1e-8) {
            isMonotonic = false;
            break;
        }
    }
    std::cout << "Return Monotonicity:  " << (isMonotonic ? "✓ PASS" : "✗ FAIL") << std::endl;

    // Check convexity (simplified)
    bool isConvex = true;
    for (size_t i = 1; i < frontier.size() - 1; ++i) {
        double var_prev = frontier[i - 1].risk * frontier[i - 1].risk;
        double var_curr = frontier[i].risk * frontier[i].risk;
        double var_next = frontier[i + 1].risk * frontier[i + 1].risk;

        // For evenly spaced returns, check if variance is convex
        if (var_curr > (var_prev + var_next) / 2.0 + 1e-4) {
            isConvex = false;
            break;
        }
    }
    std::cout << "Variance Convexity:   " << (isConvex ? "✓ PASS" : "✗ FAIL") << std::endl;
    std::cout << std::endl;

    // Plotting instructions
    std::cout << "Visualization Instructions:" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    std::cout << "The generated files can be used directly with plotting tools:" << std::endl;
    std::cout << std::endl;
    std::cout << "Python (matplotlib):" << std::endl;
    std::cout << "  import pandas as pd" << std::endl;
    std::cout << "  import matplotlib.pyplot as plt" << std::endl;
    std::cout << "  df = pd.read_csv('efficient_frontier.csv')" << std::endl;
    std::cout << "  plt.plot(df['volatility']*100, df['return']*100)" << std::endl;
    std::cout << "  plt.xlabel('Risk (%)')" << std::endl;
    std::cout << "  plt.ylabel('Return (%)')" << std::endl;
    std::cout << "  plt.title('Efficient Frontier')" << std::endl;
    std::cout << "  plt.show()" << std::endl;
    std::cout << std::endl;

    std::cout << "R (ggplot2):" << std::endl;
    std::cout << "  library(ggplot2)" << std::endl;
    std::cout << "  df <- read.csv('efficient_frontier.csv')" << std::endl;
    std::cout << "  ggplot(df, aes(x=volatility*100, y=return*100)) +" << std::endl;
    std::cout << "    geom_line() + xlab('Risk (%)') + ylab('Return (%)')" << std::endl;
    std::cout << std::endl;

    std::cout << "JavaScript (D3.js, Chart.js):" << std::endl;
    std::cout << "  Use efficient_frontier.json with any web visualization library" << std::endl;
    std::cout << std::endl;

    std::cout << "=== Demo Complete ===" << std::endl;

    return 0;
}
