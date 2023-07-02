#include <cassert>
#include <functional>
#include <cstring>
#include <string>
#include <sstream>
#include <cmath>

namespace LIN {
template<typename Type>
class MatrixXX;

// matrix and vector at compile time
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

    Matrix<Type, Rows, Cols>(const Matrix<Type, Rows, Cols>& base): data {new Type[Rows * Cols]} {
        memcpy(this->data, base.data, sizeof(Type) * Rows * Cols);
    }


    inline Type* operator[](size_t row) {
        assert(row < Rows && "Index out of range");
        return &this->data[row * Cols];
    }

    inline size_t rows() const { return Rows; }
    inline size_t cols() const { return Cols; }

    inline const Type* operator[](size_t row) const {
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
            ss << " ]\n";
        }
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
        return (*this)[index][0];
    }
    inline const Type& operator[](size_t index) const {
        assert(index < Size && "Index out of range");
        return (*this)[index][0];
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



// matrix and vector at runtime
template<typename Type>
class MatrixXX {
public:
    virtual ~MatrixXX() { delete[] data; }
    explicit MatrixXX<Type>(size_t rows, size_t cols, Type val):
        data {new Type[rows * cols]},
        nRows {rows},
        nCols {cols} {
            for (size_t i = 0; i < rows*cols; i++) data[i] = val;
    }

    explicit MatrixXX<Type>(size_t rows, size_t cols):
        data {new Type[rows * cols]},
        nRows {rows},
        nCols {cols} {
            memset(data, 0, sizeof(Type) * rows * cols);
    }

    MatrixXX<Type>(const MatrixXX<Type>& base):
        data {new Type[base.nRows * base.nCols]},
        nRows {base.nRows},
        nCols {base.nCols} {
            memcpy(data, base.data, base.nRows*base.nCols);
    }

    // init with a function that accept position (row and col) and spit out 1 number of type Type
    explicit MatrixXX<Type>(size_t rows, size_t cols, std::function<Type()> init_func):
        data { new Type[rows * cols] },
        nRows{ rows },
        nCols{ cols } {
        for (size_t i = 0; i < rows*cols; i++) {
            this->data[i] = init_func();
        }
    }

    // init with a function that accept position (row and col) and spit out 1 number of type Type
    explicit MatrixXX<Type>(size_t rows, size_t cols, std::function<Type(size_t, size_t)> init_func):
        data{ new Type[rows * cols] },
        nRows{ rows },
        nCols{ cols } {
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                this->data[i*cols + j] = init_func(i, j);
            }
        }
    }

    template<size_t Rows, size_t Cols>
    MatrixXX(const Matrix<Type, Rows, Cols>& base):
        data{ new Type[Rows*Cols] },
        nRows{ Rows },
        nCols{ Cols } {
            memcpy(data, base.data, sizeof(Type) * Rows * Cols);
    }

    inline Type* operator[](size_t row) {
        assert(row < nRows && "Index out of range");
        return &this->data[row * nCols];
    }
    inline size_t rows() const { return nRows; }
    inline size_t cols() const { return nCols; }
    inline const Type* operator[](size_t row) const {
        assert(row < nRows && "Index out of range");
        return &this->data[row * nCols];
    }
    const MatrixXX<Type> operator=(const MatrixXX<Type>& that) {
        assert(this->nRows == that.nRows && this->nCols == that.nCols && "Invalid size");
        for (size_t i = 0; i < nRows*nCols; i++) {
            this->data[i] = that.data[i];
        }
        return *this;
    }

    MatrixXX<Type> operator*(Type scalar) {
        MatrixXX<Type> result(nRows, nCols);
        for (size_t i = 0; i < nRows * nCols; i++) {
            result.data[i] = this->data[i] * scalar;
        }
        return result;
    }

    MatrixXX<Type> operator+(const MatrixXX<Type>& that) {
        assert(this->nRows == that.nRows && this->nCols == that.nCols && "Invalid size");
        MatrixXX<Type> result(nRows, nCols);
        for (size_t i = 0; i < nRows * nCols; i++) {
            result.data[i] = this->data[i] + that.data[i];
        }
        return result;
    }
    void operator+=(const MatrixXX<Type>& that) {
        assert(this->nRows == that.nRows && this->nCols == that.nCols && "Invalid size");
        for (size_t i = 0; i < nRows * nCols; i++) {
            this->data[i] += that.data[i];
        }
    }

    MatrixXX<Type> operator-(const MatrixXX<Type>& that) {
        assert(this->nRows == that.nRows && this->nCols == that.nCols && "Invalid size");
        MatrixXX<Type> result(nRows, nCols);
        for (size_t i = 0; i < nRows * nCols; i++) {
            result.data[i] = this->data[i] - that.data[i];
        }
        return result;
    }

    MatrixXX<Type> operator*(const MatrixXX<Type>& that) {
        assert(this->nCols == that.nRows && "Invalid size");
        MatrixXX<Type> result(this->nRows, that.nCols);
        for (size_t r = 0; r < nRows; r++) {
            for (size_t c = 0; c < that.nCols; c++) {
                for (size_t i = 0; i < nCols; i++) {
                    result[r][c] += (*this)[r][i] * that[i][c];
                }
            }
        }
        return result;
    }

    MatrixXX<Type>& unary_expr(std::function<Type(Type)> func) {
        for (size_t i = 0; i < this->nRows*this->nCols; i++) {
            this->data[i] = func(this->data[i]);
        }
        return *this;
    }

    std::string to_string() {
        std::stringstream ss;
        for (size_t i = 0; i < nRows; i++) {
            ss << "[ ";
            for (size_t j = 0; j < nCols; j++) {
                ss << std::to_string((*this)[i][j]) << ' ';
            }
            ss << " ]\n";
        }
        return ss.str();
    }

protected:
    Type* data;
    size_t nRows;
    size_t nCols;
};

template<typename Type>
class VectorX: public MatrixXX<Type> {
public:
    using MatrixXX<Type>::operator=;
    explicit VectorX<Type>(size_t size, Type val): MatrixXX<Type>(size, 1, val) {}
    explicit VectorX<Type>(size_t size): MatrixXX<Type>(size, 1) {}
    VectorX<Type>(const VectorX<Type>& base): MatrixXX<Type>(base) {} // power of polymorphism

    explicit VectorX<Type>(const MatrixXX<Type>& base): MatrixXX<Type>(base) {
        assert(1 == this->nCols && "Invalid size");
    }
    explicit VectorX<Type>(size_t size, std::function<Type()> init_func):
        MatrixXX<Type>(size, 1, init_func) {}
    explicit VectorX<Type>(size_t size, std::function<Type(size_t)> init_func):
        MatrixXX<Type>(size, 1, init_func) {}

    inline Type& operator[](size_t index) {
        assert(index < this->nRows && "Index out of range");
        return (*this)[index][0];
    }
    inline const Type& operator[](size_t index) const {
        assert(index < this->nRows && "Index out of range");
        return (*this)[index][0];
    }
    inline size_t size() const { return this->nRows; }

    Type dot(const VectorX<Type>& that) {
        assert(this->nRows == that.nRows && "Invalid size");
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

}
