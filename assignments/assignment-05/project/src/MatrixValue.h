// 행렬을 연속으로 곱하면서 커지는 정수를 부호와 9자리 묶음으로 나누어 저장한다.
// 이번 과제의 최대 범위에 맞춘 자료형이며, 임의 크기의 정수를 처리하는 용도는 아니다.
#pragma once

#include <array>
#include <cstdint>
#include <string>

class MatrixValue {
public:
    // 기본값은 0이다. int를 받는 생성자는 검사를 마친 입력 원소(-3~3)에 사용한다.
    MatrixValue() = default;
    explicit MatrixValue(int value) noexcept;

    // 현재 합계에 value * factor를 더한다. factor는 입력 행렬의 -3~3이다.
    // 왼쪽부터 차례로 곱하므로 오른쪽 행렬은 항상 원래의 작은 입력값이다.
    // 큰 정수끼리의 곱셈 대신 이 연산만 구현해 필요한 기능으로 범위를 좁혔다.
    // 입력 개수와 크기 제한을 지키는 계산에서는 자리 수가 넘치지 않으며, 예외를 던지지 않는다.
    void AddProduct(const MatrixValue& value, int factor) noexcept;
    // 부호를 포함한 십진수 문자열을 돌려준다. 화면과 파일이 같은 값을 출력할 때 사용한다.
    std::string ToString() const;

private:
    // p개 입력의 결과 한 칸은 절댓값이 최대 3^p * 100^(p-1)이다.
    // 단계마다 최대 100개의 곱을 더하므로, 10개일 때 최대치는 다음과 같다.
    // 3^10 * 100^9 = 59049000000000000000000.
    // 64비트 정수에는 들어가지 않지만, 9자리 묶음 세 개(27자리)에는 들어간다.
    static constexpr std::uint32_t kBase = 1000000000;
    // digits_[0]이 가장 낮은 9자리다. 각 묶음은 항상 0 이상 kBase 미만이다.
    std::array<std::uint32_t, 3> digits_{};
    // -1은 음수, 0은 영, 1은 양수다. 부호가 0이면 모든 자리도 0으로 둔다.
    int sign_ = 0;
};
