# Benchmarks

This directory contains performance benchmarks for the orbat library.

## Purpose

Benchmarks help us:
- Measure and track performance of optimization algorithms
- Compare different implementations
- Identify performance regressions
- Optimize hot paths in the code

## Structure

Benchmarks will be organized by component:
- Portfolio optimization algorithms
- Matrix operations
- Statistical calculations
- Data loading and processing

## Running Benchmarks

*Instructions will be added as benchmarks are implemented.*

Example structure:
```bash
# Build benchmarks
cmake --build build --target benchmarks

# Run all benchmarks
./build/benchmarks/run_benchmarks

# Run specific benchmark
./build/benchmarks/portfolio_optimization_bench
```

## Adding Benchmarks

When adding a benchmark:
1. Create a new benchmark file in this directory
2. Use a consistent benchmarking framework (e.g., Google Benchmark)
3. Document what is being measured
4. Include multiple test cases with varying input sizes
5. Compare against baseline or reference implementations when applicable

## Interpreting Results

Benchmark results should include:
- Execution time (mean, median, standard deviation)
- Throughput (operations per second)
- Memory usage when relevant
- Comparison to previous versions

## Best Practices

- Run benchmarks on a quiet system (minimal background processes)
- Run multiple iterations to account for variability
- Use realistic input sizes and data
- Document hardware and compiler settings used
- Track results over time to identify regressions
