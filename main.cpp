#include "lin_al.h"
#include "raylib.h"
#include <cstdio>
int main() {
    LIN::VectorX<float> temp(5, 291.213);
    LIN::MatrixXX<float> temp2(5, 5, 123.2123);
    LIN::MatrixXX<float> temp3{ temp2 * temp };
    LIN::VectorX<float> temp4(5, 123.1213);
    LIN::VectorX<float> temp5{ (size_t)temp.dot(temp4) };
    LIN::Matrix<float, 5, 5> temp6{ []() { return rand(); } };
    LIN::MatrixXX<float> temp7{ temp6 };
    LIN::MatrixXX<float>* temp8 = new LIN::MatrixXX{ temp4 };
    // printf("temp\n%s\n", temp.to_string().c_str());
    // printf("temp2\n%s\n", temp2.to_string().c_str());
    // printf("temp3\n%s\n", temp3.to_string().c_str());
    // printf("temp4\n%s\n", temp4.to_string().c_str());
    // printf("temp5\n%s\n", temp5.to_string().c_str());
    // printf("temp7\n%s\n", temp7.to_string().c_str());
    delete temp8;
    return 0;
}
