# Black-Litterman Model

## Overview

The Black-Litterman model is a sophisticated portfolio optimization framework that combines market equilibrium with investor views to produce more stable and intuitive portfolio allocations. Developed by Fischer Black and Robert Litterman at Goldman Sachs in 1990, it addresses key limitations of traditional mean-variance optimization.

## Key Concepts

### 1. Equilibrium Returns (Π)

The model starts with **implied equilibrium returns** derived from market capitalization weights using reverse optimization:

```
Π = λ × Σ × w_market
```

Where:
- `Π` = equilibrium returns vector
- `λ` = market risk aversion parameter (typically 2.5-3.5)
- `Σ` = covariance matrix of asset returns
- `w_market` = market capitalization weights

These returns represent the market's consensus view, assuming all investors are mean-variance optimizers.

### 2. Investor Views

Views are expressed as linear combinations of asset returns:

**Absolute View:** "Asset i will return r%"
```
P = [0, ..., 1, ..., 0]  (1 in position i)
Q = r
```

**Relative View:** "Asset i will outperform asset j by r%"
```
P = [0, ..., 1, ..., -1, ..., 0]  (1 in position i, -1 in position j)
Q = r
```

Each view has a confidence level (0 to 1) that controls its impact on the posterior returns.

### 3. View Uncertainty (Ω)

The uncertainty in each view is captured by the diagonal matrix Ω:

```
Ω_ii = (1/confidence - 1) × P_i × (τΣ) × P_i'
```

Where:
- Higher confidence → lower uncertainty → greater impact on posterior
- Lower confidence → higher uncertainty → less impact on posterior

### 4. Posterior Returns

The Black-Litterman formula combines equilibrium and views:

```
μ_BL = [(τΣ)^(-1) + P'Ω^(-1)P]^(-1) × [(τΣ)^(-1)Π + P'Ω^(-1)Q]
```

Where:
- `τ` = scalar uncertainty parameter (typically 0.01-0.05)
- `P` = view matrix (K×N where K = number of views, N = number of assets)
- `Q` = view returns vector (K×1)
- `Ω` = view uncertainty matrix (K×K diagonal)

## API Usage

### Basic Example

```cpp
#include "orbat/optimizer/black_litterman.hpp"

using namespace orbat::optimizer;
using namespace orbat::core;

// Define market equilibrium
Vector marketWeights({0.60, 0.25, 0.15});  // US, International, Bonds
CovarianceMatrix cov({
    {0.0400, 0.0150, 0.0080},
    {0.0150, 0.0625, 0.0100},
    {0.0080, 0.0100, 0.0100}
});

// Create optimizer
double riskAversion = 2.5;
double tau = 0.025;
BlackLittermanOptimizer bl(marketWeights, cov, riskAversion, tau);

// Get equilibrium returns
const auto& equilibrium = bl.equilibriumReturns();
```

### Adding Views

```cpp
// Absolute view: US stocks will return 10%
View view1({1.0, 0.0, 0.0}, 0.10, 0.75);  // 75% confidence
bl.addView(view1);

// Relative view: International will outperform US by 2%
View view2({-1.0, 1.0, 0.0}, 0.02, 0.60);  // 60% confidence
bl.addView(view2);

// Compute posterior returns
auto posteriorReturns = bl.computePosteriorReturns();
```

### Portfolio Optimization

```cpp
// Optimize with default risk aversion
auto result = bl.optimize();

std::cout << "Weights: ";
for (size_t i = 0; i < result.weights.size(); ++i) {
    std::cout << result.weights[i] * 100 << "% ";
}
std::cout << std::endl;
std::cout << "Expected Return: " << result.expectedReturn * 100 << "%" << std::endl;
std::cout << "Risk: " << result.risk * 100 << "%" << std::endl;
```

## Key Properties

### Zero-View Property
With no views, posterior returns equal equilibrium returns:
```cpp
BlackLittermanOptimizer bl(marketWeights, cov, riskAversion);
auto posterior = bl.computePosteriorReturns();
// posterior == equilibrium returns
```

### View Confidence
- **High confidence (0.9-0.99):** View dominates equilibrium
- **Medium confidence (0.5-0.8):** Balanced blend
- **Low confidence (0.1-0.4):** Equilibrium dominates

