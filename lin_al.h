#pragma once
#include <cassert>
#include <functional>
#include <cstring>
#include <string>
#include <sstream>
#include <cmath>
inline size_t destruction_times = 0;
namespace LIN {
template<typename Type>
class VectorX;
// matrix and vector at runtime
template<typename Type>
class MatrixXX {
    friend class VectorX<Type>;
protected:
    MatrixXX<Type>(size_t rows, size_t cols, Type* data, bool borrowed): // special constructor
        data{ data },
        nRows{ rows },
        nCols{ cols },
        isBorrowed{ borrowed } { }
public:
    virtual ~MatrixXX() {
        if (!this->isBorrowed) {
            delete[] data;
            // printf("Destructor called %zu times\n", ++destruction_times);
        }
        // else printf("Can't delete borrowed data\n");
    }
// start define constructor
// anywhere that isBorrowed is not initialized, it is default initialized to false
    explicit MatrixXX<Type>(size_t rows, size_t cols):
        data{ new Type[rows * cols] },
        nRows{ rows },
        nCols{ cols } {
            memset(data, 0, sizeof(Type) * rows * cols);
    }

    explicit MatrixXX<Type>(size_t rows, size_t cols, Type val): MatrixXX<Type>(rows, cols) {
        for (size_t i = 0; i < rows*cols; i++) data[i] = val;
    }

    MatrixXX<Type>(const MatrixXX<Type>& base): MatrixXX<Type>(base.nRows, base.nCols) {
        memcpy(this->data, base.data, base.nRows * base.nCols * sizeof(Type));
    }

    // init with a function that spit out a row
    explicit MatrixXX<Type>(size_t rows, size_t cols, std::function<VectorX<Type>(size_t r)> init_func): MatrixXX<Type>(rows, cols) {
        for (size_t r = 0; r < nRows; r++) {
            const auto& row_vec = init_func(r);
            memcpy(this->data + r*nCols, row_vec.data, nCols);
        }
    }

    explicit MatrixXX<Type>(size_t rows, size_t cols, std::function<Type()> init_func): MatrixXX<Type>(rows, cols) {
        for (size_t i = 0; i < rows*cols; i++) data[i] = init_func();
    }

    explicit MatrixXX<Type>(size_t rows, size_t cols, std::function<Type(size_t, size_t)> init_func): MatrixXX<Type>(rows, cols) {
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                data[i*cols + j] = init_func(i, j);
            }
        }
    }
    explicit MatrixXX<Type>(size_t rows, size_t cols, const Type* ref_data): MatrixXX<Type>(rows, cosl) {  // this will copy data
            assert(data != nullptr && "Invalid data");
            memcpy(this->data, ref_data, rows * cols * sizeof(Type));
    }
// end define constructor

    inline Type& at(size_t r, size_t c) {
        assert(r < nRows && c < nCols && "Invalid index");
        return data[r * nCols + c];
    }

    inline const Type& at(size_t r, size_t c) const {
        assert(r < nRows && c < nCols && "Invalid index");
        return data[r * nCols + c];
    }

    inline size_t rows() const { return nRows; }
    inline size_t cols() const { return nCols; }
    inline Type* raw() { return data; }
    inline const Type* raw() const { return data; }

// --------TODO check again seems dumb and dangerous--------------
    VectorX<Type> operator[](size_t row) {
        return VectorX<Type>(nCols, &data[row * nCols], true); // create a borrowed data vector
    }
    const VectorX<Type> operator[](size_t row) const {
        return VectorX<Type>(nCols, &data[row * nCols], true); // create a borrowed data vector
    }
