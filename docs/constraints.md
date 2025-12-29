# Portfolio Constraints

This guide covers the portfolio constraint system in orbat, which enables real-world investment constraints consistent with CFA Institute best practices.

## Overview

The constraint system provides a modular and composable framework for enforcing portfolio rules during optimization. Constraints can be combined to model complex real-world portfolio requirements including regulatory restrictions, risk management rules, and investment policies.

## Constraint Interface

All constraints inherit from the abstract `Constraint` base class:

```cpp
#include "orbat/optimizer/constraint.hpp"

using orbat::optimizer::Constraint;
using orbat::core::Vector;

// Check if portfolio weights satisfy a constraint
bool feasible = constraint->isFeasible(weights);

// Get constraint information
std::string name = constraint->getName();
std::string description = constraint->getDescription();
```

## Built-in Constraints

### FullyInvestedConstraint

Ensures that portfolio weights sum to 1.0 (100% invested).

**Use Case:** Most long-only portfolios require full investment of capital.

**Example:**
```cpp
#include "orbat/optimizer/constraint.hpp"

using orbat::optimizer::FullyInvestedConstraint;
using orbat::core::Vector;

// Default tolerance (EPSILON)
FullyInvestedConstraint fullyInvested;

// Custom tolerance
FullyInvestedConstraint fullyInvested(1e-6);

// Check feasibility
Vector weights({0.3, 0.4, 0.3});  // Sum = 1.0
bool feasible = fullyInvested.isFeasible(weights);  // true

Vector weights2({0.3, 0.3, 0.3});  // Sum = 0.9
feasible = fullyInvested.isFeasible(weights2);  // false
```

### LongOnlyConstraint

Prohibits short selling by requiring all weights to be non-negative.

**Use Case:** Traditional portfolios, mutual funds, and many institutional mandates prohibit short positions.

**Example:**
```cpp
#include "orbat/optimizer/constraint.hpp"

using orbat::optimizer::LongOnlyConstraint;
using orbat::core::Vector;

LongOnlyConstraint longOnly;

// Feasible: all non-negative
Vector weights({0.4, 0.35, 0.25});
bool feasible = longOnly.isFeasible(weights);  // true

// Infeasible: contains short position
Vector shortWeights({0.6, -0.2, 0.6});
feasible = longOnly.isFeasible(shortWeights);  // false
```

### BoxConstraint

Limits individual asset weights to specified bounds.

**Use Cases:**
- Limiting concentration risk
- Enforcing regulatory position limits
- Implementing portfolio management policies

**Uniform Bounds Example:**
```cpp
#include "orbat/optimizer/constraint.hpp"

using orbat::optimizer::BoxConstraint;
using orbat::core::Vector;

// Each asset between 0% and 40%
BoxConstraint box(0.0, 0.4);

Vector weights({0.3, 0.35, 0.35});
bool feasible = box.isFeasible(weights);  // true

Vector concentrated({0.5, 0.3, 0.2});
feasible = box.isFeasible(concentrated);  // false (0.5 > 0.4)
```

**Per-Asset Bounds Example:**
```cpp
#include "orbat/optimizer/constraint.hpp"

using orbat::optimizer::BoxConstraint;
using orbat::core::Vector;

// Different bounds for different assets
std::vector<double> lower = {0.1, 0.0, 0.0};  // Asset 1: min 10%
std::vector<double> upper = {0.5, 0.4, 0.6};  // Asset 1: max 50%

BoxConstraint box(lower, upper);

Vector weights({0.3, 0.3, 0.4});
bool feasible = box.isFeasible(weights);  // true

Vector infeasible({0.05, 0.45, 0.5});  // Asset 1 < 10%
feasible = box.isFeasible(infeasible);  // false
```

## ConstraintSet: Composing Multiple Constraints

The `ConstraintSet` class manages multiple constraints and checks them collectively.

**Example:**
```cpp
#include "orbat/optimizer/constraint.hpp"

using orbat::optimizer::ConstraintSet;
using orbat::optimizer::FullyInvestedConstraint;
using orbat::optimizer::LongOnlyConstraint;
using orbat::optimizer::BoxConstraint;
using orbat::core::Vector;

// Create constraint set
ConstraintSet constraints;
constraints.add(std::make_shared<FullyInvestedConstraint>());
constraints.add(std::make_shared<LongOnlyConstraint>());
constraints.add(std::make_shared<BoxConstraint>(0.0, 0.4));

// Check if weights satisfy all constraints
Vector weights({0.3, 0.35, 0.35});
if (constraints.isFeasible(weights)) {
    // All constraints satisfied
}

// Iterate over constraints
for (const auto& constraint : constraints.getConstraints()) {
    std::cout << constraint->getName() << ": "
              << constraint->getDescription() << std::endl;
}
```

## Feasibility Detection

The `ConstraintSet::hasInfeasibleCombination()` method detects obviously infeasible constraint combinations before optimization.

**Example:**
```cpp
#include "orbat/optimizer/constraint.hpp"

using orbat::optimizer::ConstraintSet;
using orbat::optimizer::FullyInvestedConstraint;
using orbat::optimizer::BoxConstraint;

// Infeasible: upper bounds too low
ConstraintSet constraints;
constraints.add(std::make_shared<FullyInvestedConstraint>());
constraints.add(std::make_shared<BoxConstraint>(0.0, 0.2));  // Max 0.2 per asset

size_t numAssets = 3;  // 3 * 0.2 = 0.6 < 1.0
if (constraints.hasInfeasibleCombination(numAssets)) {
    std::cerr << "Constraint set is infeasible!" << std::endl;
    // Cannot achieve fully invested with these bounds
}
```

