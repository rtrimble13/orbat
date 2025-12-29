#!/bin/bash
# Format all C++ files in the project using clang-format
# This script recursively finds and formats all C++ source and header files

set -e

# Change to repository root
cd "$(dirname "$0")/.."

echo "Formatting C++ files..."

# Find and format all C++, C, and header files
find include src tests examples -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.cc" -o -name "*.cxx" \) -exec clang-format -i {} \;

echo "Formatting complete!"
