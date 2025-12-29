#include "orbat/optimizer/black_litterman.hpp"

#include <iomanip>
#include <iostream>

using namespace orbat::optimizer;
using namespace orbat::core;

int main() {
    std::cout << "=== Black-Litterman Portfolio Optimizer Demo ===" << std::endl;
    std::cout << std::endl;

    // Define market equilibrium for three asset classes:
    // Asset 1: US Stocks (60% of market)
    // Asset 2: International Stocks (25% of market)
    // Asset 3: Bonds (15% of market)
    Vector marketWeights({0.60, 0.25, 0.15});

    // Historical covariance matrix (annualized)
    CovarianceMatrix cov({
        {0.0400, 0.0150, 0.0080},  // US Stocks: σ = 20%
        {0.0150, 0.0625, 0.0100},  // Intl Stocks: σ = 25%
        {0.0080, 0.0100, 0.0100}   // Bonds: σ = 10%
    });

    // Market risk aversion (typical value 2.5-3.5)
    double riskAversion = 2.5;
    double tau = 0.025;  // Uncertainty in prior

    // Create Black-Litterman optimizer
    BlackLittermanOptimizer bl(marketWeights, cov, riskAversion, tau);

    std::cout << std::fixed << std::setprecision(2);

    // Show equilibrium (implied) returns
    std::cout << "MARKET EQUILIBRIUM" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    std::cout << "Market capitalization weights:" << std::endl;
    std::cout << "  US Stocks:          " << marketWeights[0] * 100 << "%" << std::endl;
    std::cout << "  International:      " << marketWeights[1] * 100 << "%" << std::endl;
    std::cout << "  Bonds:              " << marketWeights[2] * 100 << "%" << std::endl;
    std::cout << std::endl;

    const auto& eqReturns = bl.equilibriumReturns();
    std::cout << "Implied equilibrium returns (from reverse optimization):" << std::endl;
    std::cout << "  US Stocks:          " << eqReturns[0] * 100 << "%" << std::endl;
    std::cout << "  International:      " << eqReturns[1] * 100 << "%" << std::endl;
    std::cout << "  Bonds:              " << eqReturns[2] * 100 << "%" << std::endl;
    std::cout << std::endl;

    // Scenario 1: No views (baseline)
    std::cout << "SCENARIO 1: NO VIEWS (Market Portfolio)" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    auto result1 = bl.optimize();
    if (result1.success()) {
        std::cout << "Posterior returns = Equilibrium returns (no views)" << std::endl;
        std::cout << "Portfolio allocation:" << std::endl;
        std::cout << "  US Stocks:          " << result1.weights[0] * 100 << "%" << std::endl;
        std::cout << "  International:      " << result1.weights[1] * 100 << "%" << std::endl;
        std::cout << "  Bonds:              " << result1.weights[2] * 100 << "%" << std::endl;
        std::cout << "Expected Return:      " << result1.expectedReturn * 100 << "%" << std::endl;
        std::cout << "Portfolio Risk:       " << result1.risk * 100 << "%" << std::endl;
    }
    std::cout << std::endl;

    // Scenario 2: Bullish on US Stocks
    std::cout << "SCENARIO 2: BULLISH VIEW ON US STOCKS" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    std::cout << "Investor view: US Stocks will return 12% (80% confidence)" << std::endl;
    std::cout << std::endl;

    // Add absolute view: US Stocks will return 12%
    View view1({1.0, 0.0, 0.0}, 0.12, 0.80);
    bl.addView(view1);

    auto posteriorReturns1 = bl.computePosteriorReturns();
    std::cout << "Posterior returns (blending equilibrium + view):" << std::endl;
    std::cout << "  US Stocks:          " << posteriorReturns1[0] * 100 << "%" << std::endl;
    std::cout << "  International:      " << posteriorReturns1[1] * 100 << "%" << std::endl;
    std::cout << "  Bonds:              " << posteriorReturns1[2] * 100 << "%" << std::endl;
    std::cout << std::endl;

    auto result2 = bl.optimize();
    if (result2.success()) {
        std::cout << "Optimal portfolio allocation:" << std::endl;
        std::cout << "  US Stocks:          " << result2.weights[0] * 100 << "%  (was "
                  << marketWeights[0] * 100 << "%)" << std::endl;
        std::cout << "  International:      " << result2.weights[1] * 100 << "%  (was "
                  << marketWeights[1] * 100 << "%)" << std::endl;
        std::cout << "  Bonds:              " << result2.weights[2] * 100 << "%  (was "
                  << marketWeights[2] * 100 << "%)" << std::endl;
        std::cout << "Expected Return:      " << result2.expectedReturn * 100 << "%" << std::endl;
        std::cout << "Portfolio Risk:       " << result2.risk * 100 << "%" << std::endl;
    }
    std::cout << std::endl;

    // Scenario 3: Relative view - International outperforms US
    bl.clearViews();
    std::cout << "SCENARIO 3: RELATIVE VIEW" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    std::cout << "Investor view: International will outperform US by 3% (70% confidence)"
              << std::endl;
    std::cout << std::endl;

    // Add relative view: International outperforms US by 3%
    View view2({-1.0, 1.0, 0.0}, 0.03, 0.70);
    bl.addView(view2);

    auto posteriorReturns2 = bl.computePosteriorReturns();
    std::cout << "Posterior returns (blending equilibrium + view):" << std::endl;
    std::cout << "  US Stocks:          " << posteriorReturns2[0] * 100 << "%" << std::endl;
    std::cout << "  International:      " << posteriorReturns2[1] * 100 << "%" << std::endl;
    std::cout << "  Bonds:              " << posteriorReturns2[2] * 100 << "%" << std::endl;
    std::cout << std::endl;

    auto result3 = bl.optimize();
    if (result3.success()) {
        std::cout << "Optimal portfolio allocation:" << std::endl;
        std::cout << "  US Stocks:          " << result3.weights[0] * 100 << "%  (was "
                  << marketWeights[0] * 100 << "%)" << std::endl;
        std::cout << "  International:      " << result3.weights[1] * 100 << "%  (was "
                  << marketWeights[1] * 100 << "%)" << std::endl;
        std::cout << "  Bonds:              " << result3.weights[2] * 100 << "%  (was "
                  << marketWeights[2] * 100 << "%)" << std::endl;
        std::cout << "Expected Return:      " << result3.expectedReturn * 100 << "%" << std::endl;
        std::cout << "Portfolio Risk:       " << result3.risk * 100 << "%" << std::endl;
    }
    std::cout << std::endl;

    // Scenario 4: Multiple views
    bl.clearViews();
    std::cout << "SCENARIO 4: MULTIPLE VIEWS" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    std::cout << "View 1: US Stocks will return 11% (75% confidence)" << std::endl;
    std::cout << "View 2: Bonds will return 4% (60% confidence)" << std::endl;
    std::cout << std::endl;

    View view3({1.0, 0.0, 0.0}, 0.11, 0.75);
    View view4({0.0, 0.0, 1.0}, 0.04, 0.60);
    bl.addView(view3);
    bl.addView(view4);

    auto posteriorReturns3 = bl.computePosteriorReturns();
    std::cout << "Posterior returns (blending equilibrium + multiple views):" << std::endl;
    std::cout << "  US Stocks:          " << posteriorReturns3[0] * 100 << "%" << std::endl;
    std::cout << "  International:      " << posteriorReturns3[1] * 100 << "%" << std::endl;
    std::cout << "  Bonds:              " << posteriorReturns3[2] * 100 << "%" << std::endl;
    std::cout << std::endl;

    auto result4 = bl.optimize();
    if (result4.success()) {
        std::cout << "Optimal portfolio allocation:" << std::endl;
        std::cout << "  US Stocks:          " << result4.weights[0] * 100 << "%  (was "
                  << marketWeights[0] * 100 << "%)" << std::endl;
        std::cout << "  International:      " << result4.weights[1] * 100 << "%  (was "
                  << marketWeights[1] * 100 << "%)" << std::endl;
        std::cout << "  Bonds:              " << result4.weights[2] * 100 << "%  (was "
                  << marketWeights[2] * 100 << "%)" << std::endl;
        std::cout << "Expected Return:      " << result4.expectedReturn * 100 << "%" << std::endl;
        std::cout << "Portfolio Risk:       " << result4.risk * 100 << "%" << std::endl;
    }
    std::cout << std::endl;

    // Scenario 5: Effect of view confidence
    std::cout << "SCENARIO 5: IMPACT OF VIEW CONFIDENCE" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    std::cout << "Same view (US Stocks = 12%) with varying confidence levels:" << std::endl;
    std::cout << std::endl;
    std::cout << "Confidence  Posterior Return  US Weight  Expected Return  Risk" << std::endl;

    for (double confidence : {0.2, 0.4, 0.6, 0.8, 0.95}) {
        bl.clearViews();
        View viewConf({1.0, 0.0, 0.0}, 0.12, confidence);
        bl.addView(viewConf);

        auto postRet = bl.computePosteriorReturns();
        auto result = bl.optimize();

        if (result.success()) {
            std::cout << std::setw(10) << confidence * 100 << "%";
            std::cout << std::setw(16) << postRet[0] * 100 << "%";
            std::cout << std::setw(12) << result.weights[0] * 100 << "%";
            std::cout << std::setw(17) << result.expectedReturn * 100 << "%";
            std::cout << std::setw(7) << result.risk * 100 << "%" << std::endl;
        }
    }
    std::cout << std::endl;

    std::cout << "KEY INSIGHTS:" << std::endl;
    std::cout << "- Higher confidence views have greater impact on posterior returns" << std::endl;
    std::cout << "- Views smoothly blend with equilibrium (no discrete jumps)" << std::endl;
    std::cout << "- Multiple views can be combined consistently" << std::endl;
    std::cout << "- Model preserves investor views while accounting for risk" << std::endl;

    return 0;
}