// ---------------------------------------------------------------
    bool operator==(const MatrixXX<Type>& that) const {
        if (!(nRows == that.nRows && nCols == that.nCols)) return false;
        for (size_t i = 0; i < nRows * nCols; i++) {
            if (this->data[i] != that.data[i]) return false;
        }
        return true;
    }
    bool operator!=(const MatrixXX<Type>& that) const { return !((*this) == that); }


    constexpr MatrixXX<Type> operator=(const MatrixXX<Type>& that) {
        assert(nRows == that.nRows && nCols == that.nCols && "Invalid size");
        for (size_t i = 0; i < nRows * nCols; i++) {
            this->data[i] = that.data[i];
        }
        return *this;
    }

    MatrixXX<Type> operator*(Type scalar) const {
        MatrixXX<Type> result(nRows, nCols);
        for (size_t i = 0; i < nRows * nCols; i++) {
            result.data[i] = this->data[i] * scalar;
        }
        return result;
    }

    MatrixXX<Type> operator+(Type scalar) const {
        MatrixXX<Type> result(nRows, nCols);
        for (size_t i = 0; i < nRows * nCols; i++) {
            result.data[i] = this->data[i] + scalar;
        }
        return result;
    }

    MatrixXX<Type> operator-(Type scalar) const {
        MatrixXX<Type> result(nRows, nCols);
        for (size_t i = 0; i < nRows * nCols; i++) {
            result.data[i] = this->data[i] - scalar;
        }
        return result;
    }


    MatrixXX<Type> operator/(Type scalar) const {
        MatrixXX<Type> result(nRows, nCols);
        for (size_t i = 0; i < nRows * nCols; i++) {
            result.data[i] = this->data[i] / scalar;
        }
        return result;
    }


    MatrixXX<Type> operator+(const MatrixXX<Type>& that) const {
        assert(this->nRows == that.nRows && this->nCols == that.nCols && "Invalid size");
        MatrixXX<Type> result(nRows, nCols);
        for (size_t i = 0; i < nRows * nCols; i++) {
            result.data[i] = this->data[i] + (that.data[i]);
        }
        return result;
    }


    MatrixXX<Type> operator-(const MatrixXX<Type>& that) const {
        assert(this->nRows == that.nRows && this->nCols == that.nCols && "Invalid size");
        MatrixXX<Type> result(nRows, nCols);
        for (size_t i = 0; i < nRows * nCols; i++) {
            result.data[i] = this->data[i] - that.data[i];
        }
        return result;
    }

    MatrixXX<Type> operator*(const MatrixXX<Type>& that) const {
        assert(this->nCols == that.nRows && "Invalid size");
        MatrixXX<Type> result(this->nRows, that.nCols);
        for (size_t r = 0; r < this->nRows; r++) {
            for (size_t c = 0; c < that.nCols; c++) {
                for (size_t i = 0; i < this->nCols; i++) {
                    result.at(r, c) += this->at(r, i) * that.at(i, c);
                }
            }
        }
        return result;
    }

    MatrixXX<Type>& transform(std::function<Type(Type)> func) {
        for (size_t i = 0; i < nRows * nCols; i++) {
            data[i] = func(data[i]);
        }
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& out, const MatrixXX<Type>& mat) {
        for (size_t i = 0; i < mat.nRows; i++) {
            out << "[ ";
            for (size_t j = 0; j < mat.nCols; j++) {
                out << std::to_string(mat.at(i, j)) << (j == mat.nCols - 1 ? "" : " ");
            }
            out << " ]\n";
        }
        return out;
    }

    std::string to_string() {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }

protected:
    Type* data;
    size_t nRows;
    size_t nCols;
    bool isBorrowed = false; // this variable is used for access row ( return a borrowed data vector, borrowed data won't be freed -_- rust moment)
};

