// Python이 보내는 연산을 수행하고 결과를 돌려준다. 정답은 Python 큰 정수로 따로 구한다.
#include "../src/MatrixValue.h"

#include <array>
#include <iostream>

int main() {
    std::array<MatrixValue, 32> values;
    for (std::size_t i = 0; i < values.size(); ++i) {
        values[i] = MatrixValue(static_cast<int>(i % 7) - 3);
    }
    std::size_t destination = 0;
    std::size_t source = 0;
    int factor = 0;
    while (std::cin >> destination >> source >> factor) {
        if (destination >= values.size() || source >= values.size() || factor < -3 || factor > 3) {
            return 1;
        }
        values[destination].AddProduct(values[source], factor);
        std::cout << values[destination].ToString() << '\n';
    }
    return std::cin.eof() && std::cout ? 0 : 1;
}
