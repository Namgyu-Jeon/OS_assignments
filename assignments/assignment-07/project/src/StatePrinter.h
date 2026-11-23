// 입력 상태와 계산 결과를 받아 과제 예제의 표와 결과 문구로 출력하는 함수를 선언한다.
#pragma once

#include "ResourceState.h"
#include "SafetyAlgorithm.h"

#include <iosfwd>

// Safe이면 완료 가능한 전체 순서를, UnSafe이면 완료를 보장하지 못한 목록을 출력한다.
// output으로 출력 대상을 받아 화면 출력과 별도의 검증에 같은 함수를 사용할 수 있다.
void PrintSafetyResult(const ResourceState& state,
    const SafetyResult& result, std::ostream& output);
