// 이 파일은 5단씩 묶인 삼각형 구구단의 문자열 생성과 열 정렬을 담당합니다.
// 끝난 단에는 같은 폭의 공백을 넣어 오른쪽 열이 아래 행에서도 같은 위치를 유지하게 합니다.

#include "TableRenderer.h"

#include <algorithm>
#include <array>
#include <charconv>
#include <ostream>
#include <string>

namespace {

// 전체 표에서 모든 묶음이 공통으로 사용할 숫자 폭과 한 열의 너비를 보관합니다.
// 최대 단수로 한 번만 계산한 값을 재사용해야 위아래 묶음이 같은 다섯 개의 세로선을 공유합니다.
struct TableLayout {
    std::size_t tableWidth;
    std::size_t multiplierWidth;
    std::size_t productWidth;
    std::size_t columnWidth;
};

// 정수를 현재 행 문자열에 지정한 최소 폭으로 오른쪽 정렬해 이어 붙입니다.
// value는 단 번호·행 번호·곱이고 minimumWidth는 확보할 자릿수이며 반환값은 없습니다.
// 실제 숫자보다 폭이 넓으면 앞에 공백을 채워, 자릿수가 달라져도 같은 열의 연산 기호 위치를 맞춥니다.
// 64비트 정수도 담을 수 있는 고정 버퍼를 사용하므로 별도의 숫자 문자열이나 스트림 객체가 필요하지 않습니다.
void AppendNumber(std::string& target, long long value, std::size_t minimumWidth = 0) {
    std::array<char, 32> buffer{};
    const auto conversion = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
    const std::size_t length = static_cast<std::size_t>(conversion.ptr - buffer.data());
    if (minimumWidth > length) {
        target.append(minimumWidth - length, ' ');
    }
    target.append(buffer.data(), conversion.ptr);
}

// 정수의 십진수 표현이 몇 글자인지 고정 버퍼에서 계산합니다.
// value는 길이를 구할 정수이고, 반환값은 부호를 포함한 실제 문자 수입니다.
// 열 너비만 필요할 때 완성된 임시 문자열을 만들지 않기 위해 사용합니다.
std::size_t NumberLength(long long value) {
    std::array<char, 32> buffer{};
    const auto conversion = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
    return static_cast<std::size_t>(conversion.ptr - buffer.data());
}

// 한 곱셈식을 현재 행 문자열에 바로 이어 붙입니다.
// target은 작성 중인 한 행이고 table과 multiplier는 각각 단과 현재 행입니다.
// layout은 전체 표에 공통으로 적용할 단·승수·결과 자릿수이며 반환값은 없습니다.
// 곱은 long long으로 계산해 int 범위의 단수끼리 곱해도 중간 계산이 넘치지 않게 합니다.
// 칸마다 별도 string을 만들지 않아 출력량이 클 때 반복되는 임시 메모리 할당을 줄입니다.
void AppendExpression(
    std::string& target,
    int table,
    int multiplier,
    const TableLayout& layout) {
    const long long product = static_cast<long long>(table) * multiplier;
    AppendNumber(target, table, layout.tableWidth);
    target += " x ";
    AppendNumber(target, multiplier, layout.multiplierWidth);
    target += " = ";
    AppendNumber(target, product, layout.productWidth);
}

// 최대 단수를 이용해 전체 표에 적용할 공통 열 배치를 계산합니다.
// maximumTable은 출력할 마지막 단이고, 반환값은 모든 묶음이 공유할 TableLayout입니다.
// 단과 승수는 N의 자릿수, 결과는 N*N의 자릿수를 사용해야 윗묶음과 아랫묶음의 위치가 일치합니다.
TableLayout MakeLayout(int maximumTable) {
    const long long maximumProduct =
        static_cast<long long>(maximumTable) * maximumTable;
    const std::size_t tableWidth = NumberLength(maximumTable);
    const std::size_t multiplierWidth = tableWidth;
    const std::size_t productWidth = NumberLength(maximumProduct);
    const std::size_t columnWidth = tableWidth + 3 + multiplierWidth + 3 + productWidth;
    return {tableWidth, multiplierWidth, productWidth, columnWidth};
}

}  // namespace

