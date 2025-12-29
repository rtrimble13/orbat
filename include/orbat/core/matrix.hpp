#pragma once

#include "orbat/core/constants.hpp"
#include "orbat/core/vector.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <initializer_list>
#include <stdexcept>
#include <vector>

namespace orbat {
namespace core {

/**
 * @brief A lightweight matrix class for portfolio optimization.
 *
 * Provides basic matrix operations needed for portfolio calculations including
 * matrix multiplication, transpose, and inversion via Cholesky decomposition
 * for positive-definite matrices (such as covariance matrices).
 *
 * Matrix data is stored in row-major order.
 *
 * Example:
 *   Matrix A(2, 2);
 *   A(0, 0) = 1.0; A(0, 1) = 2.0;
 *   A(1, 0) = 3.0; A(1, 1) = 4.0;
 *   Matrix B = A.transpose();
 *   Matrix C = A * B;
 */
class Matrix {
public:
    /**
     * @brief Construct an empty matrix.
     */
    Matrix() : rows_(0), cols_(0) {}

    /**
     * @brief Construct a matrix of given dimensions, initialized to zero.
     * @param rows Number of rows
     * @param cols Number of columns
     */
    Matrix(size_t rows, size_t cols) : rows_(rows), cols_(cols), data_(rows * cols, 0.0) {}

    /**
     * @brief Construct a matrix of given dimensions with initial value.
     * @param rows Number of rows
     * @param cols Number of columns
     * @param value Initial value for all elements
     */
    Matrix(size_t rows, size_t cols, double value)
        : rows_(rows), cols_(cols), data_(rows * cols, value) {}

    /**
     * @brief Construct a matrix from 2D initializer list.
     * @param init 2D initializer list
     * @throws std::invalid_argument if rows have different lengths
     */
    Matrix(std::initializer_list<std::initializer_list<double>> init) {
        rows_ = init.size();
        if (rows_ == 0) {
            cols_ = 0;
            return;
        }

        cols_ = init.begin()->size();
        data_.reserve(rows_ * cols_);

        for (const auto& row : init) {
            if (row.size() != cols_) {
                throw std::invalid_argument("All rows must have the same length");
            }
            data_.insert(data_.end(), row.begin(), row.end());
        }
    }

    /**
     * @brief Get number of rows.
     * @return Number of rows
     */
    size_t rows() const { return rows_; }

    /**
     * @brief Get number of columns.
     * @return Number of columns
     */
    size_t cols() const { return cols_; }

    /**
     * @brief Get total number of elements.
     * @return rows * cols
     */
    size_t size() const { return rows_ * cols_; }

    /**
     * @brief Check if matrix is empty.
     * @return true if empty, false otherwise
     */
    bool empty() const { return rows_ == 0 || cols_ == 0; }

    /**
     * @brief Check if matrix is square.
     * @return true if square, false otherwise
     */
    bool isSquare() const { return rows_ == cols_; }

    /**
     * @brief Resize the matrix.
     * @param rows New number of rows
     * @param cols New number of columns
     * @param value Value for new elements (default 0.0)
     */
    void resize(size_t rows, size_t cols, double value = 0.0) {
        rows_ = rows;
        cols_ = cols;
        data_.resize(rows * cols, value);
    }

    /**
     * @brief Access element (const).
     * @param row Row index
     * @param col Column index
     * @return Element value
     */
    double operator()(size_t row, size_t col) const {
        assert(row < rows_ && col < cols_ && "Matrix index out of bounds");
        return data_[row * cols_ + col];
    }

    /**
     * @brief Access element (non-const).
     * @param row Row index
     * @param col Column index
     * @return Reference to element
     */
    double& operator()(size_t row, size_t col) {
        assert(row < rows_ && col < cols_ && "Matrix index out of bounds");
        return data_[row * cols_ + col];
    }

    /**
     * @brief Access element with bounds checking.
     * @param row Row index
     * @param col Column index
     * @return Element value
     * @throws std::out_of_range if indices are invalid
     */
    double at(size_t row, size_t col) const {
        if (row >= rows_ || col >= cols_) {
            throw std::out_of_range("Matrix index out of bounds");
        }
        return data_[row * cols_ + col];
    }

