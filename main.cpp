#include "lin_al.h"
#include <cstdio>
int main() {
    Matrix<5, 5, float> temp(1.121);
    Vector<5, float> temp2(12.31f);
    Matrix check = temp2;
    Matrix temp3 = temp * temp2;
    Vector<5, float> temp4(1.2f);
    Matrix<5, 5, float> temp5([](size_t _r, size_t _c) {return rand()*1.0f / RAND_MAX;});
    Matrix temp6 = temp5 * temp;
    float d = temp4.dot(temp2);
    printf("Dot: %.3f\n", d);
    printf("Len: %.3f\n", temp4.length());
    printf("Check\n%s\n", check.to_string().c_str());
    printf("Temp3\n%s\n", temp3.to_string().c_str());
    printf("Temp2\n%s\n", temp2.to_string().c_str());
    printf("Temp\n%s\n", temp.to_string().c_str());
    printf("Temp5\n%s\n", temp5.to_string().c_str());
    printf("Temp6\n%s\n", temp6.to_string().c_str());
    return 0;
}
