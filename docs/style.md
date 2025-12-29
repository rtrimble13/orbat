# C++ Style Guide for orbat

This document defines the coding style and conventions for the orbat project. All code contributions must adhere to these guidelines.

## Language Standard

- **C++20** is the minimum required standard
- Use modern C++ features when appropriate
- Avoid deprecated features from older standards

## Naming Conventions

### Classes and Structs

Use **PascalCase** for class and struct names:

```cpp
class Portfolio { };
class MeanVarianceOptimizer { };
struct OptimizationResult { };
```

### Functions and Methods

Use **camelCase** for function and method names:

```cpp
double calculateReturn(const std::vector<double>& weights);
void optimizePortfolio();
bool isValid() const;
```

### Variables

Use **camelCase** for variable names:

```cpp
double expectedReturn = 0.0;
std::vector<double> assetWeights;
int numAssets = 10;
```

### Private Member Variables

Use **camelCase with trailing underscore** for private member variables:

```cpp
class Portfolio {
private:
    std::vector<double> weights_;
    double totalReturn_;
    int numAssets_;
};
```

### Constants

Use **UPPER_SNAKE_CASE** for constants:

```cpp
constexpr double PI = 3.14159265358979323846;
constexpr int MAX_ITERATIONS = 1000;
const std::string DEFAULT_OUTPUT_FORMAT = "csv";
```

### Namespaces

Use **lowercase** for namespace names:

```cpp
namespace orbat {
namespace optimization {
// ...
}  // namespace optimization
}  // namespace orbat
```

## Formatting Rules

### Indentation

- Use **4 spaces** for indentation (no tabs)
- Continuation lines should be indented by 4 spaces

```cpp
void longFunctionName(
    int parameter1,
    double parameter2,
    const std::string& parameter3) {
    // Function body
}
```

### Line Length

- Maximum line length is **100 characters**
- Break long lines at logical points
- Break before operators when splitting expressions

```cpp
// Good
double result = calculateComplexValue(parameter1, parameter2) +
                calculateAnotherValue(parameter3);

// Avoid exceeding 100 characters
```

### Braces

Use **K&R brace style** (opening brace on same line):

```cpp
// Classes
class Portfolio {
public:
    Portfolio();
};

// Functions
void function() {
    if (condition) {
        // ...
    } else {
        // ...
    }
}

// Control flow
for (int i = 0; i < n; ++i) {
    // ...
}

while (condition) {
    // ...
}
```

### Spaces

- Space after keywords: `if (`, `for (`, `while (`
- Space around binary operators: `a + b`, `x == y`
- No space after unary operators: `!flag`, `++i`
- No space inside parentheses: `function(arg)` not `function( arg )`
- Two spaces before trailing comments: `int x = 5;  // comment`

```cpp
// Good
if (x > 0) {
    result = a + b;
}

// Bad
if(x>0){
    result=a+b;
}
```

## Code Organization

### Header Files

#### Header Guards

Use `#pragma once` at the top of all header files:

```cpp
#pragma once

namespace orbat {
// ...
}  // namespace orbat
```

#### Include Order

Order includes as follows:

1. Related header (for .cpp files)
2. Project headers (`"orbat/..."`)
3. Third-party library headers (`<boost/...>`)
4. Standard library headers (`<vector>`, `<string>`)

```cpp
#include "orbat/portfolio.hpp"  // Related header

#include "orbat/optimizer.hpp"  // Other project headers
#include "orbat/utils.hpp"

#include <boost/numeric/ublas/matrix.hpp>  // Third-party

#include <algorithm>  // Standard library
#include <vector>
```

### Class Layout

Order class members as follows:

1. Public members
2. Protected members
3. Private members

Within each section, order by:

1. Types and nested classes
2. Static constants
3. Constructors and destructor
4. Methods
5. Member variables

```cpp
class Example {
public:
    // Types
    using ValueType = double;
    
    // Constants
    static constexpr int DEFAULT_SIZE = 100;
    
    // Constructors
    Example();
    explicit Example(int size);
    ~Example();
    
    // Methods
    void process();
    double getValue() const;

private:
    // Member variables
    int size_;
    std::vector<double> data_;
};
```

## Best Practices