    /**
     * @brief Access element with bounds checking (non-const).
     * @param row Row index
     * @param col Column index
     * @return Reference to element
     * @throws std::out_of_range if indices are invalid
     */
    double& at(size_t row, size_t col) {
        if (row >= rows_ || col >= cols_) {
            throw std::out_of_range("Matrix index out of bounds");
        }
        return data_[row * cols_ + col];
    }

    /**
     * @brief Get underlying data as const vector.
     * @return Const reference to data (row-major order)
     */
    const std::vector<double>& data() const { return data_; }

    /**
     * @brief Get underlying data as non-const vector.
     * @return Reference to data (row-major order)
     */
    std::vector<double>& data() { return data_; }

    /**
     * @brief Get a row as a Vector.
     * @param row Row index
     * @return Vector containing row elements
     * @throws std::out_of_range if row index is invalid
     */
    Vector getRow(size_t row) const {
        if (row >= rows_) {
            throw std::out_of_range("Row index out of bounds");
        }

        Vector result(cols_);
        for (size_t j = 0; j < cols_; ++j) {
            result[j] = (*this)(row, j);
        }
        return result;
    }

    /**
     * @brief Get a column as a Vector.
     * @param col Column index
     * @return Vector containing column elements
     * @throws std::out_of_range if column index is invalid
     */
    Vector getColumn(size_t col) const {
        if (col >= cols_) {
            throw std::out_of_range("Column index out of bounds");
        }

        Vector result(rows_);
        for (size_t i = 0; i < rows_; ++i) {
            result[i] = (*this)(i, col);
        }
        return result;
    }

    /**
     * @brief Set a row from a Vector.
     * @param row Row index
     * @param values Vector of values
     * @throws std::invalid_argument if vector size doesn't match columns
     */
    void setRow(size_t row, const Vector& values) {
        if (row >= rows_) {
            throw std::out_of_range("Row index out of bounds");
        }
        if (values.size() != cols_) {
            throw std::invalid_argument("Vector size must match number of columns");
        }

        for (size_t j = 0; j < cols_; ++j) {
            (*this)(row, j) = values[j];
        }
    }

    /**
     * @brief Set a column from a Vector.
     * @param col Column index
     * @param values Vector of values
     * @throws std::invalid_argument if vector size doesn't match rows
     */
    void setColumn(size_t col, const Vector& values) {
        if (col >= cols_) {
            throw std::out_of_range("Column index out of bounds");
        }
        if (values.size() != rows_) {
            throw std::invalid_argument("Vector size must match number of rows");
        }

        for (size_t i = 0; i < rows_; ++i) {
            (*this)(i, col) = values[i];
        }
    }

    /**
     * @brief Compute transpose of the matrix.
     * @return Transposed matrix
     */
    Matrix transpose() const {
        Matrix result(cols_, rows_);
        for (size_t i = 0; i < rows_; ++i) {
            for (size_t j = 0; j < cols_; ++j) {
                result(j, i) = (*this)(i, j);
            }
        }
        return result;
    }

    /**
     * @brief Matrix multiplication.
     * @param other Matrix to multiply with
     * @return Result matrix
     * @throws std::invalid_argument if dimensions are incompatible
     */
    Matrix operator*(const Matrix& other) const {
        if (cols_ != other.rows_) {
            throw std::invalid_argument(
                "Matrix multiplication requires cols of first matrix to match "
                "rows of second");
        }

        Matrix result(rows_, other.cols_);
        for (size_t i = 0; i < rows_; ++i) {
            for (size_t j = 0; j < other.cols_; ++j) {
                double sum = 0.0;
                for (size_t k = 0; k < cols_; ++k) {
                    sum += (*this)(i, k) * other(k, j);
                }
                result(i, j) = sum;
            }
        }
        return result;
    }

