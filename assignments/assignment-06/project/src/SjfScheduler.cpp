// 도착, I/O 완료, CPU 구간 종료 시각으로 이동하며 스케줄링을 시뮬레이션한다.
// 실제로 잠들거나 운영체제 프로세스를 만들지 않고 가상 시간으로 계산한다.
#include "SjfScheduler.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <vector>

namespace {

// 원래 입력과 분리한 실행 상태다. burstIndex는 현재 CPU 구간을 가리킨다.
struct ProcessState {
    std::size_t burstIndex = 0;
    SimulationTime remaining = 0;
    SimulationTime readySince = 0;
    SimulationTime waitingTime = 0;
};

// I/O는 서로 기다리지 않는다. 각 프로세스가 자기 완료 시각을 따로 갖는다.
struct IoCompletion {
    std::size_t processIndex;
    SimulationTime time;
};

// 큰 입력에서도 시간이 음수로 바뀌지 않도록 덧셈 전에 표현 범위를 확인한다.
SimulationTime AddTime(const SimulationTime left, const SimulationTime right) {
    if (right > std::numeric_limits<SimulationTime>::max() - left) {
        throw std::overflow_error("The simulation time exceeds the 64-bit range.");
    }
    return left + right;
}

} // namespace

std::vector<SimulationTime> SimulateSjf(const std::vector<ProcessInfo>& processes) {
    std::vector<ProcessState> states(processes.size());
    std::vector<std::size_t> arrivals(processes.size());
    std::iota(arrivals.begin(), arrivals.end(), std::size_t{0});
    // 도착 시각만 정렬한다. 시각이 같으면 입력 파일의 줄 순서를 유지한다.
    std::stable_sort(arrivals.begin(), arrivals.end(),
        [&processes](const std::size_t left, const std::size_t right) {
            return processes[left].arrival < processes[right].arrival;
        });
    for (std::size_t i = 0; i < processes.size(); ++i) {
        states[i].remaining = processes[i].bursts.front();
    }

    std::vector<std::size_t> ready;
    std::vector<IoCompletion> waiting;
    std::optional<std::size_t> running; // 값이 없으면 CPU가 비어 있다.
    std::size_t nextArrival = 0;
    std::size_t finished = 0;
    SimulationTime now = 0;

    // 대기열에 들어온 시각을 기억하고, 나중에 CPU를 받을 때 차이를 더한다.
    const auto enterReady = [&](const std::size_t index) {
        states[index].readySince = now;
        ready.push_back(index);
    };

    while (finished < processes.size()) {
        // 같은 시각의 진입 순서 1: New에서 온 프로세스를 먼저 넣는다.
        while (nextArrival < arrivals.size() &&
            processes[arrivals[nextArrival]].arrival == now) {
            enterReady(arrivals[nextArrival++]);
        }

        // 진입 순서 2: I/O를 끝낸 프로세스를 넣는다. 동시에 끝나면
        // 먼저 I/O를 시작한 순서를 유지한다. I/O 장치를 직렬로 처리하지 않는다.
        for (auto item = waiting.begin(); item != waiting.end();) {
            if (item->time == now) {
                enterReady(item->processIndex);
                item = waiting.erase(item);
            }
            else {
                ++item;
            }
        }

        // 남은 CPU 시간이 같으면 대기열에 먼저 들어온 프로세스를 고른다.
        // min_element는 첫 최솟값을 반환하므로 별도의 이름순 정렬을 하지 않는다.
        const auto shortest = std::min_element(ready.begin(), ready.end(),
            [&states](const std::size_t left, const std::size_t right) {
                return states[left].remaining < states[right].remaining;
            });
        if (running && shortest != ready.end() &&
            states[*shortest].remaining < states[*running].remaining) {
            // 진입 순서 3: 선점당한 Running은 새 도착과 I/O 완료 뒤에 넣는다.
            // 시간이 같을 때는 선점하지 않고 현재 실행을 그대로 이어 간다.
            enterReady(*running);
            running.reset();
        }

        if (!running && !ready.empty()) {
            const auto selected = std::min_element(ready.begin(), ready.end(),
                [&states](const std::size_t left, const std::size_t right) {
                    return states[left].remaining < states[right].remaining;
                });
            running = *selected;
            auto& state = states[*running];
            state.waitingTime = AddTime(state.waitingTime, now - state.readySince);
            ready.erase(selected);
        }

        // 아무 변화가 없는 구간은 한 번에 건너뛴다. CPU가 비어 있거나
        // 첫 도착이 늦어도, 실제 초 단위로 반복하며 기다릴 필요가 없다.
        std::optional<SimulationTime> nextTime;
        const auto consider = [&](const SimulationTime time) {
            if (!nextTime || time < *nextTime) {
                nextTime = time;
            }
        };
        if (nextArrival < arrivals.size()) {
            consider(processes[arrivals[nextArrival]].arrival);
        }
        for (const auto& item : waiting) {
            consider(item.time);
        }
        if (running) {
            consider(AddTime(now, states[*running].remaining));
        }
        if (!nextTime) {
            throw std::logic_error("An unfinished process has no scheduled event.");
        }
        const SimulationTime elapsed = *nextTime - now;
        now = *nextTime;
        if (!running) {
            continue;
        }

        auto& state = states[*running];
        state.remaining -= elapsed;
        if (state.remaining == 0) {
            const auto& bursts = processes[*running].bursts;
            ++state.burstIndex;
            if (state.burstIndex == bursts.size()) {
                ++finished;
            }
            else {
                // CPU를 마친 시각부터 I/O를 시작하고, 다음 CPU 구간을 준비한다.
                const auto completion = AddTime(now, bursts[state.burstIndex]);
                waiting.push_back({*running, completion});
                ++state.burstIndex;
                state.remaining = bursts[state.burstIndex];
            }
            running.reset();
        }
    }

    std::vector<SimulationTime> result;
    result.reserve(states.size());
    for (const auto& state : states) {
        result.push_back(state.waitingTime);
    }
    return result;
}
