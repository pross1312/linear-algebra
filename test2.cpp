#include "lin_al_test.h"
#include <iostream>

int main() {
    Matrix<float, 5, 5> mat1(1);
    mat1.unary_transform([](float a) {
        return a + 3;
    });
    Matrix<int, 5, 5> mat2(121);
    mat1.bi_transform<int>(mat2, [](float a, int b) {
        return a + b;
    });
    mat1[0].unary_transform([](float a) {
        return a + 3;
    });
    std::cout << mat1[0][100] << "\n";
    std::cout << mat2 << "\n";
    std::cout << mat1 << "\n";
    std::cout << mat1 * mat2 << "\n";
    // Matrix<float, 5, 5> mat2(121.21);
    // // mat1.bi_transform(mat2, [](float a, float b) {
    // //     return a + b;
    // // });
    // std::cout << "------\n" << std::flush;
    // Matrix<float, 5, 5> mat3{ mat1 * mat2 };
    // std::cout << "------\n" << std::flush;
    // // Matrix<float, 5, 5> mat4{ mat1 };
    // std::cout << mat1 * mat2 << "\n";
    // std::cout << mat1        << "\n";
    // MatrixXX<float> matx1(5, 5, 5);
    // // std::cout << mat1 * matx1 << "\n";
    // MatrixXX<float> matx2(5, 5, 1);
    // MatrixXX<float> temp = matx1 * matx2;
    // std::cout << temp << "\n";
    return 0;
}
