#include "orbat/optimizer/markowitz.hpp"

#include <iomanip>
#include <iostream>

using namespace orbat::optimizer;
using namespace orbat::core;

int main() {
    std::cout << "=== Markowitz Portfolio Optimizer Demo ===" << std::endl;
    std::cout << std::endl;

    // Define three assets with different risk-return profiles
    // Asset 1: Low risk, low return (e.g., bonds)
    // Asset 2: Medium risk, medium return (e.g., balanced fund)
    // Asset 3: High risk, high return (e.g., growth stocks)

    ExpectedReturns returns({0.08, 0.12, 0.16});

    CovarianceMatrix cov({
        {0.0100, 0.0030, 0.0020},  // Asset 1: σ = 10%
        {0.0030, 0.0225, 0.0080},  // Asset 2: σ = 15%
        {0.0020, 0.0080, 0.0400}   // Asset 3: σ = 20%
    });

    // Add long-only and concentration constraints
    ConstraintSet constraints;
    constraints.add(std::make_shared<LongOnlyConstraint>());
    constraints.add(std::make_shared<BoxConstraint>(0.0, 0.60));  // Max 60% per asset

    // Create optimizer
    MarkowitzOptimizer optimizer(returns, cov, constraints);

    // 1. Minimum Variance Portfolio
    std::cout << "1. MINIMUM VARIANCE PORTFOLIO" << std::endl;
    std::cout << "   (Lowest possible risk)" << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    auto minVar = optimizer.minimumVariance();
    if (minVar.success()) {
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "   Expected Return: " << minVar.expectedReturn * 100 << "%" << std::endl;
        std::cout << "   Risk (Std Dev):  " << minVar.risk * 100 << "%" << std::endl;
        std::cout << "   Portfolio Weights:" << std::endl;
        std::cout << "     Asset 1 (Bonds):       " << minVar.weights[0] * 100 << "%" << std::endl;
        std::cout << "     Asset 2 (Balanced):    " << minVar.weights[1] * 100 << "%" << std::endl;
        std::cout << "     Asset 3 (Stocks):      " << minVar.weights[2] * 100 << "%" << std::endl;
    }
    std::cout << std::endl;

    // 2. Mean-Variance Optimization with Different Risk Aversions
    std::cout << "2. MEAN-VARIANCE OPTIMIZATION" << std::endl;
    std::cout << "   (Different risk-return preferences)" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    std::cout << "   Risk Aversion (λ)  Return    Risk" << std::endl;

    for (double lambda : {0.5, 1.0, 2.0, 5.0, 10.0}) {
        auto result = optimizer.optimize(lambda);
        if (result.success()) {
            std::cout << "   " << std::setw(17) << lambda << std::setw(10)
                      << result.expectedReturn * 100 << "%" << std::setw(8) << result.risk * 100
                      << "%" << std::endl;
        }
    }
    std::cout << std::endl;

    // 3. Target Return Portfolios
    std::cout << "3. TARGET RETURN PORTFOLIOS" << std::endl;
    std::cout << "   (Minimum risk for target return)" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    std::cout << "   Target Return  Achieved    Risk    Asset Allocation" << std::endl;

    for (double target : {0.09, 0.11, 0.13, 0.15}) {
        auto result = optimizer.targetReturn(target);
        if (result.success()) {
            std::cout << "   " << std::setw(13) << target * 100 << "%" << std::setw(10)
                      << result.expectedReturn * 100 << "%" << std::setw(8) << result.risk * 100
                      << "%"
                      << "    " << std::setw(5) << static_cast<int>(result.weights[0] * 100)
                      << "% / " << std::setw(3) << static_cast<int>(result.weights[1] * 100)
                      << "% / " << std::setw(3) << static_cast<int>(result.weights[2] * 100) << "%"
                      << std::endl;
        } else {
            std::cout << "   " << std::setw(13) << target * 100 << "%  "
                      << "Not achievable with constraints" << std::endl;
        }
    }
    std::cout << std::endl;

    // 4. Efficient Frontier
    std::cout << "4. EFFICIENT FRONTIER" << std::endl;
    std::cout << "   (Optimal portfolios for different return levels)" << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    auto frontier = optimizer.efficientFrontier(15);
    if (!frontier.empty()) {
        std::cout << "   Portfolio #  Return    Risk" << std::endl;
        for (size_t i = 0; i < frontier.size(); ++i) {
            const auto& p = frontier[i];
            std::cout << "   " << std::setw(11) << (i + 1) << std::setw(10)
                      << p.expectedReturn * 100 << "%" << std::setw(8) << p.risk * 100 << "%"
                      << std::endl;
        }
    }
    std::cout << std::endl;

    // 5. Risk-Return Trade-off Analysis
    std::cout << "5. RISK-RETURN TRADE-OFF" << std::endl;
    std::cout << "   (Comparing different strategies)" << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    // Equal weights (naive diversification)
    Vector equalWeights(3, 1.0 / 3.0);
    double equalReturn = returns.data().dot(equalWeights);
    Vector Sw = cov.data() * equalWeights;
    double equalRisk = std::sqrt(equalWeights.dot(Sw));

    std::cout << "   Strategy             Return    Risk    Sharpe Ratio*" << std::endl;
    std::cout << "   Equal Weights      " << std::setw(8) << equalReturn * 100 << "%"
              << std::setw(8) << equalRisk * 100 << "%       " << std::setw(4)
              << std::setprecision(2) << (equalReturn / equalRisk) << std::endl;

    auto conservative = optimizer.optimize(0.5);
    if (conservative.success()) {
        std::cout << "   Conservative       " << std::setw(8) << conservative.expectedReturn * 100
                  << "%" << std::setw(8) << conservative.risk * 100 << "%       " << std::setw(4)
                  << (conservative.expectedReturn / conservative.risk) << std::endl;
    }

    auto moderate = optimizer.optimize(2.0);
    if (moderate.success()) {
        std::cout << "   Moderate           " << std::setw(8) << moderate.expectedReturn * 100
                  << "%" << std::setw(8) << moderate.risk * 100 << "%       " << std::setw(4)
                  << (moderate.expectedReturn / moderate.risk) << std::endl;
    }

    auto aggressive = optimizer.optimize(10.0);
    if (aggressive.success()) {
        std::cout << "   Aggressive         " << std::setw(8) << aggressive.expectedReturn * 100
                  << "%" << std::setw(8) << aggressive.risk * 100 << "%       " << std::setw(4)
                  << (aggressive.expectedReturn / aggressive.risk) << std::endl;
    }

    std::cout << "   * Assumes risk-free rate = 0" << std::endl;
    std::cout << std::endl;

    std::cout << "=== Demo Complete ===" << std::endl;

    return 0;
}
