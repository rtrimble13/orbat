# Linear Algebra Module

The `orbat::core` module provides lightweight, dependency-free linear algebra primitives for portfolio optimization.

## Design Decision

**Custom Implementation vs External Libraries**

After evaluating options (BLAS, LAPACK, Eigen, etc.), we chose a custom implementation for:

- **Minimal dependencies**: No external library requirements
- **Full control**: Optimize specifically for portfolio calculations
- **Transparency**: Easy to understand and debug
- **Portability**: Works everywhere with a C++20 compiler

The implementation focuses on numerical stability for the specific operations needed in portfolio optimization:
- Covariance matrix operations (always symmetric positive-definite)
- Portfolio variance/risk calculations
- Cholesky decomposition for matrix inversion

## Components

### Vector Class

`orbat::core::Vector` - A lightweight 1D vector class.

**Features:**
- Construction from size, initializer lists, or `std::vector`
- Element access with bounds checking (debug mode via `assert`, runtime via `.at()`)
- Dot products
- Vector norms (L2/Euclidean)
- Element-wise operations (addition, subtraction, scalar multiplication/division)
- In-place operations

**Example:**
```cpp
#include "orbat/core/vector.hpp"

using orbat::core::Vector;

// Create vectors
Vector returns({0.10, 0.12, 0.15});
Vector weights({0.4, 0.3, 0.3});

// Compute dot product
double expectedReturn = returns.dot(weights);  // 0.119

// Vector operations
Vector scaled = returns * 2.0;
double norm = returns.norm();
double sum = returns.sum();
```

### Matrix Class

`orbat::core::Matrix` - A lightweight 2D matrix class (row-major storage).

**Features:**
- Construction from dimensions or 2D initializer lists
- Element access with bounds checking
- Matrix multiplication (matrix-matrix and matrix-vector)
- Transpose
- Row/column access and modification
- Arithmetic operations (addition, subtraction, scalar operations)
- **Cholesky decomposition** for symmetric positive-definite matrices
- **Matrix inversion** via Cholesky (for covariance matrices)
- Triangular system solvers (forward/backward substitution)

**Example:**
```cpp
#include "orbat/core/matrix.hpp"

using orbat::core::Matrix;
using orbat::core::Vector;

// Create a covariance matrix
Matrix cov({{0.04, 0.01, 0.005},
            {0.01, 0.0225, 0.008},
            {0.005, 0.008, 0.01}});

// Compute inverse (for optimization)
Matrix invCov = cov.inverse();

// Portfolio variance: w^T * Cov * w
Vector weights({0.5, 0.3, 0.2});
double variance = weights.dot(cov * weights);
double stdDev = std::sqrt(variance);
```

## Numerical Stability

### Cholesky Decomposition

For symmetric positive-definite matrices (like covariance matrices), we use Cholesky decomposition:

```
A = L * L^T
```

Where `L` is lower triangular. This is:
- More numerically stable than general LU decomposition
- Faster (roughly half the operations)
- Guaranteed to work for positive-definite matrices
- Exploits the structure of covariance matrices

### Matrix Inversion

Rather than computing the inverse directly, we solve:

```
A * A^(-1) = I
```

By solving `n` triangular systems (one for each column of the identity matrix):

```
L * y = e_i    (forward substitution)
L^T * x = y    (backward substitution)
```

This is more stable than computing the inverse directly and is the recommended approach for positive-definite systems.

### Bounds Checking

- **Debug mode**: Uses `assert()` for zero-overhead checking in release builds
- **Runtime checking**: Use `.at()` methods for runtime bounds checking with exceptions

## Performance Characteristics

| Operation | Complexity | Notes |
|-----------|-----------|-------|
| Vector dot product | O(n) | Linear scan |
| Matrix multiplication | O(n³) | Standard triple loop |
| Transpose | O(n²) | Copy with swapped indices |
| Cholesky decomposition | O(n³/3) | Half the work of LU |
| Matrix inversion | O(n³) | Via Cholesky + triangular solves |

For small to medium-sized matrices (typical in portfolio optimization with 10-1000 assets), this implementation is sufficient. For very large matrices or high-frequency calculations, consider profiling and potentially switching to optimized BLAS libraries.

## Usage in Portfolio Optimization

### Expected Return Calculation

```cpp
Vector assetReturns({0.08, 0.10, 0.12});
Vector weights({0.4, 0.3, 0.3});
double portfolioReturn = assetReturns.dot(weights);
```

### Portfolio Variance

```cpp
Matrix covariance = computeCovariance(returns);
Vector weights({0.4, 0.3, 0.3});

// Variance = w^T * Cov * w
double variance = weights.dot(covariance * weights);
double risk = std::sqrt(variance);
```

### Mean-Variance Optimization Setup

```cpp
// For quadratic programming: minimize (1/2) * w^T * Cov * w
Matrix cov = computeCovariance(returns);
Matrix invCov = cov.inverse();

// Can now use invCov in closed-form solutions or optimization
```

## Testing

Comprehensive unit tests cover:

- **Correctness**: Matrix multiplication, transpose, etc.
- **Edge cases**: Empty vectors, identity matrices, etc.
- **Error handling**: Size mismatches, invalid operations
- **Numerical stability**: Small/large values, covariance inversion
- **Positive-definite matrices**: Realistic covariance matrices

Run tests:
```bash
cd build
ctest --output-on-failure
```

## Thread Safety

The `Vector` and `Matrix` classes are **not thread-safe** for concurrent modifications. However:

- Multiple threads can safely read from the same vector/matrix
- Each thread should have its own instances for modifications
- Standard copy semantics apply (deep copies are made)

## Future Improvements

Potential enhancements if profiling shows bottlenecks:

1. **SIMD vectorization** for dot products and matrix operations
2. **Blocked matrix multiplication** for better cache utilization
3. **OpenMP parallelization** for large matrix operations
4. **BLAS/LAPACK integration** as an optional backend
5. **Sparse matrix support** for large-scale problems

The current implementation provides a solid foundation that can be optimized as needed.

## References

- **Cholesky decomposition**: Golub & Van Loan, "Matrix Computations", 4th ed.
- **Numerical stability**: Higham, "Accuracy and Stability of Numerical Algorithms", 2nd ed.
- **Portfolio optimization**: Markowitz, "Portfolio Selection", Journal of Finance, 1952

## See Also

- [Example: Linear Algebra Demo](../examples/linear_algebra_demo.cpp)
- [Vector API Reference](../include/orbat/core/vector.hpp)
- [Matrix API Reference](../include/orbat/core/matrix.hpp)
