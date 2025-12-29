# Include Directory

This directory contains the public header files for the orbat library.

## Structure

```
include/
└── orbat/
    ├── core/           # Core data structures and interfaces
    ├── optimization/   # Optimization algorithms
    ├── utils/          # Utility functions and helpers
    └── orbat.hpp       # Main header including all subcomponents
```

## Usage

Users of the orbat library should include headers from this directory:

```cpp
#include "orbat/portfolio.hpp"
#include "orbat/optimization/mean_variance.hpp"
```

## Guidelines

### What Goes Here

- **Public API only** - Headers that are part of the library's public interface
- **Interface definitions** - Class declarations, function prototypes
- **Template implementations** - Full template code (since it must be in headers)
- **Inline functions** - Small functions marked `inline`

### What Doesn't Go Here

- **Private implementation details** - These go in `src/` directory
- **Internal utilities** - Non-public helper code
- **Test code** - Tests go in `tests/` directory

### Header File Best Practices

1. **Use `#pragma once`** at the top of every header file
2. **Include guards** - `#pragma once` is preferred over traditional guards
3. **Forward declarations** - Use when possible to reduce dependencies
4. **Minimal includes** - Only include what's necessary in the header
5. **Documentation** - Document all public APIs with clear comments
6. **Namespace** - All code should be in the `orbat` namespace

### Example Header Structure

```cpp
#pragma once

#include <vector>

namespace orbat {

/**
 * @brief Represents a portfolio of assets.
 * 
 * Detailed description of the class...
 */
class Portfolio {
public:
    Portfolio();
    ~Portfolio();
    
    // Public methods...
    
private:
    // Private members...
};

}  // namespace orbat
```

## Namespace Organization

The `orbat` namespace contains:
- Top-level: Core types and commonly used classes
- `orbat::optimization`: Optimization algorithms
- `orbat::utils`: Utility functions and helpers
- Additional sub-namespaces as needed

## Include Dependencies

Headers should minimize dependencies:
- Standard library includes are fine
- Third-party library includes should be carefully considered
- Use forward declarations to avoid including other project headers when possible

## Stability

Public headers constitute the library's API contract:
- Changes should maintain backward compatibility when possible
- Breaking changes should be documented and versioned
- Deprecated features should be marked with deprecation warnings

## Documentation

All public APIs in headers should be documented:
- Brief description of purpose
- Parameter descriptions
- Return value documentation
- Exception specifications
- Usage examples when helpful