/*
//----------------------- static Matrix and Vector --------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template<typename Type, size_t Rows, size_t Cols>
class Matrix {
public:
    virtual ~Matrix() { delete[] this->data; }
    explicit Matrix<Type, Rows, Cols>(Type val): data {new Type[Rows * Cols]} {
        for (size_t i = 0; i < Rows * Cols; i++) data[i] = val;
    }

    explicit Matrix<Type, Rows, Cols>(): data {new Type[Rows * Cols]} {
        memset(data, 0, Rows*Cols);
    }

    // init with a function that accept position (row and col) and spit out 1 number of type Type
    explicit Matrix<Type, Rows, Cols>(std::function<Type()> init_func): data {new Type[Rows * Cols]} {
        for (size_t i = 0; i < Rows*Cols; i++) {
            this->data[i] = init_func();
        }
    }

    // init with a function that accept position (row and col) and spit out 1 number of type Type
    explicit Matrix<Type, Rows, Cols>(std::function<Type(size_t, size_t)> init_func): data {new Type[Rows * Cols]} {
        for (size_t i = 0; i < Rows; i++) {
            for (size_t j = 0; j < Cols; j++) {
                this->data[i*Cols + j] = init_func(i, j);
            }
        }
    }

    explicit Matrix<Type, Rows, Cols>(const Type* data): // this constructor will copy
        data{ new Type[Rows * Cols] } {
            assert(data != nullptr && "Invalid data");
            memcpy(this->data, data, Rows * Cols * sizeof(Type));
    }

    Matrix<Type, Rows, Cols>(const Matrix<Type, Rows, Cols>& base): data {new Type[Rows * Cols]} {
        memcpy(this->data, base.data, sizeof(Type) * Rows * Cols);
    }


    inline Type* operator[](size_t row) {
        assert(row < Rows && "Index out of range");
        return &this->data[row * Cols];
    }

    inline size_t rows() const { return Rows; }
    inline size_t cols() const { return Cols; }

    inline Type* operator[](size_t row) const {
        assert(row < Rows && "Index out of range");
        return &this->data[row * Cols];
    }

    Matrix<Type, Rows, Cols> operator+(const Matrix<Type, Rows, Cols>& that) {
        // invalid size will be check at compile time by compiler
        Matrix<Type, Rows, Cols> result(Rows, Cols, 0);
        for (size_t i = 0; i < Rows * Cols; i++) {
            result.data[i] = this->data[i] + that.data[i];
        }
        return result;
    }

    Matrix<Type, Rows, Cols> operator-(const Matrix<Type, Rows, Cols>& that) {
        Matrix<Type, Rows, Cols> result(Rows, Cols, 0);
        for (size_t i = 0; i < Rows * Cols; i++) {
            result.data[i] = this->data[i] - that.data[i];
        }
        return result;
    }

    Matrix<Type, Rows, Cols> operator*(Type scalar) {
        Matrix<Type, Rows, Cols> result(Rows, Cols, 0);
        for (size_t i = 0; i < Rows * Cols; i++) {
            result.data[i] = this->data[i] * scalar;
        }
        return result;
    }

    template<size_t Op2_Col>
    Matrix<Type, Rows, Op2_Col> operator*(const Matrix<Type, Rows, Op2_Col>& that) {
        Matrix<Type, Rows, Op2_Col> result;
        for (size_t r = 0; r < Rows; r++) {
            for (size_t c = 0; c < Op2_Col; c++) {
                for (size_t i = 0; i < Cols; i++) {
                    result.data[r][c] += (*this)[r][i] * that[i][c];
                }
            }
        }
        return result;
    }

    friend std::ostream& operator<<(std::ostream& out, const Matrix<Type, Rows, Cols>& mat) {
        for (size_t i = 0; i < Rows; i++) {
            out << "[ ";
            for (size_t j = 0; j < Cols; j++) {
                out << std::to_string(mat[i][j]) << (j == Cols - 1 ? "" : " ");
            }
            out << " ]\n";
        }
        return out;
    }

    std::string to_string() {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }

    friend class MatrixXX<Type>;
protected:
    Type* data;
};

template<typename Type, size_t Size>
class Vector: public Matrix<Type, Size, 1> {
public:
    using Matrix<Type, Size, 1>::Matrix;
    inline Type& operator[](size_t index) {
        assert(index < Size && "Index out of range");
        return this->data[index];
    }
    inline const Type& operator[](size_t index) const {
        assert(index < Size && "Index out of range");
        return this->data[index];
    }

    Type dot(const Vector<Type, Size>& that) {
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

template<typename Type>
class Vector2: public Vector<Type, 2> {
public:
    using Vector<Type, 2>::Vector;
    explicit Vector2(Type x, Type y): Vector<Type, 2>() { this->data[0] = x; this->data[1] = y; }
    Type& x() { return this->data[0]; }
    Type& y() { return this->data[1]; }
};

template<typename Type>
class Vector3: public Vector<Type, 3> {
public:
    using Vector<Type, 3>::Vector;
    explicit Vector3(Type x, Type y, Type z) { this->data[0] = x; this->data[1] = y; this->data[2] = z; }
    Type& x() { return this->data[0]; }
    Type& y() { return this->data[1]; }
    Type& z() { return this->data[2]; }
};


template<typename Type>
class Vector4: public Vector<Type, 4> {
public:
    using Vector<Type, 4>::Vector;
    explicit Vector4(Type x, Type y, Type z, Type w) { this->data[0] = x; this->data[1] = y; this->data[2] = z; this->data[3] = w; }
    Type& x() { return this->data[0]; }
    Type& y() { return this->data[1]; }
    Type& z() { return this->data[2]; }
    Type& w() { return this->data[3]; }
};
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
*/
}
