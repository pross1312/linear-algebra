#include "lin_al.h"
#include "raylib.h"
#include <cstdio>
int main() {
    LIN::VectorX<float> temp(5, 0);
    LIN::MatrixXX<float> temp2(5, 5, 123.2123);
    LIN::MatrixXX<float> temp3 = temp2 * temp;
    LIN::VectorX<float> temp4(5, 123.1213);
    LIN::VectorX<float> temp5 {temp.dot(temp4)};
    printf("temp\n%s\n", temp.to_string().c_str());
    printf("temp2\n%s\n", temp2.to_string().c_str());
    printf("temp3\n%s\n", temp3.to_string().c_str());
    printf("temp4\n%s\n", temp4.to_string().c_str());
    printf("temp5\n%s\n", temp5.to_string().c_str());
    return 0;
}
