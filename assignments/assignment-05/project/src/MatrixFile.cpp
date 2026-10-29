// 제공된 샘플과 같이 공백으로 열을, 줄바꿈으로 행을 구분한다.
#include "MatrixFile.h"

#include <Windows.h>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

namespace {

// 새 결과를 쓰는 동안 이전 결과를 유지한다. 임시 파일은 결과와 같은 폴더에
// 생성하며, 저장이나 교체에 실패해도 이 객체가 자신이 만든 임시 파일을 정리한다.
class TemporaryResult {
public:
    explicit TemporaryResult(const std::filesystem::path& destination) {
        const auto folder = destination.has_parent_path() ? destination.parent_path()
                                                         : std::filesystem::path(L".");
        // 마지막 인수 앞의 0은 이름만 정하지 않고 빈 파일까지 새로 만들라는 뜻이다.
        // 기존 파일과 이름이 겹치면 Windows가 다른 이름을 선택한다.
        if (GetTempFileNameW(folder.c_str(), L"mm", 0, name_) == 0) {
            throw std::system_error(static_cast<int>(GetLastError()),
                std::system_category(), "Cannot create a temporary result file");
        }
    }

    TemporaryResult(const TemporaryResult&) = delete;
    TemporaryResult& operator=(const TemporaryResult&) = delete;

    ~TemporaryResult() noexcept {
        if (name_[0] != L'\0') {
            DeleteFileW(name_);
        }
    }

    const wchar_t* Name() const noexcept { return name_; }

    void Publish(const std::filesystem::path& destination) {
        // 같은 폴더에서 완성된 파일을 교체한다. 이전 결과를 먼저 비우지 않으므로
        // 쓰기 실패나 교체 권한 오류가 나도 이전 파일은 그대로 남는다.
        if (!MoveFileExW(name_, destination.c_str(),
            MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
            throw std::system_error(static_cast<int>(GetLastError()),
                std::system_category(), "Cannot replace result.txt");
        }
        name_[0] = L'\0';
    }

private:
    wchar_t name_[MAX_PATH]{};
};

} // namespace

InputMatrix ReadMatrix(const std::filesystem::path& filename) {
    // 파일 이름에 한글이나 공백이 있어도 경로를 그대로 열 수 있도록 path를 받는다.
    std::ifstream input(filename);
    if (!input) {
        throw std::runtime_error("Cannot open an input matrix file.");
    }

    // 크기 정보가 따로 없으므로 우선 줄 단위로 읽는다. 모든 행의 길이가 같은지
    // 확인한 다음 정확한 크기의 행렬로 옮겨, 불완전한 입력이 계산에 넘어가지 않게 한다.
    std::vector<std::vector<int>> rows;
    std::string line;
    while (std::getline(input, line)) {
        // 첫 행을 읽기 전에 UTF-8 파일 시작 표시(BOM)가 있으면 제거한다.
        // 이 표시를 숫자의 일부로 해석해 정상 파일을 거부하지 않기 위한 처리다.
        if (rows.empty() && line.compare(0, 3, "\xEF\xBB\xBF") == 0) {
            line.erase(0, 3);
        }
        std::istringstream fields(line);
        std::vector<int> row;
        std::string token;
        while (fields >> token) {
            // 공백으로 나눈 항목 전체가 정수여야 한다. '1.0'이나 '1abc'에서
            // 앞의 1만 읽고 정상값으로 받아들이지 않도록 끝까지 읽었는지도 확인한다.
            std::istringstream number(token);
            int value = 0;
            if (!(number >> value) || !number.eof() || value < -3 || value > 3) {
                throw std::runtime_error("Input entries must be integers from -3 to 3.");
            }
            row.push_back(value);
            if (row.size() > kMaxMatrixSize) {
                throw std::runtime_error("An input matrix has more than 100 columns.");
            }
        }

        // 빈 줄은 행렬의 행으로 세지 않는다. 끝에 빈 줄이 있어도 크기가 바뀌지 않는다.
        if (row.empty()) {
            continue;
        }
        if (!rows.empty() && row.size() != rows.front().size()) {
            throw std::runtime_error("All rows must have the same number of entries.");
        }
        rows.push_back(std::move(row));
        if (rows.size() > kMaxMatrixSize) {
            throw std::runtime_error("An input matrix has more than 100 rows.");
        }
    }
    // 정상적인 파일 끝과 실제 읽기 오류를 구분한다.
    if (input.bad()) {
        throw std::runtime_error("Failed while reading an input matrix file.");
    }
    if (rows.empty()) {
        throw std::runtime_error("An input matrix is empty.");
    }

    InputMatrix matrix(rows.size(), rows.front().size());
    for (std::size_t i = 0; i < matrix.Rows(); ++i) {
        for (std::size_t j = 0; j < matrix.Columns(); ++j) {
            matrix.At(i, j) = rows[i][j];
        }
    }
    return matrix;
}

std::string FormatMatrix(const ResultMatrix& matrix) {
    // 가장 긴 값보다 한 칸 넓게 잡아 큰 수와 음수도 열을 맞춰 출력한다.
    // 먼저 모든 값의 길이를 확인하므로 뒤쪽 행에 큰 값이 나와도 정렬이 유지된다.
    std::size_t width = 5;
    std::vector<std::string> cells;
    cells.reserve(matrix.Rows() * matrix.Columns());
    for (std::size_t i = 0; i < matrix.Rows(); ++i) {
        for (std::size_t j = 0; j < matrix.Columns(); ++j) {
            cells.push_back(matrix.At(i, j).ToString());
            width = std::max(width, cells.back().size() + 1);
        }
    }

    // 정수에서 문자열로 바꾸는 일은 위에서 한 번씩 끝냈다. 같은 문자열을 정렬해
    // 화면과 파일에 함께 사용하므로 출력할 때마다 변환을 반복하지 않는다.
    std::ostringstream output;
    for (std::size_t i = 0; i < matrix.Rows(); ++i) {
        for (std::size_t j = 0; j < matrix.Columns(); ++j) {
            output << std::setw(static_cast<int>(width)) << cells[i * matrix.Columns() + j];
        }
        output << '\n';
    }
    if (!output) {
        throw std::runtime_error("Cannot format the result matrix.");
    }
    return output.str();
}

void SaveMatrix(const std::string& text, const std::filesystem::path& filename) {
    TemporaryResult temporary(filename);
    std::ofstream output(std::filesystem::path(temporary.Name()));
    if (!output) {
        throw std::runtime_error("Cannot open a temporary result file for writing.");
    }
    // 시간이나 제목을 파일에 넣지 않아 입력과 같은 순수 행렬 형식을 유지한다.
    output << text;
    // 파일을 닫을 때 남아 있던 내용이 기록되므로, 닫은 뒤에도 실패 여부를 확인한다.
    output.close();
    if (!output) {
        throw std::runtime_error("Failed to finish writing result.txt.");
    }
    temporary.Publish(filename);
}
