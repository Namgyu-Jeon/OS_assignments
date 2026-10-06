// 프로세스들을 부모와 자식으로 묶어서 출력할 때 필요한 정보를 담는다.
#pragma once

#include "ProcessInfo.h"

#include <cstddef>
#include <vector>

struct ProcessTreeNode {
    ProcessInfo process;
    // 화면에서 이 프로세스를 어느 부모 아래에 붙일지 나타내는 번호이다.
    // 이 값을 바꿔도 Windows의 실제 부모 관계가 바뀌는 것은 아니다.
    ProcessId treeParentPid = 0;
    // 이 프로세스의 자식들이 nodes의 몇 번째 자리에 있는지 저장한다.
    std::vector<std::size_t> children;
};

struct ProcessTree {
    // 트리 맨 위의 가상 루트는 찾기 쉽도록 항상 첫 번째 자리(nodes[0])에 둔다.
    std::vector<ProcessTreeNode> nodes;
    // Windows에서 읽어 온 실제 항목 수이다. 화면용 루트를 만들었다고 더하지 않는다.
    std::size_t runningProcessCount = 0;
};

// 읽어 온 목록에서 부모를 찾아, 각 프로세스를 부모 아래에 붙인다.
// 같은 PID가 두 번 나오면 어느 항목인지 구분할 수 없으므로 오류를 알린다.
ProcessTree BuildProcessTree(const std::vector<ProcessInfo>& processes);
