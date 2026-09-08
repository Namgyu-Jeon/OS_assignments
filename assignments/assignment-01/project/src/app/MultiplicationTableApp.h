// 이 파일은 구구단 프로그램의 전체 실행 흐름을 선언하는 역할을 합니다.
// 명령행 인수 검사, 오류 뒤 재입력, 표 렌더링의 순서를 한곳에서 관리합니다.

#pragma once

#include <iosfwd>
#include <optional>

// 명령행 인수와 표 출력 모듈을 연결하는 응용 프로그램 클래스입니다.
// 올바른 값이면 즉시 표를 출력하고, 잘못된 값이면 사용자가 종료하거나 올바른 값을 줄 때까지 다시 받습니다.
class MultiplicationTableApp {
public:
    // 프로그램 전체를 실행합니다.
    // argc와 argv는 운영체제가 전달한 명령행 인수 개수와 문자열 목록이며, 반환값은 운영체제에 돌려줄 종료 코드입니다.
    // 인수 개수를 먼저 확인해야 "하나의 양의 정수"라는 약속을 지키고 잘못된 시작도 재입력으로 회복할 수 있습니다.
    int Run(int argc, char* argv[]) const;

private:
    // 오류 이유와 올바른 실행 예시를 표준 출력에 보여 줍니다.
    // 반환값은 없으며, 사용자가 다음 입력에서 형식을 바로잡을 수 있도록 같은 안내를 일관되게 사용합니다.
    void PrintInputError(std::ostream& output) const;

    // 표준 입력에서 한 줄씩 읽어 양의 정수를 찾거나 종료 요청을 처리합니다.
    // 입력 스트림과 안내를 보낼 출력 스트림을 받고, 유효한 단수는 optional<int>로 반환합니다.
    // q 또는 Q, 혹은 입력 끝을 만나면 값이 없는 optional을 반환해 정상 종료를 알립니다.
    std::optional<int> ReadMaximumTable(std::istream& input, std::ostream& output) const;
};
