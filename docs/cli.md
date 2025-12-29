# Command-Line Interface (CLI)

The `orbat-cli` command-line tool provides a scriptable interface for portfolio optimization workflows.

## Building the CLI

The CLI is built separately from the library. Enable it with the `BUILD_CLI` option:

```bash
cmake -S . -B build -DBUILD_CLI=ON
cmake --build build
```

The executable will be located at `build/src/orbat-cli`.

## Installation

To install the CLI to your system:

```bash
cmake --install build
```

This will install the `orbat-cli` executable to the default installation prefix (typically `/usr/local/bin`).

## Usage

### General Help

```bash
orbat-cli --help
```

Output:
```
orbat - Portfolio Optimization Command Line Tool

Usage: orbat <command> [options]

Available Commands:
  mpt        Modern Portfolio Theory (Mean-Variance) optimization
  bl         Black-Litterman portfolio optimization

Options:
  --help, -h Show help for the command

Examples:
  orbat mpt --help
  orbat bl --help
  orbat mpt --returns returns.csv --covariance cov.csv
  orbat bl --returns market_weights.csv --covariance cov.csv
```

## Commands

### `mpt` - Modern Portfolio Theory

Performs mean-variance portfolio optimization using the Markowitz model.

#### Usage

```bash
orbat mpt --returns <file> --covariance <file> [options]
```

#### Required Flags

- `--returns <file>`: Path to CSV file containing expected returns
- `--covariance <file>`: Path to CSV file containing the covariance matrix

#### Optional Flags

- `--rf-rate <value>`: Risk-free rate for Sharpe ratio calculation (default: 0.0)
- `--constraints <file>`: Path to constraints file (not yet implemented)
- `--output <file>`: Output file for results in JSON format (default: stdout)
- `--help, -h`: Show help message

#### Input File Formats

**Returns CSV** (`returns.csv`):
```csv
# Expected returns for 3 assets
0.08
0.12
0.16
```

**Covariance CSV** (`cov.csv`):
```csv
# 3x3 covariance matrix
0.0100,0.0030,0.0020
0.0030,0.0225,0.0080
0.0020,0.0080,0.0400
```

- Comments starting with `#` are ignored
- Values can be comma-separated or one per line (for returns)

#### Examples

Basic usage:
```bash
orbat mpt --returns returns.csv --covariance cov.csv
```

With risk-free rate:
```bash
orbat mpt --returns returns.csv --covariance cov.csv --rf-rate 0.02
```

Save output to file:
```bash
orbat mpt --returns returns.csv --covariance cov.csv --output result.json
```

#### Output

**Console Output:**
```
=== Modern Portfolio Theory Optimization ===

Status: SUCCESS
Message: Minimum variance portfolio computed

Portfolio Metrics:
  Expected Return:  9.7524%
  Risk (Std Dev):   8.7320%
  Sharpe Ratio:     1.1169

Optimal Weights:
  Asset 1: 67.7143%
  Asset 2: 20.7619%
  Asset 3: 11.5238%
```

**JSON Output** (with `--output`):
```json
{
  "converged": true,
  "message": "Minimum variance portfolio computed",
  "expectedReturn": 0.09752381,
  "risk": 0.08731988,
  "sharpeRatio": 1.11685686,
  "weights": [0.67714286, 0.20761905, 0.11523810]
}
```

### `bl` - Black-Litterman

Performs Bayesian portfolio optimization using the Black-Litterman model.

#### Usage

```bash
orbat bl --returns <file> --covariance <file> [options]
```

#### Required Flags

- `--returns <file>`: Path to CSV file containing market equilibrium weights
- `--covariance <file>`: Path to CSV file containing the covariance matrix

#### Optional Flags

- `--rf-rate <value>`: Risk-free rate for Sharpe ratio calculation (default: 0.0)
- `--constraints <file>`: Path to constraints file (not yet implemented)
- `--output <file>`: Output file for results in JSON format (default: stdout)
- `--help, -h`: Show help message

