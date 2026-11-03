// PDF 예제와 같은 입력 목록, 대기 시간 표, 평균을 화면에 출력한다.
#pragma once

#include "ProcessInfo.h"

#include <iosfwd>
#include <vector>

// 출력 맨 앞과 맨 뒤에는 빈 줄을 하나씩 둔다. 평균은 소수점 둘째 자리까지 쓴다.
void PrintSchedule(const std::vector<ProcessInfo>& processes,
    const std::vector<SimulationTime>& waitingTimes, std::ostream& output);