### Multiple Views
Views are combined consistently through the Bayesian framework:
```cpp
bl.addView(view1);
bl.addView(view2);
bl.addView(view3);
auto posterior = bl.computePosteriorReturns();
```

## Parameters

### Risk Aversion (λ)
- **2.5-3.5:** Typical market risk aversion
- **Higher values:** More conservative (less risk-taking)
- **Lower values:** More aggressive (more risk-taking)

### Tau (τ)
- **0.025:** Standard value (recommended)
- **0.01-0.05:** Acceptable range
- Represents uncertainty in the prior (equilibrium returns)

### Confidence
- **0.0-1.0:** Range for view confidence
- **0.5:** Neutral confidence
- **0.8-0.9:** High confidence (common for strong views)
- **0.3-0.5:** Low confidence (weak views)

## Advantages Over Traditional Mean-Variance

1. **Stability:** Less sensitive to estimation errors in expected returns
2. **Intuitive:** Investors express views naturally (absolute/relative)
3. **Flexible:** Easy to incorporate multiple views
4. **Consistent:** Bayesian framework ensures coherence
5. **Practical:** Market equilibrium provides sensible starting point

## Best Practices

### 1. Choose Appropriate Market Weights
Use actual market capitalization weights when available:
```cpp
// Good: Actual market weights
Vector marketWeights({0.60, 0.25, 0.15});

// Avoid: Equal weights (not market equilibrium)
Vector badWeights({0.33, 0.33, 0.34});
```

### 2. Calibrate Risk Aversion
Match to your risk profile:
```cpp
double conservative = 3.5;  // More weight on risk minimization
double moderate = 2.5;      // Balanced
double aggressive = 1.5;    // More weight on return maximization
```

### 3. Express Views Carefully
- Use confidence to reflect uncertainty
- Prefer relative views when appropriate
- Don't overfit (too many views)

```cpp
// Good: Moderate confidence, reasonable return expectation
View goodView({1.0, 0.0, 0.0}, 0.10, 0.70);

// Risky: Very high confidence and extreme return
View riskyView({1.0, 0.0, 0.0}, 0.50, 0.99);
```

### 4. Validate Results
Check that posterior returns are reasonable:
```cpp
auto posterior = bl.computePosteriorReturns();
// Verify returns are in reasonable range
// Check that high-confidence views had impact
```

## Mathematical Details

### Derivation
The Black-Litterman model is derived from Bayes' theorem, treating:
- **Prior:** Equilibrium returns with uncertainty τΣ
- **Likelihood:** Views with uncertainty Ω
- **Posterior:** Blended returns

### Posterior Distribution
The posterior returns follow a normal distribution:
```
μ_BL ~ N(μ̂, Σ_BL)
```

Where:
- `μ̂` = posterior mean (from BL formula)
- `Σ_BL = [(τΣ)^(-1) + P'Ω^(-1)P]^(-1)` = posterior covariance

## Example Use Cases

### 1. Tactical Asset Allocation
```cpp
// Base allocation on market equilibrium
// Add short-term views based on analysis
View nearTermView({1.0, -1.0, 0.0}, 0.03, 0.60);
bl.addView(nearTermView);
```

### 2. Factor-Based Investing
```cpp
// View: Value factor will outperform growth by 4%
View valueView({1.0, -1.0, 0.0}, 0.04, 0.70);
bl.addView(valueView);
```

### 3. Scenario Analysis
```cpp
// Recession scenario: bonds outperform stocks
View recessionView({-1.0, 0.0, 1.0}, 0.05, 0.75);
bl.addView(recessionView);
```

## References

1. Black, F., & Litterman, R. (1992). "Global Portfolio Optimization." Financial Analysts Journal, 48(5), 28-43.

2. He, G., & Litterman, R. (1999). "The Intuition Behind Black-Litterman Model Portfolios." Goldman Sachs Quantitative Resources Group.

3. Idzorek, T. (2005). "A Step-by-Step Guide to the Black-Litterman Model." Zephyr Associates.

## See Also

- [Markowitz Optimization](markowitz.md)
- [Efficient Frontier](efficient_frontier.md)
- [Portfolio Constraints](constraints.md)
