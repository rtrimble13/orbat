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

## Building

*Build instructions will be added as the project develops.*

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
