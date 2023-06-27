#include <cassert>
#include <functional>
#include <cstring>
#include <string>
#include <sstream>
#include <cmath>

// TODO: detect error from compile type by use template for rows and cols

template<typename Type = int>
class Matrix {
public:
    Matrix(size_t rows, size_t cols, Type val):
        data {new Type[rows * cols] {}},
        nRows {rows},
        nCols {cols} {
        for (size_t i = 0; i < rows * cols; i++) data[i] = val;
    }

    Matrix(size_t rows, size_t cols): Matrix(rows, cols, 0) {}

    // init with a function that accept position (row and col) and spit out 1 number of type Type
    Matrix(size_t rows, size_t cols, std::function<Type(size_t, size_t)> init_func) {
        this->nRows = rows;
        this->nCols = cols;
        this->data = new Type[rows * cols];
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                this->data[i*cols + j] = init_func(i, j);
            }
        }
    }
    Matrix(const Matrix& base) {
        memcpy(this->data, base.data, sizeof(Type) * base.nRows * base.nCols);
        this->nRows = base.nRows;
        this->nCols = base.nCols;
    }
    ~Matrix() { delete[] this->data; }

    Type* operator[](size_t row) {
        assert(row < this->nRows);
        return &this->data[row * this->nCols];
    }
    const Type* operator[](size_t row) const {
        assert(row < this->nRows);
        return &this->data[row * this->nCols];
    }

    Matrix operator+(const Matrix& that) {
        assert(this->nCols == that.nCols && this->nRows == that.nRows && "Invalid matrix sizes for add");
        Matrix<Type> result(this->nRows, this->nCols, 0);
        for (size_t i = 0; i < this->nRows * this->nCols; i++) {
            result.data[i] = this->data[i] + that.data[i];
        }
    }

    Matrix operator-(const Matrix& that) {
        assert(this->nCols == that.nCols && this->nRows == that.nRows && "Invalid matrix sizes for add");
        Matrix<Type> result(this->nRows, this->nCols, 0);
        for (size_t i = 0; i < this->nRows * this->nCols; i++) {
            result.data[i] = this->data[i] - that.data[i];
        }
    }

    Matrix operator*(const Matrix& that) {
        assert(this->nCols == that.nRows && "Invalid matrix sizes for multiplication");
        Matrix result(this->nRows, that.nCols);
        for (size_t r = 0; r < this->nRows; r++) {
            for (size_t c = 0; c < that.nCols; c++) {
                for (size_t i = 0; i < this->nCols; i++) {
                    result[r][c] += (*this)[r][i] * that[i][c];
                }
            }
        }
        return result;
    }

    std::string to_string() {
        std::stringstream ss;
        for (size_t i = 0; i < this->nRows; i++) {
            ss << "[ ";
            for (size_t j = 0; j < this->nCols; j++) {
                ss << std::to_string((*this)[i][j]) << ' ';
            }
            ss << "]";
            ss << '\n';
        }
        return ss.str();
    }

protected:
    Type* data;
    size_t nRows;
    size_t nCols;
};

template<typename Type = int>
class Vector: public Matrix<Type> {
public:
    Vector(size_t size): Matrix<Type>(size, 1, 0) {}
    Vector(size_t size, Type val): Matrix<Type>(size, 1, val) {}
    Vector(size_t size, std::function<Type(size_t, size_t)> init_func): Matrix<Type>(size, 1, init_func) {}
    Type dot(const Vector& that) {
        assert(this->nRows == that.nRows);
        Type result {};
        for (size_t i = 0; i < this->nRows; i++) {
            result += this->data[i] * that.data[i];
        }
        return result;
    }
    float length() {
        float len = 0;
        for (size_t i = 0; i < this->nRows; i++) {
            len += this->data[i] * this->data[i];
        }
        return std::sqrt(len);
    }
};