#### Input File Formats

**Market Weights CSV** (`market_weights.csv`):
```csv
# Market capitalization weights for 3 assets
0.60
0.25
0.15
```

**Covariance CSV** (`cov.csv`):
```csv
# 3x3 covariance matrix
0.0400,0.0150,0.0080
0.0150,0.0625,0.0100
0.0080,0.0100,0.0100
```

#### Examples

Basic usage:
```bash
orbat bl --returns market_weights.csv --covariance cov.csv
```

Save output to file:
```bash
orbat bl --returns market_weights.csv --covariance cov.csv --output result.json
```

#### Output

**Console Output:**
```
=== Black-Litterman Portfolio Optimization ===

Status: SUCCESS
Message: Mean-variance portfolio computed

Implied Equilibrium Returns:
  Asset 1: 1.7625%
  Asset 2: 2.1562%
  Asset 3: 2.3000%

Portfolio Metrics:
  Expected Return:  2.1273%
  Risk (Std Dev):   11.4681%
  Sharpe Ratio:     0.1855

Optimal Weights:
  Asset 1: 19.5000%
  Asset 2: 47.2500%
  Asset 3: 33.2500%
```

**JSON Output** (with `--output`):
```json
{
  "converged": true,
  "message": "Mean-variance portfolio computed",
  "expectedReturn": 0.02127273,
  "risk": 0.11468095,
  "sharpeRatio": 0.18549669,
  "weights": [0.19500000, 0.47250000, 0.33250000]
}
```

## Error Handling

The CLI provides user-friendly error messages:

### Missing Required Flags
```bash
$ orbat mpt
Error: Missing required flag: --returns
Use 'orbat mpt --help' for usage information.
```

### Unknown Command
```bash
$ orbat unknown
Error: Unknown command 'unknown'
Use 'orbat --help' for available commands.
```

### Invalid Input Files
```bash
$ orbat mpt --returns missing.csv --covariance cov.csv
Error: Cannot open returns file: missing.csv
Use 'orbat mpt --help' for usage information.
```

## Exit Codes

- `0`: Success
- `1`: Error (invalid arguments, missing files, optimization failure, etc.)

## Scripting

The CLI is designed to be scriptable and can be easily integrated into automated workflows:

### Bash Script Example

```bash
#!/bin/bash
set -e

# Run portfolio optimization
orbat mpt \
  --returns data/returns.csv \
  --covariance data/cov.csv \
  --rf-rate 0.02 \
  --output results/portfolio_$(date +%Y%m%d).json

echo "Optimization complete!"
```

### Python Integration Example

```python
import subprocess
import json

# Run optimization
result = subprocess.run(
    ["orbat", "mpt",
     "--returns", "returns.csv",
     "--covariance", "cov.csv",
     "--output", "result.json"],
    capture_output=True,
    text=True,
    check=True
)

# Load results
with open("result.json") as f:
    portfolio = json.load(f)

print(f"Expected Return: {portfolio['expectedReturn']:.4f}")
print(f"Risk: {portfolio['risk']:.4f}")
print(f"Weights: {portfolio['weights']}")
```

## Future Enhancements

Planned features for future releases:

- **Custom Constraints**: Support for loading constraint definitions from files
- **View Specification**: Black-Litterman views from CSV/JSON files
- **Multiple Optimization Modes**: Target return, risk aversion parameter
- **Efficient Frontier**: Generate multiple portfolios along the efficient frontier
- **Batch Processing**: Process multiple input files in a single invocation
- **Validation Mode**: Validate input files without running optimization
- **Verbose Output**: Detailed logging and diagnostic information

## See Also

- [Markowitz Optimization Guide](markowitz.md)
- [Black-Litterman Model Guide](black_litterman.md)
- [Portfolio Constraints](constraints.md)
