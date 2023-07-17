#pragma once
#include <cassert>
#include <functional>
#include <cstring>
#include <string>
#include <sstream>
#include <cmath>

inline size_t count = 0;
template<typename Type> class RowVectorX;
template<typename Type>
class MatrixXX {
private:
    friend class RowVectorX<Type>;
    MatrixXX(size_t Rows, size_t Cols, Type* data, bool borrowed):
        nRows         { Rows     },
        nCols         { Cols     },
        data          { data     },
        borrowed_data { borrowed } {}
public:
    using BiIndexFunc    = std::function<Type(size_t, size_t)>;
    using UnaryIndexFunc = std::function<RowVectorX<Type>(size_t)>;
    using NoIndexFunc    = std::function<Type()>;
    template<typename Op2Type>
    using BiTypeFunc     = std::function<Type(Type, Op2Type)>;
    using UnaryTypeFunc  = std::function<Type(Type)>;
    virtual ~MatrixXX() {
        if (!borrowed_data) delete[] data;
    }
    explicit MatrixXX(size_t Rows, size_t Cols):
        nRows         { Rows                   },
        nCols         { Cols                   },
        data          { new Type[Rows*Cols] {} },
        borrowed_data { false                  }{
            memset(data, 0, nRows*nCols);
    }

    explicit MatrixXX(size_t Rows, size_t Cols, Type val): MatrixXX(Rows, Cols) {
        for (size_t i = 0;  i < Rows*Cols; i++) data[i] = val;
    }

    MatrixXX(const MatrixXX& base): MatrixXX(base.nRows, base.nCols) {
        memcpy(this->data, base.data, nRows*nCols);
    }

    explicit MatrixXX(size_t Rows, size_t Cols, const BiIndexFunc& init_func): MatrixXX(Rows, Cols) {
        for (size_t row = 0; row < nRows; row++) {
            for (size_t col = 0; col < nCols; col++) {
                data[row*Cols + col] = init_func(row, col);
            }
        }
    }

    explicit MatrixXX(size_t Rows, size_t Cols, const UnaryIndexFunc& init_func): MatrixXX(Rows, Cols) {
        for (size_t i = 0; i < nRows; i++) {
            memcpy(&this->data[i*nCols], init_func(i).data, nCols*sizeof(Type));
        }
    }

    explicit MatrixXX(size_t Rows, size_t Cols, const NoIndexFunc& init_func): MatrixXX(Rows, Cols) {
        for (size_t i = 0; i < nRows*nCols; i++) data[i] = init_func();
    }

    inline Type& at(size_t r, size_t c) {
        assert(r < nRows && c < nCols);
        return data[r*nCols + c];
    }

    inline const Type& at(size_t r, size_t c) const {
        assert(r < nRows && c < nCols);
        return data[r*nCols + c];
    }

    inline size_t rows() const { return nRows; }
    inline size_t cols() const { return nCols; }
    inline const Type*  raw()  const { return data;  }
    inline Type*  raw()  { return data;  }

    inline const MatrixXX& operator=(const MatrixXX& object) {
        memcpy(this->data, object.data, nRows*nCols);
        return *this;
    }

    inline RowVectorX<Type> operator[](size_t index) {
        return RowVectorX<Type>(nCols, &this->data[index * nCols], true);
    }

    template<typename Op2Type>
    inline MatrixXX<Type> operator*(const MatrixXX<Op2Type>& that) const {
        assert(this->nCols == that.rows() && "Invalid size");
        MatrixXX<Type> result(this->nRows, that.cols());
        for (size_t r = 0; r < this->nRows; r++) {
            for (size_t c = 0; c < that.cols(); c++) {
                for (size_t i = 0; i < this->nCols; i++) {
                    result.at(r, c) += this->at(r, i) * (Type)that.at(i, c);
                }
            }
        }
        return result;
    }

    template<typename Op2Type>
    inline void bi_transform(const MatrixXX<Op2Type>& object, const BiTypeFunc<Op2Type>& func) {
        assert(this->nRows == object.rows() && this->nCols == object.cols());
        for (size_t i = 0; i < nRows*nCols; i++) data[i] = (Type)func(this->data[i], object.raw()[i]);
    }

