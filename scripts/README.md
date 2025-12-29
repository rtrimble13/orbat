# Development Scripts

This directory contains utility scripts for development and maintenance of the orbat project.

## Scripts

### format.sh

Automatically formats all C++ source files using clang-format.

**Usage:**
```bash
./scripts/format.sh
```

This script:
- Finds all C++ files (`.cpp`, `.hpp`, `.h`, `.cc`, `.cxx`) in `include/`, `src/`, `tests/`, and `examples/`
- Applies the formatting rules defined in `.clang-format`
- Modifies files in-place to conform to the project's style guide

Run this before committing to ensure your code follows the project's formatting standards.

### check-format.sh

Checks if all C++ files are properly formatted without modifying them.

**Usage:**
```bash
./scripts/check-format.sh
```

This script:
- Checks all C++ files for formatting compliance
- Returns exit code 0 if all files are properly formatted
- Returns exit code 1 if any files need formatting
- Used in CI/CD pipelines to enforce formatting standards

If this script reports formatting issues, run `./scripts/format.sh` to fix them.

## Requirements

Both scripts require `clang-format` to be installed and available in your PATH.

**Installation:**

- **Ubuntu/Debian:**
  ```bash
  sudo apt-get install clang-format
  ```

- **macOS:**
  ```bash
  brew install clang-format
  ```

- **Windows:**
  Download from [LLVM releases](https://releases.llvm.org/) or use a package manager like Chocolatey

## CI Integration

The `check-format.sh` script is designed to be used in continuous integration pipelines to automatically verify that all code follows the formatting standards. If the check fails, the CI build will fail, preventing unformatted code from being merged.

## Making Scripts Executable

If you need to make the scripts executable:

```bash
chmod +x scripts/format.sh
chmod +x scripts/check-format.sh
```
