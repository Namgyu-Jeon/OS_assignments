// 이 파일은 프로그램의 실행 순서와 오류 후 재입력을 담당합니다.
// 유효한 명령행 인수는 바로 렌더러로 보내고, 잘못된 시작은 안전하게 다시 입력받습니다.

#include "MultiplicationTableApp.h"

#include "input/PositiveIntegerParser.h"
#include "output/TableRenderer.h"

#include <iostream>
#include <string>

// 오류 메시지와 실행 예시를 출력합니다.
// output은 메시지를 보낼 스트림이며 반환값은 없습니다.
// 모든 잘못된 입력에 같은 기준을 알리면 재입력 때 사용자가 기대 형식을 쉽게 확인할 수 있습니다.
void MultiplicationTableApp::PrintInputError(std::ostream& output) const {
    output << "오류: 최대 단수는 1 이상의 정수 하나만 입력할 수 있습니다.\n"
           << "예: multi.exe 8\n\n";
}

// 오류가 발생한 뒤 표준 입력에서 올바른 단수 또는 종료 요청을 받습니다.
// input은 사용자가 입력한 한 줄을 읽는 스트림이고 output은 프롬프트와 오류를 보낼 스트림입니다.
// 유효한 양의 정수는 반환하고, q/Q 또는 입력 끝은 값이 없는 optional로 반환합니다.
std::optional<int> MultiplicationTableApp::ReadMaximumTable(
    std::istream& input,
    std::ostream& output) const {
    while (true) {
        output << "다시 입력할 최대 단수 (종료: q): ";

        std::string line;
        if (!std::getline(input, line)) {
            // 표준 입력이 더 이상 없으면 대화형 프로그램을 오류 없이 끝낼 수 있어야 합니다.
            return std::nullopt;
        }

        // q와 Q는 숫자 형식과 별도로 약속한 종료 명령이다.
        // 숫자 검증 전에 확인해야 종료 요청을 오류 입력으로 되묻지 않습니다.
        if (line == "q" || line == "Q") {
            return std::nullopt;
        }

        const std::optional<int> maximumTable = PositiveIntegerParser::Parse(line);
        if (maximumTable.has_value()) {
            return maximumTable;
        }

        // 잘못된 재입력도 같은 기준으로 안내한 뒤 반복해야 사용자가 성공할 때까지 회복할 수 있습니다.
        PrintInputError(output);
    }
}

// 명령행 인수부터 표 렌더링까지 프로그램의 흐름을 실행합니다.
// argc와 argv는 운영체제가 준 인수 개수와 문자열 목록이고, 반환값 0은 정상 종료를 뜻합니다.
// 인수가 정확히 하나이고 양의 정수일 때만 안내 문구 없이 즉시 표를 출력합니다.
int MultiplicationTableApp::Run(int argc, char* argv[]) const {
    // 프로그램 이름을 제외한 인수가 하나인지 먼저 검사한다.
    // 인수가 없거나 둘 이상이면 어떤 값을 최대 단수로 쓸지 확정할 수 없으므로 재입력으로 전환합니다.
    if (argc == 2) {
        const std::optional<int> maximumTable = PositiveIntegerParser::Parse(argv[1]);
        if (maximumTable.has_value()) {
            TableRenderer renderer;
            renderer.Render(*maximumTable, std::cout);
            return 0;
        }
    }

    PrintInputError(std::cout);
    const std::optional<int> maximumTable = ReadMaximumTable(std::cin, std::cout);
    if (!maximumTable.has_value()) {
        // q/Q 또는 입력 끝은 사용자가 요청한 정상 종료이므로 오류 코드가 아닙니다.
        return 0;
    }

    TableRenderer renderer;
    renderer.Render(*maximumTable, std::cout);
    return 0;
}
