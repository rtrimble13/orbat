#pragma once

namespace orbat {
namespace core {

/**
 * @brief Numerical constants used throughout the linear algebra module.
 */

/**
 * @brief Machine epsilon for floating-point comparisons.
 *
 * Used for comparing floating-point numbers and detecting near-zero values.
 * This value is appropriate for double-precision (64-bit) floating-point.
 */
static constexpr double EPSILON = 1e-15;

}  // namespace core
}  // namespace orbat
