# Efficient Frontier

This document explains how to generate and export the efficient frontier for portfolio visualization and analysis.

## Overview

The efficient frontier represents the set of optimal portfolios that offer the highest expected return for each level of risk. This is a fundamental concept in modern portfolio theory and is essential for understanding the risk-return trade-off in portfolio construction.

## Theory

### What is the Efficient Frontier?

The efficient frontier is the boundary of the feasible investment opportunity set in the risk-return space. Portfolios on the efficient frontier are considered "efficient" because:

1. **Maximum return for given risk**: No other portfolio offers higher expected return for the same level of risk
2. **Minimum risk for given return**: No other portfolio has lower risk for the same expected return
3. **Convexity**: The frontier is convex in variance (risk squared), reflecting diversification benefits

### Properties of the Efficient Frontier

The efficient frontier has several important mathematical properties:

- **Monotonicity**: Expected return increases monotonically as you move along the frontier
- **Convexity**: Variance (risk squared) is a convex function of expected return
- **Starting point**: Begins at the minimum variance portfolio (leftmost point)
- **Ending point**: Approaches the maximum return asset (rightmost point)

## Usage

### Generating the Efficient Frontier

```cpp
#include "orbat/optimizer/markowitz.hpp"
#include "orbat/optimizer/efficient_frontier.hpp"

using namespace orbat::optimizer;

// Setup: define returns and covariance
ExpectedReturns returns({0.08, 0.12, 0.16});
CovarianceMatrix cov({
    {0.04, 0.01, 0.005},
    {0.01, 0.0225, 0.008},
    {0.005, 0.008, 0.01}
});

// Create optimizer
MarkowitzOptimizer optimizer(returns, cov);

// Generate efficient frontier with 50 portfolios
auto frontier = optimizer.efficientFrontier(50);

// Each portfolio in the frontier contains:
// - expectedReturn: portfolio expected return
// - risk: portfolio volatility (standard deviation)
// - weights: asset allocation weights
```

### Choosing the Number of Points

The number of points determines the granularity of the frontier:

- **10-20 points**: Quick visualization, coarse resolution
- **50 points**: Standard resolution for most applications (recommended)
- **100+ points**: High resolution for detailed analysis or smooth curves

```cpp
auto coarse_frontier = optimizer.efficientFrontier(10);    // Fast, less detail
auto standard_frontier = optimizer.efficientFrontier(50);  // Recommended
auto fine_frontier = optimizer.efficientFrontier(100);     // Smooth curves
```

### Exporting to CSV

Export the frontier to CSV format for use with spreadsheets, Python, R, and other tools:

```cpp
#include "orbat/optimizer/efficient_frontier.hpp"

// Export without asset labels
exportFrontierToCSV(frontier, "frontier.csv");

// Export with asset labels for better readability
std::vector<std::string> labels = {"Bonds", "Stocks", "Real Estate"};
exportFrontierToCSV(frontier, "frontier.csv", labels);
```

#### CSV Format

The exported CSV file has the following structure:

```csv
return,volatility,Bonds,Stocks,Real Estate
0.0975,0.0873,0.6771,0.2076,0.1152
0.0988,0.0874,0.6580,0.2140,0.1280
0.1001,0.0876,0.6389,0.2204,0.1407
...
```

- **Column 1**: Expected return (decimal, e.g., 0.0975 = 9.75%)
- **Column 2**: Volatility/risk (standard deviation)
- **Columns 3+**: Portfolio weights for each asset

### Exporting to JSON

Export to JSON format for web applications and JavaScript-based visualization:

```cpp
// Export without asset labels
exportFrontierToJSON(frontier, "frontier.json");

// Export with asset labels
std::vector<std::string> labels = {"Bonds", "Stocks", "Real Estate"};
exportFrontierToJSON(frontier, "frontier.json", labels);

// Or convert to JSON string for API responses
std::string jsonStr = frontierToJSONString(frontier, labels);
```

