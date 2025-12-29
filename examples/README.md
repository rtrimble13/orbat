# Examples

This directory contains example programs demonstrating how to use the orbat library.

## Purpose

Examples help users:
- Learn how to use the library
- Understand common usage patterns
- See working implementations of features
- Get started quickly with their own projects

## Structure

Examples will be organized by topic:
- **Basic Usage** - Simple examples to get started
- **Portfolio Optimization** - Various optimization strategies
- **Data Handling** - Loading and processing financial data
- **Advanced Features** - Complex scenarios and advanced usage

## Building Examples

*Instructions will be added as examples are implemented.*

Example build process:
```bash
# Build all examples
cmake --build build --target examples

# Run an example
./build/examples/basic_optimization
```

## Available Examples

Examples will be added as the library develops. Planned examples include:

- `basic_optimization.cpp` - Simple mean-variance optimization
- `custom_constraints.cpp` - Portfolio with custom constraints
- `efficient_frontier.cpp` - Computing the efficient frontier
- `risk_parity.cpp` - Risk parity portfolio construction
- `black_litterman.cpp` - Black-Litterman model example

## Using Examples

Each example is a standalone program that can be:
- Compiled and run independently
- Used as a template for your own code
- Modified to experiment with different parameters

## Adding Examples

When adding a new example:
1. Create a `.cpp` file with a descriptive name
2. Include comprehensive comments explaining the code
3. Show input data preparation
4. Demonstrate core functionality
5. Display results in a clear format
6. Handle errors gracefully
7. Keep examples focused on one concept

## Example Template

```cpp
#include "orbat/portfolio.hpp"
#include <iostream>
#include <vector>

int main() {
    // 1. Prepare input data
    // ...

    // 2. Create and configure optimizer
    // ...

    // 3. Run optimization
    // ...

    // 4. Display results
    // ...

    return 0;
}
```
