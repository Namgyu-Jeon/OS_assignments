// 입력 표를 다시 보여 주고, 안전성 검사 결과를 PDF와 제공된 정답 파일의 형식으로 출력한다.
// 계산은 하지 않으며 전달받은 값과 프로세스 순서를 화면에 배치하는 역할만 한다.
#include "StatePrinter.h"

#include <iomanip>
#include <locale>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

// 표 제목이나 프로세스 이름을 왼쪽 12칸에 놓고 자원별 값을 오른쪽에 정렬한다.
// 자원 이름 행과 숫자 행의 칸 간격을 맞추기 위해 두 경우에 같은 함수를 사용한다.
// setw는 최소 너비만 정하므로 긴 이름이나 큰 숫자가 들어와도 잘라내지는 않는다.
template <typename Values>
void PrintRow(std::ostream& output, const std::string& name, const Values& values) {
    output << std::left << std::setw(12) << name << std::right;
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0) {
            output << ' ';
        }
        output << std::setw(3) << values[i];
    }
    output << '\n';
}

// Allocation 또는 Max의 제목, 자원 이름, 프로세스별 값을 하나의 표로 출력한다.
// 계산 중 완료된 순서와 관계없이 원래 입력의 행 순서를 유지해 입력과 비교하기 쉽게 한다.
void PrintTable(std::ostream& output, const std::string& title,
    const ResourceTable& table, const ResourceState& state) {
    PrintRow(output, title, state.resourceNames);
    for (std::size_t i = 0; i < table.size(); ++i) {
        PrintRow(output, state.processNames[i], table[i]);
    }
    output << '\n';
}

} // namespace

// 화면 전체를 입력 상태, 두 표, Available, 판정 결과 순서로 구성한다.
void PrintSafetyResult(const ResourceState& state,
    const SafetyResult& result, std::ostream& output) {
    // 문자열을 만드는 도중 문제가 생겼을 때 일부 표만 먼저 출력되는 일을 줄이기 위해
    // 임시 문자열 스트림에 내용을 모은다. 완성된 뒤 실제 출력 대상으로 전달한다.
    std::ostringstream text;
    // 컴퓨터의 지역 설정이 달라도 숫자에 자릿수 구분 기호가 붙지 않게 한다.
    text.imbue(std::locale::classic());
    // 첫 줄은 자원 종류 수와 프로세스 수다. 출력 시작 전에는 빈 줄을 하나 둔다.
    text << '\n' << std::setw(5) << state.resourceNames.size() << ' '
         << std::setw(5) << state.processNames.size() << " \n\n";
    PrintTable(text, "Allocation", state.allocation, state);
    PrintTable(text, "Max", state.maximum, state);
    PrintRow(text, "Available", state.resourceNames);
    PrintRow(text, "", state.available);

    // 정답 예제와 같은 길이의 구분선을 사용한다. 미완료 목록이 비어 있는지에 따라
    // 상태와 출력할 목록을 함께 선택해 UnSafe에 일부 완료 순서가 섞이지 않게 한다.
    const std::string separator(82, '-');
    const bool isSafe = result.unsafeProcesses.empty();
    const auto& processes = isSafe ? result.safeSequence : result.unsafeProcesses;
    text << '\n' << separator << "\nResult: " << (isSafe ? "Safe" : "UnSafe") << " --> ";
    for (const auto index : processes) {
        text << state.processNames[index] << ' ';
    }
    // 마지막 구분선의 줄바꿈에 하나를 더 붙여 출력 뒤에도 빈 줄을 하나 남긴다.
    text << '\n' << separator << "\n\n";
    if (!text) {
        throw std::runtime_error("Cannot format the safety result.");
    }
    // flush로 모아 둔 출력을 내보내고, 출력 대상에 쓰지 못한 경우도 오류로 처리한다.
    output << text.str() << std::flush;
    if (!output) {
        throw std::runtime_error("Cannot print the safety result.");
    }
}
