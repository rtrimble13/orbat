# Python Integration

This directory contains Python bindings and visualization tools for the orbat library.

## Purpose

The Python integration provides:
- **Python bindings** - Use orbat from Python using pybind11 or similar
- **Visualization tools** - Plot efficient frontiers, portfolio compositions, etc.
- **Data processing** - Helper scripts for preparing financial data
- **Interactive analysis** - Jupyter notebook integration

## Structure

```
python/
└── orbat_viz/
    ├── __init__.py       # Package initialization
    ├── bindings.py       # Python bindings to C++ library
    ├── plotting.py       # Visualization functions
    ├── utils.py          # Utility functions
    └── examples/         # Example Python scripts and notebooks
```

## Installation

*Instructions will be added as Python integration is implemented.*

Example installation:
```bash
# Install in development mode
pip install -e python/

# Or build wheel
python setup.py bdist_wheel
pip install dist/orbat_viz-*.whl
```

## Usage

Example Python usage:
```python
import orbat_viz as ov
import numpy as np

# Load data
returns = ov.load_returns('data.csv')

# Optimize portfolio
optimizer = ov.MeanVarianceOptimizer()
weights = optimizer.optimize(returns, target_return=0.10)

# Visualize results
ov.plot_efficient_frontier(returns)
ov.plot_portfolio_composition(weights)
```

## Visualization Features

Planned visualization capabilities:
- Efficient frontier plots
- Portfolio composition pie charts
- Risk-return scatter plots
- Asset correlation heatmaps
- Performance over time
- Rolling statistics

## Development

### Setting Up Python Development Environment

```bash
# Create virtual environment
python -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate

# Install dependencies
pip install -r requirements.txt

# Install in development mode
pip install -e python/
```

### Running Tests

```bash
# Run Python tests
pytest python/tests/

# Run with coverage
pytest --cov=orbat_viz python/tests/
```

## Dependencies

Python dependencies will include:
- `numpy` - Numerical operations
- `pandas` - Data manipulation
- `matplotlib` - Plotting
- `seaborn` - Statistical visualization
- `pybind11` - C++ bindings (if used)

## Contributing

When adding Python code:
- Follow PEP 8 style guidelines
- Use type hints
- Write docstrings for all public functions
- Add unit tests
- Update documentation

## Future Enhancements

Potential additions:
- Jupyter notebook tutorials
- Interactive dashboards with Plotly or Bokeh
- Integration with financial data APIs
- Portfolio backtesting tools
- Risk analytics
