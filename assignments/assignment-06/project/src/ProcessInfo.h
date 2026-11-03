// 입력 파일의 프로세스 정보와 시뮬레이션에서 사용하는 시간 단위를 정의한다.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

using SimulationTime = std::int64_t;

// bursts의 짝수 위치는 CPU, 홀수 위치는 I/O 시간이다. 처음과 끝은 CPU다.
// 입력 정보는 계산 중 바꾸지 않고, 남은 실행 시간은 스케줄러가 따로 관리한다.
struct ProcessInfo {
    std::string name;
    SimulationTime arrival = 0;
    std::vector<SimulationTime> bursts;
};
