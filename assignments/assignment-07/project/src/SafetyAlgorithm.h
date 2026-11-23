// 안전성 검사의 결과와 검사 함수를 선언한다. 파일 읽기나 화면 출력은 담당하지 않는다.
#pragma once

#include "ResourceState.h"

#include <cstddef>
#include <vector>

struct SafetyResult {
    // 실제 이름 대신 ResourceState의 행 번호를 저장한다.
    // UnSafe 상태에서는 safeSequence에 일부 완료 가능한 프로세스만 들어 있을 수 있다.
    std::vector<std::size_t> safeSequence;    // 완료를 가정한 순서대로 기록한 프로세스
    std::vector<std::size_t> unsafeProcesses; // 검사 종료 시 완료를 보장할 수 없었던 프로세스
};

// 원본 상태를 바꾸지 않고 완료 가능한 프로세스를 반복해서 찾는다.
// unsafeProcesses가 비어 있으면 모든 프로세스의 완료가 가능하므로 Safe 상태다.
SafetyResult CheckSafety(const ResourceState& state);