    /**
     * @brief Matrix-vector multiplication.
     * @param vec Vector to multiply with
     * @return Result vector
     * @throws std::invalid_argument if dimensions are incompatible
     */
    Vector operator*(const Vector& vec) const {
        if (cols_ != vec.size()) {
            throw std::invalid_argument(
                "Matrix-vector multiplication requires matrix columns to match "
                "vector size");
        }

        Vector result(rows_);
        for (size_t i = 0; i < rows_; ++i) {
            double sum = 0.0;
            for (size_t j = 0; j < cols_; ++j) {
                sum += (*this)(i, j) * vec[j];
            }
            result[i] = sum;
        }
        return result;
    }

    /**
     * @brief Matrix addition.
     * @param other Matrix to add
     * @return Result matrix
     * @throws std::invalid_argument if dimensions don't match
     */
    Matrix operator+(const Matrix& other) const {
        if (rows_ != other.rows_ || cols_ != other.cols_) {
            throw std::invalid_argument("Matrix addition requires equal dimensions");
        }

        Matrix result(rows_, cols_);
        for (size_t i = 0; i < data_.size(); ++i) {
            result.data_[i] = data_[i] + other.data_[i];
        }
        return result;
    }

    /**
     * @brief Matrix subtraction.
     * @param other Matrix to subtract
     * @return Result matrix
     * @throws std::invalid_argument if dimensions don't match
     */
    Matrix operator-(const Matrix& other) const {
        if (rows_ != other.rows_ || cols_ != other.cols_) {
            throw std::invalid_argument("Matrix subtraction requires equal dimensions");
        }

        Matrix result(rows_, cols_);
        for (size_t i = 0; i < data_.size(); ++i) {
            result.data_[i] = data_[i] - other.data_[i];
        }
        return result;
    }

    /**
     * @brief Scalar multiplication.
     * @param scalar Scalar value
     * @return Result matrix
     */
    Matrix operator*(double scalar) const {
        Matrix result(rows_, cols_);
        for (size_t i = 0; i < data_.size(); ++i) {
            result.data_[i] = data_[i] * scalar;
        }
        return result;
    }

    /**
     * @brief Scalar division.
     * @param scalar Scalar value
     * @return Result matrix
     * @throws std::invalid_argument if scalar is zero
     */
    Matrix operator/(double scalar) const {
        if (std::abs(scalar) < EPSILON) {
            throw std::invalid_argument("Division by zero");
        }

        Matrix result(rows_, cols_);
        for (size_t i = 0; i < data_.size(); ++i) {
            result.data_[i] = data_[i] / scalar;
        }
        return result;
    }

    /**
     * @brief Create an identity matrix.
     * @param size Dimension of the identity matrix
     * @return Identity matrix
     */
    static Matrix identity(size_t size) {
        Matrix result(size, size);
        for (size_t i = 0; i < size; ++i) {
            result(i, i) = 1.0;
        }
        return result;
    }

    /**
     * @brief Compute Cholesky decomposition of a positive-definite matrix.
     *
     * For a symmetric positive-definite matrix A, computes the lower
     * triangular matrix L such that A = L * L^T.
     *
     * This is useful for solving linear systems and computing matrix
     * inverses for covariance matrices.
     *
     * @return Lower triangular Cholesky factor L
     * @throws std::invalid_argument if matrix is not square
     * @throws std::runtime_error if matrix is not positive-definite
     */
    Matrix cholesky() const {
        if (!isSquare()) {
            throw std::invalid_argument("Cholesky decomposition requires a square matrix");
        }

        const size_t n = rows_;
        Matrix L(n, n);

        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j <= i; ++j) {
                double sum = 0.0;

                if (i == j) {
                    // Diagonal elements
                    for (size_t k = 0; k < j; ++k) {
                        sum += L(j, k) * L(j, k);
                    }

                    double value = (*this)(j, j) - sum;
                    if (value <= 0.0) {
                        throw std::runtime_error("Matrix is not positive-definite");
                    }
                    L(j, j) = std::sqrt(value);
                } else {
                    // Off-diagonal elements
                    for (size_t k = 0; k < j; ++k) {
                        sum += L(i, k) * L(j, k);
                    }
                    L(i, j) = ((*this)(i, j) - sum) / L(j, j);
                }
            }
        }

