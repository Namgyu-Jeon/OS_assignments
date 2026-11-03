// 단일 CPU의 선점형 SJF를 실행하고 프로세스별 전체 대기 시간을 구한다.
#pragma once

#include "ProcessInfo.h"

#include <vector>

// ReadProcesses로 읽은 정보를 받아 입력 순서대로 결과를 돌려준다.
// 대기 시간에는 Ready에서 기다린 시간만 포함하고, 실행 및 I/O 시간은 제외한다.
std::vector<SimulationTime> SimulateSjf(const std::vector<ProcessInfo>& processes);
