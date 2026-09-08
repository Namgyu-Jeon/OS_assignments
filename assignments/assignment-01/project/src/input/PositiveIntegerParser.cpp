// 이 파일은 문자열 전체를 엄격한 양의 정수로 검사하고 변환하는 역할을 합니다.
// 숫자 이외의 문자와 범위 초과를 먼저 막아 잘못된 값이 표 출력 계산으로 전달되지 않게 합니다.

#include "PositiveIntegerParser.h"

#include <limits>

// 입력 문자열을 1 이상 int로 검사하고 변환합니다.
// text는 명령행 인수 또는 재입력 한 줄이며, 성공한 정수 또는 실패를 나타내는 optional을 반환합니다.
// 각 자리를 누적하기 전에 최대 int를 넘는지 계산하여 정수 오버플로를 일으키지 않습니다.
std::optional<int> PositiveIntegerParser::Parse(const std::string& text) {
    if (text.empty()) {
        // 빈 문자열은 숫자 하나로 구성된 입력이 아니므로 즉시 거부합니다.
        return std::nullopt;
    }

    int value = 0;
    for (const char character : text) {
        // ASCII '0'부터 '9'까지가 아닌 문자는 문자, 공백, 소수점, 부호, 쉼표를 모두 포함한다.
        // 문자열 전체가 숫자여야 하므로 하나라도 있으면 부분 변환하지 않고 거부합니다.
        if (character < '0' || character > '9') {
            return std::nullopt;
        }

        const int digit = character - '0';
        // value * 10 + digit를 하기 전에 가능한 최대값을 넘는지 확인한다.
        // 이 순서가 필요한 이유는 계산 자체가 int 범위를 넘으면 잘못된 값을 만들 수 있기 때문입니다.
        if (value > (std::numeric_limits<int>::max() - digit) / 10) {
            return std::nullopt;
        }

        value = value * 10 + digit;
    }

    // 0은 양의 정수가 아니며, 음수는 앞의 부호 문자 검사에서 이미 거부됩니다.
    if (value == 0) {
        return std::nullopt;
    }

    return value;
}
