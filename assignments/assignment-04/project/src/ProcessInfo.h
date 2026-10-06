// 프로세스 한 개의 이름, 번호(PID), 원래 부모의 번호(PPID)를 담는다.
#pragma once

#include <cstdint>
#include <string>

using ProcessId = std::uint32_t;

struct ProcessInfo {
    std::wstring name;
    ProcessId pid = 0;
    // 부모가 없어 화면에서 다른 곳에 붙이더라도, Windows가 알려 준 부모 번호는 남겨 둔다.
    // 그래야 원래 부모가 누구였는지도 출력할 수 있다.
    ProcessId originalParentPid = 0;
};
