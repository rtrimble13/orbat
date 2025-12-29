#pragma once

#include "orbat/core/constants.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <initializer_list>
#include <stdexcept>
#include <vector>

namespace orbat {
namespace core {

/**
 * @brief A lightweight vector class for portfolio optimization.
 *
 * Provides basic vector operations needed for portfolio calculations including
 * dot products, element-wise operations, and vector norms.
 *
 * Example:
 *   Vector v1({1.0, 2.0, 3.0});
 *   Vector v2({4.0, 5.0, 6.0});
 *   double dot = v1.dot(v2);  // 32.0
 */
class Vector {
public:
    /**
     * @brief Construct an empty vector.
     */
    Vector() = default;

    /**
     * @brief Construct a vector of given size, initialized to zero.
     * @param size Number of elements
     */
    explicit Vector(size_t size) : data_(size, 0.0) {}

    /**
     * @brief Construct a vector of given size with initial value.
     * @param size Number of elements
     * @param value Initial value for all elements
     */
    Vector(size_t size, double value) : data_(size, value) {}

    /**
     * @brief Construct a vector from initializer list.
     * @param init Initializer list of values
     */
    Vector(std::initializer_list<double> init) : data_(init) {}

    /**
     * @brief Construct a vector from std::vector.
     * @param data Vector data
     */
    explicit Vector(const std::vector<double>& data) : data_(data) {}

    /**
     * @brief Construct a vector from std::vector (move).
     * @param data Vector data
     */
    explicit Vector(std::vector<double>&& data) : data_(std::move(data)) {}

    /**
     * @brief Get the size of the vector.
     * @return Number of elements
     */
    size_t size() const { return data_.size(); }

    /**
     * @brief Check if vector is empty.
     * @return true if empty, false otherwise
     */
    bool empty() const { return data_.empty(); }

    /**
     * @brief Resize the vector.
     * @param size New size
     * @param value Value for new elements (default 0.0)
     */
    void resize(size_t size, double value = 0.0) { data_.resize(size, value); }

    /**
     * @brief Access element (const).
     * @param index Element index
     * @return Element value
     */
    double operator[](size_t index) const {
        assert(index < data_.size() && "Vector index out of bounds");
        return data_[index];
    }

    /**
     * @brief Access element (non-const).
     * @param index Element index
     * @return Reference to element
     */
    double& operator[](size_t index) {
        assert(index < data_.size() && "Vector index out of bounds");
        return data_[index];
    }

    /**
     * @brief Access element with bounds checking.
     * @param index Element index
     * @return Element value
     * @throws std::out_of_range if index is invalid
     */
    double at(size_t index) const { return data_.at(index); }

    /**
     * @brief Access element with bounds checking (non-const).
     * @param index Element index
     * @return Reference to element
     * @throws std::out_of_range if index is invalid
     */
    double& at(size_t index) { return data_.at(index); }

    /**
     * @brief Get underlying data as const vector.
     * @return Const reference to data
     */
    const std::vector<double>& data() const { return data_; }

    /**
     * @brief Get underlying data as non-const vector.
     * @return Reference to data
     */
    std::vector<double>& data() { return data_; }

    /**
     * @brief Compute dot product with another vector.
     * @param other Other vector
     * @return Dot product
     * @throws std::invalid_argument if sizes don't match
     */
    double dot(const Vector& other) const {
        if (size() != other.size()) {
            throw std::invalid_argument("Vector dot product requires equal sizes");
        }

        double result = 0.0;
        for (size_t i = 0; i < size(); ++i) {
            result += data_[i] * other.data_[i];
        }
        return result;
    }

    /**
     * @brief Compute L2 (Euclidean) norm of the vector.
     * @return L2 norm
     */
    double norm() const { return std::sqrt(dot(*this)); }

    /**
     * @brief Compute sum of all elements.
     * @return Sum
     */
    double sum() const {
        double result = 0.0;
        for (double val : data_) {
            result += val;
        }
        return result;
    }

    /**
     * @brief Vector addition.
     * @param other Vector to add
     * @return Result vector
     * @throws std::invalid_argument if sizes don't match
     */
    Vector operator+(const Vector& other) const {
        if (size() != other.size()) {
            throw std::invalid_argument("Vector addition requires equal sizes");
        }

        Vector result(size());
        for (size_t i = 0; i < size(); ++i) {
            result.data_[i] = data_[i] + other.data_[i];
        }
        return result;
    }

    /**
     * @brief Vector subtraction.
     * @param other Vector to subtract
     * @return Result vector
     * @throws std::invalid_argument if sizes don't match
     */
    Vector operator-(const Vector& other) const {
        if (size() != other.size()) {
            throw std::invalid_argument("Vector subtraction requires equal sizes");
        }

        Vector result(size());
        for (size_t i = 0; i < size(); ++i) {
            result.data_[i] = data_[i] - other.data_[i];
        }
        return result;
    }

    /**
     * @brief Scalar multiplication.
     * @param scalar Scalar value
     * @return Result vector
     */
    Vector operator*(double scalar) const {
        Vector result(size());
        for (size_t i = 0; i < size(); ++i) {
            result.data_[i] = data_[i] * scalar;
        }
        return result;
    }

    /**
     * @brief Scalar division.
     * @param scalar Scalar value
     * @return Result vector
     * @throws std::invalid_argument if scalar is zero
     */
    Vector operator/(double scalar) const {
        if (std::abs(scalar) < EPSILON) {
            throw std::invalid_argument("Division by zero");
        }

        Vector result(size());
        for (size_t i = 0; i < size(); ++i) {
            result.data_[i] = data_[i] / scalar;
        }
        return result;
    }

    /**
     * @brief In-place addition.
     * @param other Vector to add
     * @return Reference to this vector
     * @throws std::invalid_argument if sizes don't match
     */
    Vector& operator+=(const Vector& other) {
        if (size() != other.size()) {
            throw std::invalid_argument("Vector addition requires equal sizes");
        }

        for (size_t i = 0; i < size(); ++i) {
            data_[i] += other.data_[i];
        }
        return *this;
    }

    /**
     * @brief In-place subtraction.
     * @param other Vector to subtract
     * @return Reference to this vector
     * @throws std::invalid_argument if sizes don't match
     */
    Vector& operator-=(const Vector& other) {
        if (size() != other.size()) {
            throw std::invalid_argument("Vector subtraction requires equal sizes");
        }

        for (size_t i = 0; i < size(); ++i) {
            data_[i] -= other.data_[i];
        }
        return *this;
    }

    /**
     * @brief In-place scalar multiplication.
     * @param scalar Scalar value
     * @return Reference to this vector
     */
    Vector& operator*=(double scalar) {
        for (size_t i = 0; i < size(); ++i) {
            data_[i] *= scalar;
        }
        return *this;
    }

    /**
     * @brief In-place scalar division.
     * @param scalar Scalar value
     * @return Reference to this vector
     * @throws std::invalid_argument if scalar is zero
     */
    Vector& operator/=(double scalar) {
        if (std::abs(scalar) < EPSILON) {
            throw std::invalid_argument("Division by zero");
        }

        for (size_t i = 0; i < size(); ++i) {
            data_[i] /= scalar;
        }
        return *this;
    }

private:
    std::vector<double> data_;
};

/**
 * @brief Scalar multiplication (scalar * vector).
 * @param scalar Scalar value
 * @param vec Vector
 * @return Result vector
 */
inline Vector operator*(double scalar, const Vector& vec) {
    return vec * scalar;
}

}  // namespace core
}  // namespace orbat
