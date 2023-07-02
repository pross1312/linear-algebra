#include <cassert>
#include <functional>
#include <cstring>
#include <string>
#include <sstream>
#include <cmath>

template<size_t Rows, size_t Cols, typename Type = int>
class Matrix {
public:
    Matrix(Type val): data {new Type[Rows * Cols]} {
        for (size_t i = 0; i < Rows * Cols; i++) data[i] = val;
    }

    Matrix(): data {new Type[Rows * Cols]} {
        memset(data, 0, Rows*Cols);
    }

    // init with a function that accept position (row and col) and spit out 1 number of type Type
    Matrix(std::function<Type(size_t, size_t)> init_func): data {new Type[Rows * Cols]} {
        for (size_t i = 0; i < Rows; i++) {
            for (size_t j = 0; j < Cols; j++) {
                this->data[i*Cols + j] = init_func(i, j);
            }
        }
    }

    Matrix<Rows, Cols, Type>(const Matrix<Rows, Cols, Type>& base): data {new Type[Rows * Cols]} {
        memcpy(this->data, base.data, sizeof(Type) * Rows * Cols);
    }

    ~Matrix() { delete[] this->data; }

    inline Type* operator[](size_t row) {
        assert(row < Rows);
        return &this->data[row * Cols];
    }

    inline size_t rows() { return Rows; }
    inline size_t cols() { return Cols; }

    inline const Type* operator[](size_t row) const {
        assert(row < Rows);
        return &this->data[row * Cols];
    }

    Matrix<Rows, Cols, Type> operator+(const Matrix<Rows, Cols, Type>& that) {
        // invalid size will be check at compile time by compiler
        Matrix<Rows, Cols, Type> result(Rows, Cols, 0);
        for (size_t i = 0; i < Rows * Cols; i++) {
            result.data[i] = this->data[i] + that.data[i];
        }
    }

    Matrix<Rows, Cols, Type> operator-(const Matrix<Rows, Cols, Type>& that) {
        Matrix<Rows, Cols, Type> result(Rows, Cols, 0);
        for (size_t i = 0; i < Rows * Cols; i++) {
            result.data[i] = this->data[i] - that.data[i];
        }
    }

    template<size_t Op2_Col>
    Matrix<Rows, Op2_Col, Type> operator*(const Matrix<Cols, Op2_Col, Type>& that) {
        Matrix<Rows, Op2_Col, Type> result;
        for (size_t r = 0; r < Rows; r++) {
            for (size_t c = 0; c < Op2_Col; c++) {
                for (size_t i = 0; i < Cols; i++) {
                    result[r][c] += (*this)[r][i] * that[i][c];
                }
            }
        }
        return result;
    }

    std::string to_string() {
        std::stringstream ss;
        for (size_t i = 0; i < Rows; i++) {
            ss << "[ ";
            for (size_t j = 0; j < Cols; j++) {
                ss << std::to_string((*this)[i][j]) << ' ';
            }
            ss << "]\n";
        }
        return ss.str();
    }

protected:
    Type* data;
};

template<size_t Size, typename Type = int>
class Vector: public Matrix<Size, 1, Type> {
public:
    Vector(): Matrix<Size, 1, Type>(0) {}
    Vector(Type val): Matrix<Size, 1, Type>(val) {}
    Vector(std::function<Type(size_t, size_t)> init_func): Matrix<Size, 1, Type>(init_func) {}
    Type dot(const Vector<Size, Type>& that) {
        Type result {};
        for (size_t i = 0; i < Size; i++) {
            result += this->data[i] * that.data[i];
        }
        return result;
    }
    float length() {
        float len = 0;
        for (size_t i = 0; i < Size; i++) {
            len += this->data[i] * this->data[i];
        }
        return std::sqrt(len);
    }
};
