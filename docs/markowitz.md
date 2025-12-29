# Markowitz Portfolio Optimization

This document provides a comprehensive guide to using the Markowitz portfolio optimizer in the orbat library.

## Overview

The Markowitz optimizer implements the classic mean-variance portfolio optimization framework, which is the foundation of modern portfolio theory. It allows you to construct efficient portfolios that balance risk and return according to your preferences.

## Theory

### Mean-Variance Framework

The Markowitz model solves the following optimization problem:

```
minimize:   w'Σw - λμ'w
subject to: w'1 = 1 (fully invested)
            w ≥ 0 (long-only, optional)
            w_min ≤ w ≤ w_max (box constraints, optional)
```

Where:
- **w** is the vector of portfolio weights (decision variables)
- **Σ** is the covariance matrix of asset returns (risk)
- **μ** is the vector of expected returns
- **λ** is the risk aversion parameter (controls risk-return trade-off)

### Risk Aversion Parameter (λ)

The parameter λ controls the trade-off between minimizing risk and maximizing return:

- **λ = 0**: Pure minimum variance portfolio (ignores returns)
- **λ > 0**: Balanced portfolios (higher λ means more emphasis on returns)
- **λ → ∞**: Maximum return portfolio (ignores risk)

## Usage

### Basic Setup

First, include the necessary headers:

```cpp
#include "orbat/optimizer/markowitz.hpp"
#include "orbat/optimizer/expected_returns.hpp"
#include "orbat/optimizer/covariance_matrix.hpp"

using namespace orbat::optimizer;
```

### Preparing Input Data

You need two inputs: expected returns and a covariance matrix.

```cpp
// Define expected returns for each asset (as decimal, e.g., 0.10 = 10%)
ExpectedReturns returns({0.08, 0.12, 0.15});

// Define covariance matrix (symmetric, positive-definite)
CovarianceMatrix cov({
    {0.04,  0.01,  0.005},
    {0.01,  0.0225, 0.008},
    {0.005, 0.008, 0.01}
});

// Create the optimizer
MarkowitzOptimizer optimizer(returns, cov);
```

### Minimum Variance Portfolio

Find the portfolio with the lowest possible risk:

```cpp
auto result = optimizer.minimumVariance();

if (result.success()) {
    std::cout << "Expected Return: " << result.expectedReturn << std::endl;
    std::cout << "Risk (Std Dev): " << result.risk << std::endl;
    std::cout << "Weights: ";
    for (size_t i = 0; i < result.weights.size(); ++i) {
        std::cout << result.weights[i] << " ";
    }
    std::cout << std::endl;
}
```

### Mean-Variance Optimization with Risk Aversion

Optimize with a specific risk aversion parameter:

```cpp
// Conservative portfolio (lower risk tolerance)
auto conservative = optimizer.optimize(0.5);

// Moderate portfolio (balanced)
auto moderate = optimizer.optimize(2.0);

// Aggressive portfolio (higher return focus)
auto aggressive = optimizer.optimize(10.0);
```

### Target Return Portfolio

Find the minimum risk portfolio that achieves a target return:

```cpp
// Target 12% annual return
auto result = optimizer.targetReturn(0.12);

if (result.success()) {
    std::cout << "Achieved Return: " << result.expectedReturn << std::endl;
    std::cout << "Risk: " << result.risk << std::endl;
}
```

### Efficient Frontier

Generate multiple efficient portfolios:

```cpp
// Generate 50 points along the efficient frontier
auto frontier = optimizer.efficientFrontier(50);

std::cout << "Efficient Frontier:" << std::endl;
std::cout << "Return\tRisk" << std::endl;
for (const auto& portfolio : frontier) {
    std::cout << portfolio.expectedReturn << "\t" 
              << portfolio.risk << std::endl;
}
```

## Adding Constraints

### Long-Only Constraint

Prevent short selling (all weights must be non-negative):

```cpp
ConstraintSet constraints;
constraints.add(std::make_shared<LongOnlyConstraint>());

MarkowitzOptimizer optimizer(returns, cov, constraints);
```

### Box Constraints

Limit individual asset weights:

```cpp
ConstraintSet constraints;

// Each asset between 0% and 40%
constraints.add(std::make_shared<BoxConstraint>(0.0, 0.4));

// Or per-asset limits
std::vector<double> lower = {0.0, 0.1, 0.0};
std::vector<double> upper = {0.5, 0.4, 0.6};
constraints.add(std::make_shared<BoxConstraint>(lower, upper));

MarkowitzOptimizer optimizer(returns, cov, constraints);
```

### Combined Constraints

Use multiple constraints together:

```cpp
ConstraintSet constraints;
constraints.add(std::make_shared<FullyInvestedConstraint>());
constraints.add(std::make_shared<LongOnlyConstraint>());
constraints.add(std::make_shared<BoxConstraint>(0.0, 0.4));

MarkowitzOptimizer optimizer(returns, cov, constraints);
```

## Advanced Configuration

### Solver Parameters

Adjust the optimizer's convergence parameters:

```cpp
MarkowitzOptimizer optimizer(returns, cov);

// Set maximum iterations for constrained optimization
optimizer.setMaxIterations(1000);

// Set convergence tolerance
optimizer.setTolerance(1e-8);
```