#### JSON Format

The exported JSON file has the following structure:

```json
{
  "assets": ["Bonds", "Stocks", "Real Estate"],
  "frontier": [
    {
      "return": 0.0975,
      "volatility": 0.0873,
      "weights": [0.6771, 0.2076, 0.1152]
    },
    {
      "return": 0.0988,
      "volatility": 0.0874,
      "weights": [0.6580, 0.2140, 0.1280]
    }
  ]
}
```

This format is compatible with modern JavaScript charting libraries like D3.js, Chart.js, and Plotly.

## Visualization

### Python (matplotlib)

```python
import pandas as pd
import matplotlib.pyplot as plt

# Load the exported CSV
df = pd.read_csv('frontier.csv')

# Plot the efficient frontier
plt.figure(figsize=(10, 6))
plt.plot(df['volatility'] * 100, df['return'] * 100, 'b-', linewidth=2)
plt.xlabel('Risk (% standard deviation)')
plt.ylabel('Expected Return (%)')
plt.title('Efficient Frontier')
plt.grid(True, alpha=0.3)
plt.show()

# Optionally add scatter plot of individual assets
assets = ['Bonds', 'Stocks', 'Real Estate']
for asset in assets:
    # Calculate asset risk and return from first row (if needed)
    pass
```

### Python (plotly - interactive)

```python
import pandas as pd
import plotly.graph_objects as go

df = pd.read_csv('frontier.csv')

fig = go.Figure()
fig.add_trace(go.Scatter(
    x=df['volatility'] * 100,
    y=df['return'] * 100,
    mode='lines+markers',
    name='Efficient Frontier',
    hovertemplate='<b>Risk</b>: %{x:.2f}%<br>' +
                  '<b>Return</b>: %{y:.2f}%<extra></extra>'
))

fig.update_layout(
    title='Efficient Frontier',
    xaxis_title='Risk (% standard deviation)',
    yaxis_title='Expected Return (%)',
    hovermode='closest'
)

fig.show()
```

### R (ggplot2)

```r
library(ggplot2)
library(readr)

# Load the frontier data
df <- read_csv('frontier.csv')

# Create the plot
ggplot(df, aes(x = volatility * 100, y = return * 100)) +
  geom_line(color = 'blue', size = 1) +
  geom_point(size = 2, alpha = 0.6) +
  labs(
    title = 'Efficient Frontier',
    x = 'Risk (% standard deviation)',
    y = 'Expected Return (%)'
  ) +
  theme_minimal()
```

### JavaScript (Chart.js)

```javascript
// Load JSON data
fetch('frontier.json')
  .then(response => response.json())
  .then(data => {
    const chartData = {
      datasets: [{
        label: 'Efficient Frontier',
        data: data.frontier.map(p => ({
          x: p.volatility * 100,
          y: p.return * 100
        })),
        borderColor: 'rgb(75, 192, 192)',
        backgroundColor: 'rgba(75, 192, 192, 0.2)',
        tension: 0.4
      }]
    };

    new Chart(document.getElementById('myChart'), {
      type: 'line',
      data: chartData,
      options: {
        scales: {
          x: { title: { display: true, text: 'Risk (%)' } },
          y: { title: { display: true, text: 'Return (%)' } }
        }
      }
    });
  });
```

## Advanced Usage

### With Constraints

Generate the efficient frontier with portfolio constraints:

```cpp
#include "orbat/optimizer/constraint.hpp"

// Create constraint set
ConstraintSet constraints;
constraints.add(std::make_shared<LongOnlyConstraint>());
constraints.add(std::make_shared<BoxConstraint>(0.0, 0.6)); // Max 60% per asset

// Create optimizer with constraints
MarkowitzOptimizer optimizer(returns, cov, constraints);

// Generate constrained frontier
auto frontier = optimizer.efficientFrontier(50);

// Export
exportFrontierToCSV(frontier, "constrained_frontier.csv");
```

### Analyzing Frontier Properties