**Common Infeasible Combinations:**
- Fully invested + box constraints where sum(upper bounds) < 1.0
- Fully invested + box constraints where sum(lower bounds) > 1.0
- Long-only + box constraints with all negative upper bounds

## Real-World Examples

### Example 1: Traditional Long-Only Portfolio

```cpp
#include "orbat/optimizer/constraint.hpp"

using orbat::optimizer::ConstraintSet;
using orbat::optimizer::FullyInvestedConstraint;
using orbat::optimizer::LongOnlyConstraint;
using orbat::optimizer::BoxConstraint;

// Typical institutional constraints
ConstraintSet constraints;
constraints.add(std::make_shared<FullyInvestedConstraint>());
constraints.add(std::make_shared<LongOnlyConstraint>());
constraints.add(std::make_shared<BoxConstraint>(0.0, 0.4));  // Max 40% per position

// Pre-check feasibility
if (constraints.hasInfeasibleCombination(5)) {
    throw std::runtime_error("Infeasible constraints");
}
```

### Example 2: Multi-Asset Class Portfolio

```cpp
#include "orbat/optimizer/constraint.hpp"

using orbat::optimizer::ConstraintSet;
using orbat::optimizer::FullyInvestedConstraint;
using orbat::optimizer::LongOnlyConstraint;
using orbat::optimizer::BoxConstraint;

// 10 assets: 3 stocks, 4 bonds, 3 alternatives
std::vector<double> lower = {
    0.05, 0.05, 0.00,  // Stocks: min 5% for first two
    0.10, 0.10, 0.00, 0.00,  // Bonds: min 10% for first two
    0.00, 0.00, 0.00   // Alternatives
};

std::vector<double> upper = {
    0.30, 0.30, 0.25,  // Stocks: max 30%, 30%, 25%
    0.35, 0.35, 0.20, 0.20,  // Bonds
    0.15, 0.15, 0.10   // Alternatives: limited exposure
};

ConstraintSet constraints;
constraints.add(std::make_shared<FullyInvestedConstraint>());
constraints.add(std::make_shared<LongOnlyConstraint>());
constraints.add(std::make_shared<BoxConstraint>(lower, upper));

// Verify feasibility
if (!constraints.hasInfeasibleCombination(10)) {
    // Constraints are feasible, proceed with optimization
}
```

### Example 3: 130/30 Long-Short Strategy

```cpp
#include "orbat/optimizer/constraint.hpp"

using orbat::optimizer::ConstraintSet;
using orbat::optimizer::FullyInvestedConstraint;
using orbat::optimizer::BoxConstraint;

// 130/30: 130% long, 30% short, net 100% invested
ConstraintSet constraints;
constraints.add(std::make_shared<FullyInvestedConstraint>());
constraints.add(std::make_shared<BoxConstraint>(-0.3, 1.0));  // Allow up to 30% short

// Example weights (sums to 1.0)
Vector weights({0.5, 0.4, -0.2, 0.3});  // 130% long, 20% short
bool feasible = constraints.isFeasible(weights);  // true
```

## Integration with Optimizers

Constraints are designed to be consumed uniformly by optimization algorithms:

```cpp
// Pseudocode for optimizer integration
class PortfolioOptimizer {
public:
    void setConstraints(const ConstraintSet& constraints) {
        constraints_ = constraints;
    }
    
    Vector optimize(const Vector& expectedReturns, 
                   const Matrix& covariance) {
        // Pre-optimization feasibility check
        if (constraints_.hasInfeasibleCombination(expectedReturns.size())) {
            throw std::runtime_error("Infeasible constraints");
        }
        
        // ... optimization logic ...
        
        // Post-optimization validation
        if (!constraints_.isFeasible(weights)) {
            throw std::runtime_error("Optimization violated constraints");
        }
        
        return weights;
    }
    
private:
    ConstraintSet constraints_;
};
```

## Best Practices

1. **Always validate constraints before optimization:**
   ```cpp
   if (constraints.hasInfeasibleCombination(numAssets)) {
       // Handle infeasible case
   }
   ```

2. **Use appropriate tolerance for numerical stability:**
   ```cpp
   FullyInvestedConstraint fullyInvested(1e-6);  // Looser tolerance
   ```

3. **Check constraint satisfaction post-optimization:**
   ```cpp
   Vector optimizedWeights = optimizer.optimize(...);
   if (!constraints.isFeasible(optimizedWeights)) {
       // Handle constraint violation
   }
   ```

4. **Document constraint rationale:**
   ```cpp
   // CFA Institute recommends limiting single-stock exposure
   // to manage concentration risk
   BoxConstraint box(0.0, 0.1);  // Max 10% per stock
   ```

## CFA Institute Guidelines

The constraint system aligns with CFA Institute best practices:

- **Diversification:** Box constraints limit concentration risk
- **Risk Management:** Long-only and fully invested constraints enforce portfolio structure
- **Transparency:** All constraints are explicitly defined and validated
- **Regulatory Compliance:** Flexible constraint system accommodates various regulatory requirements

## Further Reading

- CFA Institute: *Portfolio Management* (CFA Program Curriculum)
- Modern Portfolio Theory and Investment Analysis
- Markowitz, H. (1952). "Portfolio Selection"

## API Reference

For detailed API documentation, see the header file:
- [`include/orbat/optimizer/constraint.hpp`](../include/orbat/optimizer/constraint.hpp)

For test examples, see:
- [`tests/unit/test_constraint.cpp`](../tests/unit/test_constraint.cpp)
