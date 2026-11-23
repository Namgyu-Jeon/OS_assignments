// 입력 파일에서 읽은 정보를 하나의 상태로 묶어 계산 함수와 출력 함수에 전달한다.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

// 자원 개수는 정수로 다룬다. 여러 프로세스의 자원을 합칠 때를 고려해 64비트를 쓴다.
using ResourceCount = std::int64_t;
using ResourceVector = std::vector<ResourceCount>; // 자원 종류별 개수를 담는 한 행
using ResourceTable = std::vector<ResourceVector>; // 프로세스별 행을 모아 놓은 표

// 표의 [i][j]는 i번째 프로세스와 j번째 자원에 해당한다.
// 예를 들어 allocation[0][1]은 첫 프로세스가 이미 가진 두 번째 자원의 개수다.
// 이름과 값은 모두 입력 순서로 저장하므로 계산 결과를 원래 이름으로 출력할 수 있다.
struct ResourceState {
    std::vector<std::string> resourceNames; // 각 표의 열 이름: A, B, C 등
    std::vector<std::string> processNames;  // 각 표의 행 이름: P_00, P_01 등
    ResourceTable allocation;             // 현재 각 프로세스에 할당되어 있는 자원
    ResourceTable maximum;                // 각 프로세스가 필요로 할 수 있는 최대 자원
    ResourceVector available;             // 아직 할당되지 않아 지금 사용할 수 있는 자원
};
