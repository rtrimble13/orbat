# orbat

A modern C++ library and command-line tool for investment portfolio optimization.

## Overview

orbat provides high-performance portfolio optimization capabilities using modern C++20. The project aims to deliver efficient implementations of various portfolio optimization techniques including mean-variance optimization, risk parity, and other advanced methods.

## Project Structure

```
orbat/
├── .github/          # GitHub Actions workflows and configurations
├── .vscode/          # VS Code editor settings
├── benchmarks/       # Performance benchmarks
├── docs/             # Documentation and guides
├── examples/         # Example usage and tutorials
├── include/orbat/    # Public header files
├── python/           # Python bindings and visualization tools
├── scripts/          # Utility scripts for development
├── src/              # Implementation files
└── tests/            # Unit and integration tests
```

## Features

- **Linear Algebra Primitives**: Lightweight Vector and Matrix classes with no external dependencies
  - Vector operations: dot products, norms, arithmetic
  - Matrix operations: multiplication, transpose, Cholesky decomposition, inversion
  - Optimized for positive-definite covariance matrices
  - Comprehensive bounds checking in debug mode

- **Portfolio Constraints**: Modular constraint system for real-world portfolio optimization
  - Fully invested constraint (weights sum to 1.0)
  - Long-only constraint (no short positions)
  - Box constraints (per-asset position limits)
  - Constraint composition and feasibility validation
  - Consistent with CFA Institute best practices

- **Markowitz Portfolio Optimization**: Classic mean-variance optimization
  - Minimum variance portfolios
  - Target return optimization
  - Risk aversion parameter optimization
  - Support for portfolio constraints
  - Standardized output with Sharpe ratio
  - JSON and CSV serialization for results

- **Black-Litterman Model**: Bayesian portfolio optimization combining equilibrium and views
  - Implied equilibrium returns from market capitalization weights
  - Flexible view specification (absolute and relative)
  - View confidence scaling
  - Smooth blending of equilibrium and investor beliefs
  - Consistent with academic Black-Litterman formulation

- **Efficient Frontier**: Generate and export efficient portfolios for analysis
  - Generate N frontier portfolios with varying risk-return profiles
  - Export to CSV format (for Python, R, Excel)
  - Export to JSON format (for web visualization)
  - Guaranteed convexity and monotonicity properties
  - Stable across different input parameters

See [Linear Algebra Documentation](docs/linear_algebra.md) for detailed API reference and usage examples.

See [Portfolio Constraints Documentation](docs/constraints.md) for constraint system usage and best practices.

See [Markowitz Optimization Documentation](docs/markowitz.md) for portfolio optimization guide.

See [Black-Litterman Model Documentation](docs/black_litterman.md) for Bayesian portfolio optimization guide.

See [Efficient Frontier Documentation](docs/efficient_frontier.md) for frontier generation and visualization.

## Building

### Prerequisites

- C++20 compatible compiler (GCC 10+, Clang 10+, MSVC 2019+)
- CMake 3.14 or higher
- Git

### Build Steps

```bash
# Configure the build
cmake -S . -B build

# Build the project
cmake --build build

# Run tests
cd build && ctest

# Build with examples
cmake -S . -B build -DBUILD_EXAMPLES=ON
cmake --build build
./build/examples/linear_algebra_demo
```

### Build Options

- `BUILD_TESTS` - Build unit tests (default: ON)
- `BUILD_EXAMPLES` - Build example programs (default: OFF)
- `BUILD_CLI` - Build command-line interface (default: OFF)

Example:
```bash
cmake -S . -B build -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
```

## Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on:

- Setting up your development environment
- Coding standards and style guide
- Testing requirements
- Pull request process

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Development Status

This project is in early development. Features and APIs are subject to change.