```cpp
// Check that frontier is valid
for (size_t i = 1; i < frontier.size(); ++i) {
    // Returns should be non-decreasing
    assert(frontier[i].expectedReturn >= frontier[i-1].expectedReturn);
    
    // All portfolios should be fully invested
    assert(std::abs(frontier[i].weights.sum() - 1.0) < 1e-6);
}

// Find minimum variance portfolio
auto minVar = frontier.front();
std::cout << "Min Variance: Return = " << minVar.expectedReturn 
          << ", Risk = " << minVar.risk << std::endl;

// Find maximum return portfolio
auto maxReturn = frontier.back();
std::cout << "Max Return: Return = " << maxReturn.expectedReturn 
          << ", Risk = " << maxReturn.risk << std::endl;
```

### Locating Specific Portfolios

Find a portfolio with a target risk level:

```cpp
double targetRisk = 0.12;  // 12% volatility

// Find closest portfolio on frontier
auto it = std::min_element(frontier.begin(), frontier.end(),
    [targetRisk](const auto& a, const auto& b) {
        return std::abs(a.risk - targetRisk) < std::abs(b.risk - targetRisk);
    });

if (it != frontier.end()) {
    std::cout << "Portfolio with ~12% risk:" << std::endl;
    std::cout << "  Actual risk: " << it->risk << std::endl;
    std::cout << "  Expected return: " << it->expectedReturn << std::endl;
}
```

## Best Practices

### Input Validation

Always validate your inputs before generating the frontier:

```cpp
// 1. Check that covariance matrix is positive definite
if (!cov.isValid()) {
    std::cerr << "Invalid covariance matrix" << std::endl;
    return;
}

// 2. Ensure expected returns are reasonable
const auto& returnsData = returns.data().data();
double minReturn = *std::min_element(returnsData.begin(), returnsData.end());
double maxReturn = *std::max_element(returnsData.begin(), returnsData.end());
if (minReturn < -0.5 || maxReturn > 1.0) {
    std::cerr << "Warning: Returns seem unrealistic" << std::endl;
}

// 3. Generate the frontier
auto frontier = optimizer.efficientFrontier(50);
```

### Error Handling

```cpp
try {
    auto frontier = optimizer.efficientFrontier(50);
    exportFrontierToCSV(frontier, "output.csv");
} catch (const std::invalid_argument& e) {
    std::cerr << "Invalid argument: " << e.what() << std::endl;
} catch (const std::runtime_error& e) {
    std::cerr << "Runtime error: " << e.what() << std::endl;
}
```

### Performance Considerations

- **Number of points**: 50 points is a good balance between resolution and speed
- **Number of assets**: Performance scales roughly as O(n³) where n is the number of assets
- **Constraints**: Adding constraints increases computation time

## Example: Complete Workflow

See `examples/efficient_frontier_demo.cpp` for a complete example that demonstrates:

- Setting up the optimizer with realistic data
- Generating the efficient frontier
- Exporting to both CSV and JSON formats
- Validating frontier properties (monotonicity and convexity)
- Providing visualization instructions

Run the example:

```bash
cd build/examples
./efficient_frontier_demo
```

This will generate:
- `efficient_frontier.csv` - Ready for plotting
- `efficient_frontier.json` - Ready for web visualization

## Troubleshooting

### Empty Frontier

If the frontier is empty, check:
- Covariance matrix is positive definite
- Constraints are not infeasible
- Expected returns are within reasonable range

### Non-convex Frontier

If tests show the frontier is not convex:
- Check input data for errors
- Ensure covariance matrix is properly formed
- Verify numerical precision settings

### Export Failures

If export fails:
- Check file path and permissions
- Ensure frontier is not empty
- Verify disk space is available

## See Also

- [Markowitz Optimization](markowitz.md) - Core optimization theory and methods
- [Portfolio Constraints](constraints.md) - Adding realistic constraints
- [Examples](../examples/) - Complete working examples
