#pragma once

namespace orbat {
namespace cli {

/**
 * @brief Standard exit codes for the orbat CLI.
 *
 * These exit codes provide deterministic, machine-readable feedback for CI/CD
 * and scripting purposes.
 */
enum class ExitCode {
    SUCCESS = 0,            // Operation completed successfully
    VALIDATION_ERROR = 1,   // Input validation failed (bad data format, missing files, etc.)
    COMPUTATION_ERROR = 2,  // Computation failed (optimization didn't converge, etc.)
    INVALID_ARGUMENTS = 3,  // Invalid command-line arguments
    INTERNAL_ERROR = 4      // Unexpected internal error
};

}  // namespace cli
}  // namespace orbat
