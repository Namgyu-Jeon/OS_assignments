// 입력 순서를 유지해 결과를 보여 주고, 전체 대기 시간을 프로세스 수로 나눈다.
#include "SchedulePrinter.h"

#include <iomanip>
#include <locale>
#include <ostream>
#include <sstream>
#include <stdexcept>

void PrintSchedule(const std::vector<ProcessInfo>& processes,
    const std::vector<SimulationTime>& waitingTimes, std::ostream& output) {
    // 전체 내용을 먼저 완성해 계산이나 문자열 처리 실패 때 일부 결과만 남기지 않는다.
    std::ostringstream text;
    text.imbue(std::locale::classic());
    text << '\n';
    for (const auto& process : processes) {
        text << process.name << "\tARRIVAL:" << process.arrival;
        for (std::size_t i = 0; i < process.bursts.size(); ++i) {
            text << (i % 2 == 0 ? "\tCPU:" : "\tI/O:") << process.bursts[i];
        }
        text << '\n';
    }

    constexpr const char* separator = "------------------------------\n";
    text << "\nProcess : Waiting Time\n" << separator;
    long double total = 0;
    for (std::size_t i = 0; i < processes.size(); ++i) {
        text << processes[i].name << "\t: " << std::setw(3) << waitingTimes[i] << '\n';
        total += static_cast<long double>(waitingTimes[i]);
    }
    // CPU 구간 개수가 아니라 프로세스 수로 나눈다. 큰 합계도 정수 덧셈을 넘지 않는다.
    const long double average = total / static_cast<long double>(processes.size());
    text << separator << "Average Waiting Time  = "
         << std::fixed << std::setprecision(2) << std::setw(5) << average << "\n\n";
    if (!text) {
        throw std::runtime_error("Cannot format the scheduling result.");
    }
    output << text.str() << std::flush;
    if (!output) {
        throw std::runtime_error("Cannot print the scheduling result.");
    }
}
