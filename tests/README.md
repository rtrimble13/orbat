# Tests

This directory contains unit tests and integration tests for the orbat library.

## Testing Framework

The project uses a modern C++ testing framework (e.g., Google Test, Catch2) for unit testing.

## Structure

```
tests/
├── unit/           # Unit tests for individual components
├── integration/    # Integration tests for combined functionality
├── data/           # Test data files
└── CMakeLists.txt  # Test build configuration
```

## Running Tests

```bash
# Build tests
cmake --build build --target tests

# Run all tests using CTest
cd build && ctest

# Run tests with verbose output
cd build && ctest --verbose

# Run a specific test
./build/tests/unit/portfolio_test
```

## Writing Tests

### Test File Organization

- One test file per source file or component
- Name test files with `_test` suffix: `portfolio_test.cpp`
- Group related tests in test suites

### Test Structure

Follow the Arrange-Act-Assert (AAA) pattern:

```cpp
#include "orbat/portfolio.hpp"
#include <gtest/gtest.h>

TEST(PortfolioTest, ConstructorCreatesEmptyPortfolio) {
    // Arrange & Act
    orbat::Portfolio portfolio;
    
    // Assert
    EXPECT_TRUE(portfolio.isEmpty());
    EXPECT_EQ(portfolio.size(), 0);
}

TEST(PortfolioTest, SetWeightsValidatesSum) {
    // Arrange
    orbat::Portfolio portfolio;
    std::vector<double> weights = {0.3, 0.3, 0.3};  // Sum = 0.9
    
    // Act & Assert
    EXPECT_THROW(portfolio.setWeights(weights), std::invalid_argument);
}
```

### Test Coverage

Aim to test:
- **Normal operation** - Expected use cases
- **Edge cases** - Boundary conditions
- **Error conditions** - Invalid inputs
- **Special values** - Empty containers, zeros, negatives, infinities
- **Preconditions** - Input validation
- **Postconditions** - Output correctness

### Test Naming

Use descriptive test names that explain what is being tested:

```cpp
TEST(ClassName, MethodName_Scenario_ExpectedBehavior)
TEST(Portfolio, SetWeights_WithNegativeValues_ThrowsException)
TEST(Optimizer, Optimize_WithValidData_ReturnsValidWeights)
```

## Test Categories

### Unit Tests

Test individual functions and classes in isolation:
- Single class or function
- Mock external dependencies
- Fast execution
- No I/O operations

### Integration Tests

Test interactions between components:
- Multiple classes working together
- Real dependencies (not mocked)
- End-to-end workflows
- May involve I/O

### Performance Tests

Test performance characteristics:
- Execution time
- Memory usage
- Scalability
- Placed in `../benchmarks/` directory

## Test Data

- Place test data files in `tests/data/`
- Use small, synthetic datasets when possible
- Document data file formats
- Keep test data minimal but representative

## Best Practices

### Do's

- Write tests first (TDD) or alongside implementation
- Keep tests simple and focused
- Test one thing per test
- Use descriptive names
- Make tests independent
- Clean up resources in tests

### Don'ts

- Don't test implementation details
- Don't make tests dependent on each other
- Don't skip error case testing
- Don't use production data in tests
- Don't commit failing tests

## Continuous Integration

Tests run automatically on:
- Every pull request
- Every commit to main branch
- Scheduled nightly builds

All tests must pass before code can be merged.

## Code Coverage

We aim for high test coverage:
- Minimum 80% line coverage
- 100% coverage for critical paths
- Coverage reports generated in CI

Check coverage locally:
```bash
# Build with coverage instrumentation
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build build

# Run tests
cd build && ctest

# Generate coverage report
gcov ... # or llvm-cov, lcov, etc.
```

## Debugging Tests

```bash
# Run test under debugger
gdb ./build/tests/unit/portfolio_test

# Run test with valgrind
valgrind --leak-check=full ./build/tests/unit/portfolio_test

# Run specific test case
./build/tests/unit/portfolio_test --gtest_filter=PortfolioTest.SetWeights*
```

## Adding New Tests

When adding a feature:
1. Write test cases for the new functionality
2. Add test file in appropriate directory
3. Update CMakeLists.txt if needed
4. Run tests locally before committing
5. Ensure all tests pass in CI

## Test Utilities

Common test utilities and fixtures can be shared:
- Create `test_utils.hpp` for shared helpers
- Create base test fixtures for common setup
- Mock classes for external dependencies
