// 한 줄을 한 프로세스로 읽고, CPU와 I/O가 번갈아 나오는지 확인한다.
#include "ProcessFile.h"

#include <charconv>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>

namespace {

// 항목의 이름과 정수 전체를 확인한다. 숫자 뒤에 다른 글자가 붙으면 받지 않는다.
SimulationTime ReadTime(const std::string& token, const std::string& prefix) {
    if (token.compare(0, prefix.size(), prefix) != 0) {
        throw std::runtime_error("Unexpected input field; expected " + prefix);
    }
    SimulationTime value = 0;
    const char* first = token.data() + prefix.size();
    const char* last = token.data() + token.size();
    const auto parsed = std::from_chars(first, last, value);
    if (parsed.ec != std::errc{} || parsed.ptr != last || value < 0) {
        throw std::runtime_error("Time values must be nonnegative 64-bit integers.");
    }
    return value;
}

} // namespace

std::vector<ProcessInfo> ReadProcesses(const std::filesystem::path& filename) {
    // filesystem::path로 열어 한글이나 공백이 있는 파일 경로도 처리한다.
    std::ifstream input(filename);
    if (!input) {
        throw std::runtime_error("Cannot open the input file.");
    }

    std::vector<ProcessInfo> processes;
    std::string line;
    bool firstLine = true;
    while (std::getline(input, line)) {
        // UTF-8의 파일 시작 표시가 있으면 프로세스 이름에 섞이지 않게 제거한다.
        if (firstLine && line.compare(0, 3, "\xEF\xBB\xBF") == 0) {
            line.erase(0, 3);
        }
        firstLine = false;
        std::istringstream fields(line);
        ProcessInfo process;
        if (!(fields >> process.name)) {
            continue; // 빈 줄과 줄 끝의 공백은 프로세스로 세지 않는다.
        }

        std::string token;
        if (!(fields >> token)) {
            throw std::runtime_error("A process is missing its arrival time.");
        }
        process.arrival = ReadTime(token, "ARRIVAL:");
        while (fields >> token) {
            const bool isCpu = process.bursts.size() % 2 == 0;
            process.bursts.push_back(ReadTime(token, isCpu ? "CPU:" : "I/O:"));
        }
        if (process.bursts.empty() || process.bursts.size() % 2 == 0) {
            throw std::runtime_error("A burst sequence must start and end with CPU.");
        }
        processes.push_back(std::move(process));
    }
    if (input.bad()) {
        throw std::runtime_error("Failed while reading the input file.");
    }
    if (processes.empty()) {
        throw std::runtime_error("The input file contains no processes.");
    }
    return processes;
}