### Modern C++ Features

- Use `auto` when type is obvious or improves readability
- Use range-based for loops when appropriate
- Use smart pointers (`std::unique_ptr`, `std::shared_ptr`) instead of raw pointers
- Use `nullptr` instead of `NULL` or `0`
- Use `enum class` instead of plain `enum`

```cpp
// Good
auto data = getData();
for (const auto& item : container) {
    // ...
}
auto ptr = std::make_unique<Portfolio>();

// Avoid
Portfolio* ptr = new Portfolio();  // Use smart pointers
```

### Const Correctness

- Mark methods that don't modify state as `const`
- Use `const` for parameters that shouldn't be modified
- Use `const` references for large objects passed as parameters

```cpp
class Portfolio {
public:
    double getReturn() const;  // Doesn't modify state
    void setWeights(const std::vector<double>& weights);  // Const reference
    
private:
    std::vector<double> weights_;
};
```

### Comments

- Use `//` for single-line comments
- Use `/* */` for multi-line comments
- Write self-documenting code; add comments for complex logic
- Document public APIs with clear descriptions
- Explain "why" not "what" in comments

```cpp
// Calculate the expected return using historical data
double calculateExpectedReturn(const std::vector<double>& returns) {
    // Use arithmetic mean for simplicity
    // More sophisticated methods (geometric mean, etc.) could be added
    return std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
}
```

### Error Handling

- Use exceptions for error handling in library code
- Document exceptions that can be thrown
- Use standard exception types when appropriate
- Validate inputs and preconditions

```cpp
void Portfolio::setWeights(const std::vector<double>& weights) {
    if (weights.empty()) {
        throw std::invalid_argument("Weights vector cannot be empty");
    }
    
    double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
    if (std::abs(sum - 1.0) > 1e-6) {
        throw std::invalid_argument("Weights must sum to 1.0");
    }
    
    weights_ = weights;
}
```

### Resource Management

- Follow RAII (Resource Acquisition Is Initialization)
- Use smart pointers for dynamic memory
- Prefer stack allocation when possible
- Use standard containers instead of manual memory management

```cpp
// Good - RAII
class ResourceHandler {
public:
    ResourceHandler() : resource_(acquireResource()) { }
    ~ResourceHandler() { releaseResource(resource_); }
    
private:
    Resource resource_;
};

// Good - smart pointer
auto data = std::make_unique<DataBuffer>(size);
```

## Documentation

### API Documentation

Document public APIs with clear comments explaining:

- Purpose and behavior
- Parameters and return values
- Preconditions and postconditions
- Exceptions that may be thrown
- Example usage when helpful

```cpp
/**
 * Optimize portfolio weights using mean-variance optimization.
 * 
 * @param returns Historical return data for each asset
 * @param targetReturn Desired portfolio return
 * @return Optimal portfolio weights (sum to 1.0)
 * @throws std::invalid_argument if returns is empty or targetReturn is invalid
 * 
 * Example:
 *   std::vector<std::vector<double>> returns = loadHistoricalData();
 *   auto weights = optimizePortfolio(returns, 0.08);
 */
std::vector<double> optimizePortfolio(
    const std::vector<std::vector<double>>& returns,
    double targetReturn);
```

## Automatic Formatting

The project uses clang-format for automatic code formatting. Configuration is in `.clang-format`.

### Format Code

```bash
# Format all C++ files
./scripts/format.sh

# Check formatting without modifying files
./scripts/check-format.sh
```

### Editor Integration

Most modern editors support clang-format integration. Configure your editor to format on save for the best experience.

## Testing

- Write unit tests for all new functionality
- Test edge cases and error conditions
- Use descriptive test names
- Follow the Arrange-Act-Assert pattern

```cpp
TEST(PortfolioTest, WeightsMustSumToOne) {
    // Arrange
    Portfolio portfolio;
    std::vector<double> invalidWeights = {0.3, 0.3, 0.3};  // Sum = 0.9
    
    // Act & Assert
    EXPECT_THROW(portfolio.setWeights(invalidWeights), std::invalid_argument);
}
```

## Continuous Improvement

This style guide may evolve as the project grows. Suggestions for improvements are welcome through the standard contribution process.