        return L;
    }

    /**
     * @brief Solve the linear system Lx = b where L is lower triangular.
     *
     * Forward substitution for lower triangular systems.
     *
     * @param b Right-hand side vector
     * @return Solution vector x
     * @throws std::invalid_argument if dimensions don't match
     */
    Vector solveLower(const Vector& b) const {
        if (rows_ != b.size() || !isSquare()) {
            throw std::invalid_argument("Matrix must be square and match vector size");
        }

        const size_t n = rows_;
        Vector x(n);

        for (size_t i = 0; i < n; ++i) {
            double sum = 0.0;
            for (size_t j = 0; j < i; ++j) {
                sum += (*this)(i, j) * x[j];
            }

            if (std::abs((*this)(i, i)) < EPSILON) {
                throw std::runtime_error("Matrix is singular (zero diagonal element)");
            }

            x[i] = (b[i] - sum) / (*this)(i, i);
        }

        return x;
    }

    /**
     * @brief Solve the linear system Ux = b where U is upper triangular.
     *
     * Backward substitution for upper triangular systems.
     *
     * @param b Right-hand side vector
     * @return Solution vector x
     * @throws std::invalid_argument if dimensions don't match
     */
    Vector solveUpper(const Vector& b) const {
        if (rows_ != b.size() || !isSquare()) {
            throw std::invalid_argument("Matrix must be square and match vector size");
        }

        const size_t n = rows_;
        Vector x(n);

        // Backward substitution: iterate from last row to first
        for (size_t i = n; i-- > 0;) {
            double sum = 0.0;
            for (size_t j = i + 1; j < n; ++j) {
                sum += (*this)(i, j) * x[j];
            }

            if (std::abs((*this)(i, i)) < EPSILON) {
                throw std::runtime_error("Matrix is singular (zero diagonal element)");
            }

            x[i] = (b[i] - sum) / (*this)(i, i);
        }

        return x;
    }

    /**
     * @brief Compute inverse of a positive-definite matrix using Cholesky
     * decomposition.
     *
     * For a symmetric positive-definite matrix A (such as a covariance
     * matrix), computes the inverse A^-1 using Cholesky decomposition.
     *
     * This is more numerically stable than general matrix inversion for
     * positive-definite matrices.
     *
     * @return Inverse matrix
     * @throws std::invalid_argument if matrix is not square
     * @throws std::runtime_error if matrix is not positive-definite
     */
    Matrix inverse() const {
        if (!isSquare()) {
            throw std::invalid_argument("Matrix inversion requires a square matrix");
        }

        const size_t n = rows_;

        // Compute Cholesky decomposition: A = L * L^T
        Matrix L = cholesky();
        Matrix LT = L.transpose();

        // Compute inverse by solving: A * A^-1 = I
        // For each column i of the identity matrix:
        //   Solve L * y = e_i (forward substitution)
        //   Solve L^T * x = y (backward substitution)
        //   x is the i-th column of A^-1

        Matrix inv(n, n);

        for (size_t i = 0; i < n; ++i) {
            // Create i-th column of identity matrix
            Vector ei(n, 0.0);
            ei[i] = 1.0;

            // Solve L * y = e_i
            Vector y = L.solveLower(ei);

            // Solve L^T * x = y
            Vector x = LT.solveUpper(y);

            // Set i-th column of inverse
            inv.setColumn(i, x);
        }

        return inv;
    }

private:
    size_t rows_;
    size_t cols_;
    std::vector<double> data_;  // Row-major order
};

/**
 * @brief Scalar multiplication (scalar * matrix).
 * @param scalar Scalar value
 * @param mat Matrix
 * @return Result matrix
 */
inline Matrix operator*(double scalar, const Matrix& mat) {
    return mat * scalar;
}

}  // namespace core
}  // namespace orbat