## Complete Example

Here's a complete example showing typical usage:

```cpp
#include "orbat/optimizer/markowitz.hpp"
#include <iostream>

int main() {
    using namespace orbat::optimizer;
    
    // Define three assets
    ExpectedReturns returns({0.08, 0.12, 0.15});
    CovarianceMatrix cov({
        {0.04,  0.01,  0.005},
        {0.01,  0.0225, 0.008},
        {0.005, 0.008, 0.01}
    });
    
    // Add constraints
    ConstraintSet constraints;
    constraints.add(std::make_shared<LongOnlyConstraint>());
    constraints.add(std::make_shared<BoxConstraint>(0.0, 0.5));
    
    // Create optimizer
    MarkowitzOptimizer optimizer(returns, cov, constraints);
    
    // Find minimum variance portfolio
    std::cout << "=== Minimum Variance Portfolio ===" << std::endl;
    auto minVar = optimizer.minimumVariance();
    if (minVar.success()) {
        std::cout << "Expected Return: " << minVar.expectedReturn * 100 << "%" << std::endl;
        std::cout << "Risk (Std Dev): " << minVar.risk * 100 << "%" << std::endl;
        std::cout << "Weights: ";
        for (double w : minVar.weights.data()) {
            std::cout << w * 100 << "% ";
        }
        std::cout << std::endl;
    }
    
    // Optimize with different risk aversions
    std::cout << "\n=== Risk-Return Trade-off ===" << std::endl;
    for (double lambda : {0.5, 1.0, 2.0, 5.0}) {
        auto result = optimizer.optimize(lambda);
        if (result.success()) {
            std::cout << "λ = " << lambda 
                      << ": Return = " << result.expectedReturn * 100 << "%, "
                      << "Risk = " << result.risk * 100 << "%" << std::endl;
        }
    }
    
    // Target specific returns
    std::cout << "\n=== Target Return Portfolios ===" << std::endl;
    for (double target : {0.10, 0.12, 0.14}) {
        auto result = optimizer.targetReturn(target);
        if (result.success()) {
            std::cout << "Target = " << target * 100 << "%: "
                      << "Risk = " << result.risk * 100 << "%" << std::endl;
        }
    }
    
    return 0;
}
```

## Interpreting Results

### Portfolio Weights

The `weights` vector contains the allocation to each asset. For example:
- `weights = {0.3, 0.4, 0.3}` means 30% in asset 1, 40% in asset 2, 30% in asset 3

### Expected Return

The expected portfolio return is calculated as:
```
E[R_p] = Σ w_i * μ_i
```

### Risk (Standard Deviation)

The portfolio risk (volatility) is:
```
σ_p = sqrt(w'Σw)
```

### Success Flag

Always check `result.success()` before using the results. If false, check `result.message` for details.

## Best Practices

### 1. Data Quality

- Use realistic expected returns based on historical data or forecasts
- Ensure the covariance matrix is positive-definite
- Validate that variances (diagonal elements) are positive
- Check for data entry errors (e.g., correlations > 1.0)

### 2. Parameter Selection

- Start with moderate risk aversion (λ ≈ 1-5) for balanced portfolios
- Use minimum variance as a conservative baseline
- Generate an efficient frontier to understand the full range of options

### 3. Constraints

- Always include a fully invested constraint (usually automatic)
- Consider long-only constraints for typical investment scenarios
- Use box constraints to prevent excessive concentration
- Verify that constraints are feasible before optimization

### 4. Validation

- Verify that weights sum to 1.0
- Check that risk increases with expected return along the frontier
- Compare results against simple benchmarks (e.g., equal weights)
- Ensure portfolio statistics match your inputs

## Limitations

### 1. Input Sensitivity

The Markowitz model is sensitive to input parameters, particularly expected returns. Small changes in expected returns can lead to large changes in optimal weights.

### 2. Estimation Error

In practice, expected returns and covariances must be estimated from data, which introduces uncertainty. Consider:
- Robust estimation methods
- Shrinkage estimators
- Resampling techniques

### 3. Transaction Costs

The basic model ignores transaction costs. For practical applications, you may need to:
- Add constraints on turnover
- Implement regularization
- Use multi-period models

### 4. Singular Matrices

If assets are perfectly correlated, the covariance matrix becomes singular and the optimizer will fail. This is a fundamental limitation, not a bug.

## References

1. Markowitz, H. (1952). "Portfolio Selection". *Journal of Finance*, 7(1), 77-91.
2. Merton, R. C. (1972). "An Analytic Derivation of the Efficient Portfolio Frontier". *Journal of Financial and Quantitative Analysis*, 7(4), 1851-1872.
3. CFA Institute (2023). *CFA Program Curriculum Level III: Portfolio Management*.

## See Also

- [Linear Algebra Documentation](linear_algebra.md) - Details on Vector and Matrix classes
- [Portfolio Constraints Documentation](constraints.md) - Constraint system reference
- [Expected Returns API](../include/orbat/optimizer/expected_returns.hpp)
- [Covariance Matrix API](../include/orbat/optimizer/covariance_matrix.hpp)
