#include "lin_al.h"
#include <iostream>
int main() {
    LIN::MatrixXX<size_t> zeros(5, 5);
    LIN::MatrixXX<size_t> ones(5, 5, 1);
    LIN::MatrixXX<size_t> randoms(5, 5, []()->size_t { return rand(); });
    LIN::MatrixXX<size_t> alternating_zero_one(5, 5, [](size_t r, size_t c)->size_t { return (r + c) % 2 == 0 ? 1 : 0; });
    LIN::MatrixXX<size_t> identity(5, 5, [](size_t r, size_t c)->size_t { return r == c ? 1 : 0; });
    LIN::MatrixXX<size_t> sum{ zeros + randoms }; // 1 times or 2 times if elide-constructors kick in [use -fno-elide-constructors to disable]
    LIN::MatrixXX<size_t> mul{ identity * randoms };
    LIN::MatrixXX<size_t> transform{ zeros.transform([](size_t x)-> size_t { return x + 10; }) };
    LIN::MatrixXX<size_t> add_10{ LIN::MatrixXX<size_t>(5, 5) + 10 };

    std::cout << "ZEROS\n" << zeros << "\n";
    std::cout << "ONES\n" << ones << "\n";
    std::cout << "RANDOMS\n" << randoms << "\n";
    std::cout << "ALTERNATING_ZERO_ONE\n" << alternating_zero_one << "\n";
    std::cout << "IDENTITY\n" << identity << "\n";
    std::cout << "SUM\n" << sum << "\n";
    std::cout << "MUL\n" << mul << "\n";
    std::cout << "TRANSFORM\n" << transform << "\n";
    std::cout << "ADD_10\n" << add_10 << "\n";
    zeros = ones;
    std::cout << "ONES FROM ASSIGN\n" << zeros << "\n";

    if (sum == randoms) {
        std::cout << "Sum, Equal checked\n";
    }
    if (mul == randoms) {
        std::cout << "Mul checked\n";
    }
    if (add_10 == transform) {
        std::cout << "Add scarlar, tranforms checked\n";
    }
    LIN::VectorX<size_t> v_zeros(5);
    LIN::VectorX<size_t> v_ones(5, 1);
    LIN::VectorX<size_t> v_randoms(5, []()-> size_t { return rand(); });
    LIN::VectorX<size_t> v_alternating_zero_one(5, [](size_t i)-> size_t { return i%2 == 0 ? 1 : 0; });
    LIN::VectorX<size_t> v_sum{ v_ones + v_zeros };
    LIN::VectorX<size_t> v_transform{ v_zeros.transform([](size_t x)-> size_t { return x + 10; }) };
    LIN::VectorX<size_t> v_add_10{ LIN::VectorX<size_t>(5) + 10 };

    size_t dot = v_ones.dot(v_zeros);
    std::cout << "V_ZEROS\n" << v_zeros << "\n";
    std::cout << "V_ONES\n" << v_ones << "\n";
    std::cout << "V_RANDOMS\n" << v_randoms << "\n";
    std::cout << "V_ALTERNATING_ZERO_ONE\n" << v_alternating_zero_one << "\n";
    std::cout << "V_SUM\n" << v_sum << "\n";
    std::cout << "V_TRANSFORM\n" << v_transform << "\n";
    std::cout << "V_ADD_10\n" << v_add_10 << "\n";
    v_zeros = v_ones;
    std::cout << "V_ONES FROM ASSIGN\n" << v_zeros;

    if (dot == 0) {
        std::cout << "Dot check\n";
    }
    if (v_add_10 == v_transform) {
        std::cout << "Equal, transform, add scalar check\n";
    }
    LIN::MatrixXX<size_t> test_op(5, 5, [](size_t r, size_t c)->size_t { return r + 0 * c; });
    test_op[0] = test_op[1];
    std::cout << test_op << "\n";
    return 0;
}
