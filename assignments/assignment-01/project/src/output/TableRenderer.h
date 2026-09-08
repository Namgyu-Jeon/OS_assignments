// 이 파일은 구구단을 5단씩 가로로 정렬해 출력하는 역할을 선언합니다.
// 전체 최대 단수의 자릿수로 공통 열 너비를 계산해 모든 위아래 묶음의 위치를 맞춥니다.

#pragma once

#include <iosfwd>

// 구구단의 묶음과 열 정렬을 책임지는 출력 클래스입니다.
// 입력 검증과 분리해 두면 표 모양을 바꾸더라도 입력 규칙에 영향을 주지 않습니다.
class TableRenderer {
public:
    // 한 가로 묶음에 넣는 단의 개수입니다.
    // 매직 넘버 대신 이름 있는 상수를 써서 5단 묶음이라는 과제 규칙을 코드에서 명확히 드러냅니다.
    static constexpr int kTablesPerGroup = 5;

    // 1단부터 maximumTable단까지의 삼각형 구구단을 output으로 보냅니다.
    // maximumTable은 이미 검증된 1 이상의 최대 단수이고 output은 콘솔 등 결과를 받을 스트림입니다.
    // 반환값은 없으며, 전체 표의 공통 열 너비와 묶음별 빈 칸을 계산해 읽기 쉬운 표를 만듭니다.
    void Render(int maximumTable, std::ostream& output) const;
};
