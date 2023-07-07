#pragma once
#include "lin_al.h"
namespace LIN {

template<typename Type>
class VectorX: public MatrixXX<Type> {
private:
    friend class MatrixXX<Type>;
    VectorX<Type>(size_t size, Type* data, bool borrowed): // a special constructor for operator[] of class MatrixXX
        MatrixXX<Type>(size, 1, data, borrowed) {
            this->isBorrowed = borrowed;
            assert(data != nullptr && "Invalid data");
        }
public:
    explicit VectorX<Type>(size_t size): MatrixXX<Type>(size, 1) {}

    explicit VectorX<Type>(size_t size, Type val): MatrixXX<Type>(size, 1, val) {}

    VectorX<Type>(const VectorX<Type>& base): MatrixXX<Type>(base) {} // power of polymorphism
                                                                      //
    // with this constructor, we can use some operators defined in MatrixXX class for this class as well
    // but these operator will create a temporary matrix which will be freed after data is copied into this VectorX
    // which is ineffiecient
    // TODO: define operator for this class as well
    explicit VectorX<Type>(const MatrixXX<Type>& base): MatrixXX<Type>(base) {
        assert(1 == this->nCols && "Invalid size");
        // printf("Construct vec with mat\n");
    }

    explicit VectorX<Type>(size_t size, std::function<Type()> init_func):
        MatrixXX<Type>(size, 1, init_func) {}

    explicit VectorX<Type>(size_t size, std::function<Type(size_t)> init_func):
        MatrixXX<Type>(size, 1) {
            for (size_t i = 0; i < size; i++) this->data[i] = init_func(i);
    }

    explicit VectorX<Type>(size_t size, const Type* data):
        MatrixXX<Type>(size, 1, data) {}

    inline Type& at(size_t index) {
        assert(index < this->nRows && "Index out of range");
        return this->data[index];
    }
    inline Type at(size_t index) const {
        assert(index < this->nRows && "Index out of range");
        return this->data[index];
    }

    inline Type& operator[](size_t index) { return at(index); }
    inline const Type& operator[](size_t index) const { return at(index); }

    bool operator==(const VectorX<Type>& that) const {
        if (!(this->nRows == that.nRows)) return false;
        for (size_t i = 0; i < this->nRows; i++) {
            if (this->data[i] != that.data[i]) return false;
        }
        return true;
    }
    bool operator!=(const VectorX<Type>& that) const { return !((*this) == that); }


    VectorX<Type>& operator=(const VectorX<Type>& that) {
        assert(this->nRows == that.nRows && "Invalid size");
        memcpy(this->data, that.raw(), this->nRows * sizeof(Type));
        return *this;
    }

    VectorX<Type> operator*(Type scalar) const {
        VectorX<Type> result(this->nRows);
        for (size_t i = 0; i < this->nRows; i++) {
            result.data[i] =this->data[i] * scalar;
        }
        return result;
    }

    VectorX<Type> operator+(Type scalar) const {
        VectorX<Type> result(this->nRows);
        for (size_t i = 0; i < this->nRows; i++) {
            result.data[i] =this->data[i] + scalar;
        }
        return result;
    }

    VectorX<Type> operator-(Type scalar) const {
        VectorX<Type> result(this->nRows);
        for (size_t i = 0; i < this->nRows; i++) {
            result.data[i] =this->data[i] - scalar;
        }
        return result;
    }


    VectorX<Type> operator/(Type scalar) const {
        VectorX<Type> result(this->nRows);
        for (size_t i = 0; i < this->nRows; i++) {
            result.data[i] =this->data[i] / scalar;
        }
        return result;
    }


    VectorX<Type> operator+(const VectorX<Type>& that) const {
        assert(this->nRows == that.nRows && "Invalid size");
        VectorX<Type> result(this->nRows);
        for (size_t i = 0; i < this->nRows; i++) {
            result.data[i] = this->data[i] + (that.data[i]);
        }
        return result;
    }


    VectorX<Type> operator-(const VectorX<Type>& that) const {
        assert(this->nRows == that.nRows && this->nCols == that.nCols && "Invalid size");
        VectorX<Type> result(this->nRows);
        for (size_t i = 0; i < this->nRows; i++) {
            result.data[i] = this->data[i] - that.data[i];
        }
        // printf("Operator -\n");
        return result;
    }


    VectorX<Type> operator*(const VectorX<Type>& that) const {
        assert(this->nCols == that.nRows && "Invalid size");
        VectorX<Type> result(this->nRows, that.nCols);
        for (size_t r = 0; r < this->nRows; r++) {
            for (size_t c = 0; c < that.nCols; c++) {
                for (size_t i = 0; i < this->nCols; i++) {
                    result.at(r, c) += this->at(r, i) * that.at(i, c);
                }
            }
        }
        return result;
    }

    inline size_t size() const { return this->nRows; }

    Type dot(const VectorX<Type>& that) const {
        assert(this->nRows == that.nRows && "Invalid size");
        Type result {};
        for (size_t i = 0; i < this->nRows; i++) {
            result += (float)this->data[i] * (float)that.data[i];
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
    VectorX<Type>& unary_op(std::function<Type(Type)> func) {
        for (size_t i = 0; i < this->nRows; i++) {
            this->data[i] = func(this->data[i]);
        }
        return *this;
    }

    template<typename Op2Type>
    VectorX<Type>& binary_op(const VectorX<Op2Type>& that, std::function<Type(Type, Op2Type)> func) {
        assert(this->nRows == that.rows() && "Invalid size");
        for (size_t i = 0; i < this->nRows; i++) {
            this->data[i] = func(this->data[i], that.raw()[i]);
        }
        return *this;
    }
};

}
