// 입력 파일에 적힌 이름과 숫자를 읽어 자원 상태를 구성한다.
// >> 연산자는 공백, 탭, 줄바꿈을 건너뛰므로 칸 간격을 바꾸어도 같은 값으로 읽힌다.
#include "ResourceFile.h"

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

// 자원 개수와 프로세스 수에 공통으로 사용하는 정수 읽기 함수다.
// 누락된 값이나 음수를 읽었을 때는 오류를 알리고 중단한다.
// 초기값 0을 실제 입력으로 오인하지 않도록 즉시 오류를 발생시킨다.
ResourceCount ReadCount(std::istream& input) {
    ResourceCount value = 0;
    if (!(input >> value) || value < 0) {
        throw std::runtime_error("Expected a nonnegative resource or process count.");
    }
    return value;
}

// Allocation 등의 표 제목을 확인하고, 그 뒤의 자원 이름을 열 개수만큼 읽는다.
// 실제 이름을 보관해 두면 자원 종류가 늘어나도 A~J처럼 고정된 목록이 필요 없다.
std::vector<std::string> ReadHeader(std::istream& input,
    const std::string& expectedTitle, const std::size_t resourceCount) {
    std::string title;
    if (!(input >> title) || title != expectedTitle) {
        throw std::runtime_error("Expected the " + expectedTitle + " table.");
    }
    std::vector<std::string> names(resourceCount);
    for (auto& name : names) {
        if (!(input >> name)) {
            throw std::runtime_error("A table is missing a resource name.");
        }
    }
    return names;
}

// 자원 종류 수만큼 정수를 읽어 한 행으로 반환한다.
// Allocation과 Max의 각 프로세스 행, Available의 한 행을 모두 이 함수로 읽는다.
ResourceVector ReadResources(std::istream& input, const std::size_t resourceCount) {
    ResourceVector values(resourceCount);
    for (auto& value : values) {
        value = ReadCount(input);
    }
    return values;
}

} // namespace

// 파일을 읽는 순서는 과제의 입력 형식과 같다. 완성한 상태는 값으로 반환하며,
// 입력 파일 자체는 읽기 전용으로 열기 때문에 내용을 바꾸지 않는다.
ResourceState ReadResourceState(const std::filesystem::path& filename) {
    std::ifstream input(filename, std::ios::binary);
    if (!input) {
        throw std::runtime_error("Cannot open the input file.");
    }

    // 일부 편집기는 UTF-8 파일 맨 앞에 BOM이라는 3바이트 표시를 붙인다.
    // 표시가 있으면 건너뛰고, 없으면 파일 처음으로 되돌려 첫 숫자부터 읽는다.
    // 짧은 파일에서 read가 끝에 도달했을 수도 있어 clear로 읽기 상태도 초기화한다.
    char prefix[3]{};
    input.read(prefix, 3);
    const bool hasBom = std::string(prefix, 3) == "\xEF\xBB\xBF";
    input.clear();
    if (!hasBom) {
        input.seekg(0);
    }

    // 첫 숫자는 자원 종류 수(m), 두 번째 숫자는 프로세스 수(n)다.
    // 배열 크기를 이 값으로 정하므로 샘플마다 행과 열의 개수가 달라도 처리할 수 있다.
    const auto resourceCount = static_cast<std::size_t>(ReadCount(input));
    const auto processCount = static_cast<std::size_t>(ReadCount(input));
    if (resourceCount == 0) {
        throw std::runtime_error("There must be at least one resource type.");
    }

    // Allocation에서 프로세스 이름과 현재 할당량을 읽고 이후 표의 기준으로 삼는다.
    // reserve는 행을 추가할 공간을 미리 확보하고, resize는 이름 칸을 실제로 만든다.
    ResourceState state;
    state.resourceNames = ReadHeader(input, "Allocation", resourceCount);
    state.processNames.resize(processCount);
    state.allocation.reserve(processCount);
    for (auto& name : state.processNames) {
        if (!(input >> name)) {
            throw std::runtime_error("The Allocation table is missing a process name.");
        }
        state.allocation.push_back(ReadResources(input, resourceCount));
    }

    // Max의 열 이름과 행 이름이 Allocation과 같은 순서인지 확인한다.
    // 순서가 다르면 서로 다른 자원이나 프로세스의 값을 빼게 되므로 계산할 수 없다.
    if (ReadHeader(input, "Max", resourceCount) != state.resourceNames) {
        throw std::runtime_error("Resource names must match in all tables.");
    }
    state.maximum.reserve(processCount);
    for (std::size_t i = 0; i < processCount; ++i) {
        std::string name;
        if (!(input >> name) || name != state.processNames[i]) {
            throw std::runtime_error("Process names must match in Allocation and Max.");
        }
        state.maximum.push_back(ReadResources(input, resourceCount));
        for (std::size_t j = 0; j < resourceCount; ++j) {
            // 최대 필요량은 이미 가진 양보다 작을 수 없다. 두 값이 같으면
            // 해당 자원은 추가로 필요하지 않으므로 같다는 이유로 거부하지 않는다.
            if (state.maximum[i][j] < state.allocation[i][j]) {
                throw std::runtime_error("Max cannot be smaller than Allocation.");
            }
        }
    }

    // Available에는 프로세스 이름 없이 자원 종류별 남은 개수만 한 행으로 들어 있다.
    if (ReadHeader(input, "Available", resourceCount) != state.resourceNames) {
        throw std::runtime_error("Resource names must match in all tables.");
    }
    state.available = ReadResources(input, resourceCount);
    // 마지막 줄 뒤의 빈 줄은 허용한다. 숫자나 글자가 더 남으면 형식이 맞지 않는
    // 파일이므로, 앞부분만 읽고 정상 입력인 것처럼 처리하지 않는다.
    input >> std::ws;
    if (input.bad() || !input.eof()) {
        throw std::runtime_error("Unexpected data after the Available vector.");
    }
    return state;
}
