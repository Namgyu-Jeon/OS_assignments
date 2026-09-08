// 이 파일은 명령행 인수와 재입력 값이 올바른 양의 정수인지 검사하는 역할을 합니다.
// 숫자가 아닌 문자, 소수점, 부호, 공백, 0과 정수 범위 초과를 걸러냅니다.

#pragma once

#include <optional>
#include <string>

// 문자열 전체를 양의 int 정수로 바꾸는 검사 전용 클래스입니다.
// 한 가지 규칙을 명령행과 재입력에 공통으로 적용해 두 입력 경로의 동작 차이를 없앱니다.
class PositiveIntegerParser {
public:
    // 문자열이 ASCII 숫자만으로 이루어진 1 이상 int인지 검사하고 변환합니다.
    // text는 사용자가 입력한 전체 문자열이고, 성공하면 양의 정수, 실패하면 값이 없는 optional을 반환합니다.
    // 변환 중 범위를 먼저 검사해 매우 큰 숫자가 들어와도 오버플로 없이 안전하게 실패하도록 합니다.
    static std::optional<int> Parse(const std::string& text);
};
