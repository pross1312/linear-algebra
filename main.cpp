#include "Matrix.h"
#include <cstdio>
int main() {
    Matrix<float> temp(5, 5, 1.121);
    Vector<float> temp2(5, 12.31f);
    Matrix temp3 = temp * temp2;
    Vector<float> temp4(5, 1.2f);
    Matrix<float> temp5(5, 5, [](size_t r, size_t c) {return rand()*1.0f / RAND_MAX;});
    Matrix<float> temp6 = temp5 * temp;
    float d = temp4.dot(temp2);
    printf("Dot: %.3f\n", d);
    printf("Len: %.3f\n", temp4.length());
    printf("Temp\n%s\n", temp3.to_string().c_str());
    printf("Temp\n%s\n", temp2.to_string().c_str());
    printf("Temp\n%s\n", temp.to_string().c_str());
    printf("Temp\n%s\n", temp5.to_string().c_str());
    printf("Temp\n%s\n", temp6.to_string().c_str());
    return 0;
}
