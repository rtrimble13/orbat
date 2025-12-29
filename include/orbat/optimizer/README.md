# Optimizer Module

The optimizer module provides standardized interfaces for portfolio optimization inputs, including expected returns and covariance matrices.

## Overview

Portfolio optimization requires two key inputs:
1. **Expected Returns**: The anticipated mean return for each asset
2. **Covariance Matrix**: The covariance between asset returns (capturing volatility and correlation)

This module provides robust, validated containers for these inputs with support for:
- Programmatic construction
- CSV file import
- JSON file import
- Comprehensive validation

## Classes

### ExpectedReturns

Container for expected returns of assets in a portfolio.

**Features:**
- Backed by `orbat::core::Vector`
- CSV import with automatic header detection
- JSON import (multiple formats supported)
- Asset labels/names support
- Validation: non-empty, finite values

**Example Usage:**

```cpp
#include "orbat/optimizer/expected_returns.hpp"

using orbat::optimizer::ExpectedReturns;

// Programmatic creation
ExpectedReturns returns({0.08, 0.12, 0.10});

// With labels
std::vector<std::string> labels = {"Stock A", "Stock B", "Stock C"};
ExpectedReturns returns_labeled(orbat::core::Vector({0.08, 0.12, 0.10}), labels);

// Load from CSV
ExpectedReturns returns_csv = ExpectedReturns::fromCSV("returns.csv");

// Load from JSON (with labels if provided)
ExpectedReturns returns_json = ExpectedReturns::fromJSON("returns.json");

// Access data
for (size_t i = 0; i < returns.size(); ++i) {
    std::cout << returns.getLabel(i) << ": " << returns[i] << std::endl;
}

// Set or modify labels
returns.setLabels({"AAPL", "GOOGL", "MSFT"});
```

**CSV Format:**
```csv
0.08
0.12
0.10
```

Or with header:
```csv
expected_return
0.08
0.12
0.10
```

**JSON Format:**

Simple array:
```json
[0.08, 0.12, 0.10]
```

Object with returns:
```json
{"returns": [0.08, 0.12, 0.10]}
```

Object with returns and labels:
```json
{"returns": [0.08, 0.12, 0.10], "labels": ["Stock A", "Stock B", "Stock C"]}
```

### CovarianceMatrix

Container for covariance matrix of asset returns.

**Features:**
- Backed by `orbat::core::Matrix`
- CSV import with automatic header detection
- JSON import (multiple formats supported)
- Asset labels/names support
- Validation: square, symmetric, positive diagonal, finite values
- Dimension matching utilities

**Example Usage:**

```cpp
#include "orbat/optimizer/covariance_matrix.hpp"

using orbat::optimizer::CovarianceMatrix;

// Programmatic creation
CovarianceMatrix cov({
    {0.04, 0.01, 0.005},
    {0.01, 0.0225, 0.008},
    {0.005, 0.008, 0.01}
});

// With labels
std::vector<std::string> labels = {"Stock A", "Stock B", "Stock C"};
CovarianceMatrix cov_labeled(
    orbat::core::Matrix({{0.04, 0.01}, {0.01, 0.0225}}),
    labels
);

// Load from CSV
CovarianceMatrix cov_csv = CovarianceMatrix::fromCSV("covariance.csv");

// Load from JSON (with labels if provided)
CovarianceMatrix cov_json = CovarianceMatrix::fromJSON("covariance.json");

// Validate dimensions match
if (cov.dimensionsMatch(returns.size())) {
    std::cout << "Ready for optimization!" << std::endl;
}

// Access data with labels
for (size_t i = 0; i < cov.size(); ++i) {
    std::cout << cov.getLabel(i) << ": ";
    for (size_t j = 0; j < cov.size(); ++j) {
        std::cout << cov(i, j) << " ";
    }
    std::cout << std::endl;
}
```

**CSV Format:**
```csv
0.04,0.01,0.005
0.01,0.0225,0.008
0.005,0.008,0.01
```

**JSON Format:**

Simple 2D array:
```json
[[0.04, 0.01, 0.005],
 [0.01, 0.0225, 0.008],
 [0.005, 0.008, 0.01]]
```

Object with covariance:
```json
{"covariance": [[0.04, 0.01], [0.01, 0.0225]]}
```

Object with covariance and labels:
```json
{
  "covariance": [[0.04, 0.01], [0.01, 0.0225]],
  "labels": ["Stock A", "Stock B"]
}
```

## Validation

Both classes perform comprehensive validation:

### ExpectedReturns Validation
- ✓ Non-empty data
- ✓ All values are finite (no NaN or infinity)

### CovarianceMatrix Validation
- ✓ Non-empty data
- ✓ Square matrix
- ✓ Symmetric (within numerical tolerance)
- ✓ Positive diagonal elements (variances must be positive)
- ✓ All values are finite (no NaN or infinity)

Invalid inputs throw `std::invalid_argument` with descriptive error messages.

## Error Handling

The module provides clear, actionable error messages:

```cpp
try {
    ExpectedReturns returns = ExpectedReturns::fromCSV("invalid.csv");
} catch (const std::runtime_error& e) {
    // File I/O errors: "Cannot open file: invalid.csv"
    // Parse errors: "Invalid numeric value in CSV: abc"
} catch (const std::invalid_argument& e) {
    // Validation errors: "Expected returns cannot be empty"
}

try {
    CovarianceMatrix cov({{0.04, 0.01}, {0.02, 0.0225}});
} catch (const std::invalid_argument& e) {
    // "Covariance matrix must be symmetric"
}
```

## Integration with Optimizers

These interfaces provide clean injection points for optimizer algorithms:

```cpp
#include "orbat/optimizer/expected_returns.hpp"
#include "orbat/optimizer/covariance_matrix.hpp"

// Load inputs
ExpectedReturns returns = ExpectedReturns::fromCSV("returns.csv");
CovarianceMatrix cov = CovarianceMatrix::fromCSV("covariance.csv");

// Validate dimensions
if (!cov.dimensionsMatch(returns.size())) {
    throw std::invalid_argument("Dimension mismatch between returns and covariance");
}

// Pass to optimizer (pseudo-code)
MeanVarianceOptimizer optimizer;
optimizer.setExpectedReturns(returns.data());
optimizer.setCovarianceMatrix(cov.data());
auto optimal_weights = optimizer.optimize();
```

## Examples

See `examples/cma_inputs_demo.cpp` for a comprehensive demonstration of all features.

To build and run the example:

```bash
cmake -S . -B build -DBUILD_EXAMPLES=ON
cmake --build build
./build/examples/cma_inputs_demo
```

## Testing

The module includes comprehensive unit tests covering:
- Valid CSV and JSON loading
- Malformed input rejection
- Validation edge cases
- Realistic portfolio data

Run tests:
```bash
cmake -S . -B build
cmake --build build
cd build && ctest -R "ExpectedReturnsTest|CovarianceMatrixTest"
```

## Design Decisions

1. **No external dependencies**: Simple custom parsers for CSV/JSON to avoid external dependencies
2. **Validation on construction**: Fail fast with clear error messages
3. **Const correctness**: Proper const accessors for safe data access
4. **Move semantics**: Support efficient data transfer
5. **Numerical tolerance**: Use `EPSILON` from `core/constants.hpp` for floating-point comparisons

## Future Enhancements

Potential improvements for future versions:
- Covariance matrix estimation from historical returns
- Correlation matrix conversion utilities
- Streaming/incremental loading for large datasets