// 최대 단수까지의 구구단을 5단 묶음으로 렌더링합니다.
// maximumTable은 1 이상으로 검사된 값이고 output은 결과를 받을 스트림이며 반환값은 없습니다.
// 묶음별 열 정보를 아주 작게만 보관하고 각 식을 필요할 때 만들어 큰 표도 불필요한 전체 저장 없이 처리합니다.
void TableRenderer::Render(int maximumTable, std::ostream& output) const {
    // 실행 명령 바로 아래에 표가 붙어 보이지 않도록 첫 행 전에 두 줄을 비웁니다.
    // 제목이나 안내 문구는 추가하지 않고 숫자 표와 터미널 프롬프트 사이의 시각적 간격만 확보합니다.
    output << "\n\n";

    // 최대 단수에서 계산한 하나의 배치를 모든 5단 묶음에 재사용한다.
    // 단마다 열 너비를 따로 쓰면 1~5단과 6~10단의 시작 위치가 달라지므로 공통 배치가 필요합니다.
    const TableLayout layout = MakeLayout(maximumTable);
    int firstTable = 1;

    while (true) {
        // 남은 단이 5개보다 적을 수 있으므로 현재 묶음의 끝을 최대 단수를 넘지 않게 계산한다.
        // maximumTable - firstTable 비교를 사용해 int 최대값 근처에서도 덧셈 오버플로를 피합니다.
        const int lastTable = firstTable > maximumTable - (kTablesPerGroup - 1)
                                  ? maximumTable
                                  : firstTable + (kTablesPerGroup - 1);

        std::array<int, kTablesPerGroup> columns{};
        int columnCount = 0;
        for (int table = firstTable;; ++table) {
            // 현재 묶음에 들어갈 단 번호만 저장하고, 너비는 전체 표의 공통 layout을 사용합니다.
            columns[static_cast<std::size_t>(columnCount)] = table;
            ++columnCount;
            // 마지막 단을 처리한 뒤에는 증가 연산을 하지 않고 끝낸다.
            // maximumTable이 int 최대값이어도 table++에서 정수 범위가 넘지 않게 합니다.
            if (table == lastTable) {
                break;
            }
        }

        // 단 d는 d번째 행에서 끝나므로 현재 묶음의 가장 큰 단이 필요한 전체 행 수를 결정한다.
        // 이 반복이 1, 2, 3 ... 행으로 늘어나는 삼각형 구조를 자연스럽게 처리합니다.
        std::size_t rowWidth = 1;
        for (int index = 0; index < columnCount; ++index) {
            rowWidth += layout.columnWidth;
        }
        rowWidth += static_cast<std::size_t>(columnCount - 1) * 3;

        for (int row = 1;; ++row) {
            // 한 행을 작은 문자열에 먼저 모은 뒤 한 번에 스트림에 기록한다.
            // 단마다 여러 번 호출하던 포맷팅 연산을 줄이면서도 전체 표를 메모리에 쌓지는 않습니다.
            std::string rowText;
            rowText.reserve(rowWidth);

            for (int index = 0; index < columnCount; ++index) {
                const int table = columns[static_cast<std::size_t>(index)];

                if (row <= table) {
                    // 완성된 칸 문자열을 따로 만들지 않고 현재 행 버퍼에 식을 직접 추가한다.
                    // 추가 전후의 길이 차이로 실제 칸 길이를 구해 남은 열 폭만큼만 공백을 채웁니다.
                    const std::size_t cellStart = rowText.size();
                    AppendExpression(
                        rowText,
                        table,
                        row,
                        layout);
                    const std::size_t cellLength = rowText.size() - cellStart;
                    rowText.append(layout.columnWidth - cellLength, ' ');
                } else {
                    // 현재 행 번호가 이 단보다 크면 해당 단은 이미 출력이 끝난 상태이다.
                    // 같은 폭의 빈 공간을 출력해 오른쪽 단의 위치가 흔들리지 않도록 합니다.
                    rowText.append(layout.columnWidth, ' ');
                }

                if (index + 1 < columnCount) {
                    // 열 사이의 고정 여백은 칸 경계를 읽기 쉽게 만들되 표 테두리는 출력하지 않습니다.
                    rowText += "   ";
                }
            }
            rowText.push_back('\n');
            output.write(rowText.data(), static_cast<std::streamsize>(rowText.size()));

            // 마지막 행을 출력한 뒤에는 row를 증가시키지 않는다.
            // 이 조건도 최대 단수가 int 최대값일 때 반복 변수의 불필요한 오버플로를 막습니다.
            if (row == lastTable) {
                break;
            }
        }

        if (lastTable == maximumTable) {
            break;
        }

        // 다음 5단 묶음 앞에는 한 줄을 비워 PDF 예시처럼 묶음 경계를 분명히 합니다.
        output << '\n';
        firstTable = lastTable + 1;
    }

    // 마지막 행은 줄바꿈 하나로 끝나므로 두 번 더 줄을 바꾸면 표 아래에 빈 줄 두 개가 생깁니다.
    // 프로그램 종료 뒤 표시되는 PowerShell 또는 cmd 프롬프트와 결과가 붙지 않아 읽기 쉬워집니다.
    output << "\n\n";
}
