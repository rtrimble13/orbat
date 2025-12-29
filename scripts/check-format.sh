#!/bin/bash
# Check if all C++ files are properly formatted without modifying them
# This script is used in CI to verify code formatting

set -e

# Change to repository root
cd "$(dirname "$0")/.."

echo "Checking C++ file formatting..."

# Find and check all C++, C, and header files
# --dry-run: Don't modify files
# -Werror: Treat formatting issues as errors
find include src tests examples -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.cc" -o -name "*.cxx" \) -exec clang-format --dry-run -Werror {} \; 2>&1

if [ $? -eq 0 ]; then
    echo "All files are properly formatted!"
    exit 0
else
    echo "Some files are not properly formatted."
    echo "Run './scripts/format.sh' to fix formatting issues."
    exit 1
fi