    inline void unary_transform(const UnaryTypeFunc& func) {
        for (size_t i = 0; i < nRows*nCols; i++) data[i] = func(data[i]);
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
    size_t nRows;
    size_t nCols;
    Type* data;
    bool borrowed_data;
};

template<typename Type>
class RowVectorX: public MatrixXX<Type> {
private:
    friend class MatrixXX<Type>;
    RowVectorX(size_t size, Type* data, bool borrowed): MatrixXX<Type>(1, size, data, borrowed) {}
public:
    using Base           = MatrixXX<Type>;
    using UnaryIndexFunc = std::function<Type(size_t)>;
    using NoIndexFunc    = std::function<Type()>;
    template<typename Op2Type>
    using BiTypeFunc     = std::function<Type(Type, Op2Type)>;
    using Base::unary_transform;
    using Base::to_string;
    explicit RowVectorX(size_t size)                                                    : Base(1, size) {}
    explicit RowVectorX(size_t size, Type val)                                          : Base(1, size, val) {}
    explicit RowVectorX(size_t size, Type* data)                                        : RowVectorX() { memcpy(this->data, data, size*sizeof(Type)); }
    explicit RowVectorX(size_t size, std::function<Type(size_t r, size_t c)> init_func) : Base(1, size, init_func) {}
    RowVectorX(const RowVectorX& base)                                                  : Base(base) {}
    explicit RowVectorX(size_t size, const UnaryIndexFunc& init_func)                   : Base(1, size, init_func) {}
    explicit RowVectorX(size_t size, const NoIndexFunc&    init_func)                   : Base(1, size, init_func) {}

    inline       Type&  operator[](size_t index)       { return this->at(0, index); }
    inline const Type&  operator[](size_t index) const { return this->at(0, index); }
    inline const RowVectorX operator=(const RowVectorX& object) {
        assert(this->cols() == object.cols());
        memcpy(this->raw(), object.raw(), this->cols()*sizeof(Type));
        return *this;
    }

    template<typename Op2Type>
    inline void bi_transform(const RowVectorX<Op2Type>& object, const BiTypeFunc<Op2Type>& func) {
        (*this).Base::bi_transform(object, func);
    }
};

template<typename Type, size_t Rows, size_t Cols>
class Matrix: public MatrixXX<Type> {
public:
    using Base           = MatrixXX<Type>;
    using BiIndexFunc    = std::function<Type(size_t r, size_t c)>;
    using UnaryIndexFunc = std::function<Type(size_t i)>;
    using NoIndexFunc    = std::function<Type()>;
    template<typename Op2Type>
    using BiTypeFunc     = std::function<Type(Type, Op2Type)>;
    using Base::unary_transform;
    using Base::at;
    using Base::to_string;

    explicit Matrix()                                                  : Base(Rows, Cols) {}
    explicit Matrix(Type val)                                          : Base(Rows, Cols, val) {}
    explicit Matrix(Type* data)                                        : Matrix() { memcpy(this->data, data, Rows*Cols*sizeof(Type)); }
    explicit Matrix(std::function<Type(size_t r, size_t c)> init_func) : Base(Rows, Cols, init_func) {}
    Matrix(const Matrix& base)                                         : Base(base) {}
    explicit Matrix(const BiIndexFunc&    init_func)                   : Base(Rows, Cols, init_func) {}
    explicit Matrix(const UnaryIndexFunc& init_func)                   : Base(Rows, Cols, init_func) {}
    explicit Matrix(const NoIndexFunc&    init_func)                   : Base(Rows, Cols, init_func) {}

    template<typename Op2Type, size_t Op2Cols>
    inline Matrix<Type, Rows, Op2Cols> operator*(const Matrix<Op2Type, Cols, Op2Cols>& object) {
        MatrixXX<Type> temp = (*this).Base::operator*(object);
        Matrix<Type, Rows, Op2Cols> result(temp.raw());
        return result;
    }

    template<typename Op2Type>
    inline void bi_transform(const Matrix<Op2Type, Rows, Cols>& object, const BiTypeFunc<Op2Type>& func) {
        (*this).Base::bi_transform(object, func);
    }
};
