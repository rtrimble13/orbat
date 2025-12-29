# Source Directory

This directory contains the implementation files for the orbat library.

## Structure

```
src/
├── core/           # Core data structures implementation
├── optimization/   # Optimization algorithms implementation
├── utils/          # Utility functions implementation
└── cli/            # Command-line interface implementation
```

## Organization

### Core Components

Implementation files that correspond to public headers in `include/orbat/`:
- Portfolio management
- Asset data structures
- Risk/return calculations

### Optimization Algorithms

Implementation of various portfolio optimization methods:
- Mean-variance optimization
- Risk parity
- Black-Litterman model
- Minimum variance
- Maximum Sharpe ratio

### Utilities

Helper functions and internal utilities:
- Matrix operations
- Statistical functions
- Data validation
- Numerical optimization helpers

### CLI

Command-line interface implementation:
- Argument parsing
- File I/O
- Output formatting
- User interaction

## Build Integration

Source files are compiled into:
- **Static library** (`liborbat.a` or `orbat.lib`)
- **CLI executable** (`orbat-cli`)

CMake configuration in the root directory handles the build process.

## Coding Standards

All implementation files must follow the standards in [../docs/style.md](../docs/style.md):
- Use 4-space indentation
- Follow naming conventions
- Keep functions focused and small
- Add comments for complex algorithms
- Use modern C++20 features appropriately

## File Naming

- Use `.cpp` extension for C++ source files
- Match header names: `portfolio.hpp` → `portfolio.cpp`
- Use lowercase with underscores for multi-word names

## Implementation Guidelines

### Separation of Concerns

- Keep implementation details out of public headers
- Use anonymous namespaces or `static` for internal functions
- Place complex implementations in source files, not headers

### Example Source File Structure

```cpp
#include "orbat/portfolio.hpp"

#include "orbat/utils/validation.hpp"

#include <algorithm>
#include <numeric>

namespace orbat {
namespace {

// Internal helper function
double calculateSum(const std::vector<double>& values) {
    return std::accumulate(values.begin(), values.end(), 0.0);
}

}  // anonymous namespace

// Public class method implementations
Portfolio::Portfolio() : weights_(), expectedReturn_(0.0) {
}

void Portfolio::setWeights(const std::vector<double>& weights) {
    validateWeights(weights);
    weights_ = weights;
}

}  // namespace orbat
```

### Performance Considerations

- Profile before optimizing
- Use appropriate data structures
- Consider cache locality
- Avoid unnecessary copies
- Use move semantics when beneficial

### Error Handling

- Validate inputs early
- Throw appropriate exceptions for errors
- Provide informative error messages
- Document exception guarantees

### Testing

- Write unit tests for all implementations
- Test edge cases and error conditions
- Ensure test coverage is comprehensive
- Tests go in `../tests/` directory

## Dependencies

- Prefer standard library when possible
- Document any third-party dependencies
- Keep dependencies minimal and justified
- Check licenses for compatibility

## Adding New Features

When adding a new feature:
1. Start with header file in `include/orbat/`
2. Add implementation file(s) here in `src/`
3. Update CMakeLists.txt to include new files
4. Add corresponding tests in `tests/`
5. Add usage example in `examples/`
6. Update documentation

## Build Artifacts

Compiled object files and intermediate build artifacts should not be committed. They are excluded via `.gitignore`.
