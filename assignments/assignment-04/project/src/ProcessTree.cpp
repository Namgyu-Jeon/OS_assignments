// 각 프로세스의 부모를 찾아 자식을 붙이고, 출력이 빠지거나 끝없이 반복될 관계를 정리한다.
#include "ProcessTree.h"

#include <algorithm>
#include <stdexcept>
#include <unordered_map>

namespace {

// 부모를 아직 확인하지 않았는지, 지금 따라가는 중인지, 확인을 끝냈는지 구분한다.
enum class VisitState {
    Unvisited,
    Visiting,
    Finished
};

// 부모를 따라가다가 같은 프로세스로 돌아오는 관계가 있으면 연결 하나를 끊어 준다.
// 예를 들어 A의 부모가 B이고 B의 부모가 A이면, 그대로는 둘 사이를 계속 돌게 된다.
// 그중 PID가 가장 작은 프로세스를 맨 위에 붙여 모두 출력할 수 있게 한다.
void RemoveParentCycles(
    const std::vector<ProcessTreeNode>& nodes,
    std::vector<std::size_t>& parentIndices) {
    std::vector<VisitState> states(nodes.size(), VisitState::Unvisited);
    // 첫 번째 자리는 트리 맨 위이므로 더 따라갈 부모가 없다.
    states[0] = VisitState::Finished;
    // 이번에 따라간 부모들을 기억해 두었다가, 확인이 끝나면 한꺼번에 표시한다.
    std::vector<std::size_t> path;
    path.reserve(nodes.size());

    for (std::size_t start = 1; start < nodes.size(); ++start) {
        // 다른 프로세스의 부모를 찾다가 이미 확인한 부분은 다시 검사하지 않는다.
        if (states[start] != VisitState::Unvisited) {
            continue;
        }

        path.clear();
        std::size_t current = start;
        while (states[current] == VisitState::Unvisited) {
            states[current] = VisitState::Visiting;
            path.push_back(current);
            current = parentIndices[current];
        }

        // 이번에 지나온 프로세스로 다시 돌아왔다면 부모 관계가 빙빙 돌고 있는 것이다.
        if (states[current] == VisitState::Visiting) {
            // 목록 순서가 달라져도 같은 연결을 끊도록, 가장 작은 PID를 고른다.
            std::size_t cycleRoot = current;
            for (std::size_t member = parentIndices[current];
                 member != current; member = parentIndices[member]) {
                if (nodes[member].process.pid < nodes[cycleRoot].process.pid) {
                    cycleRoot = member;
                }
            }
            // 화면에서 붙일 곳만 맨 위로 바꾸고, 원래 부모 번호는 그대로 남겨 둔다.
            parentIndices[cycleRoot] = 0;
        }

        // 방금 따라간 부분은 확인을 끝냈다고 표시해서, 다음에 같은 일을 반복하지 않는다.
        for (const std::size_t index : path) {
            states[index] = VisitState::Finished;
        }
    }
}

}

// 프로세스 목록을 받아 부모 아래에 자식이 연결된 트리를 만든다.
ProcessTree BuildProcessTree(const std::vector<ProcessInfo>& processes) {
    ProcessTree tree;
    // 프로세스 수는 Windows에서 읽은 목록의 개수로 정한다.
    // 트리를 그리기 위해 만든 맨 위 자리를 실제 프로세스처럼 하나 더 세지 않는다.
    tree.runningProcessCount = processes.size();
    tree.nodes.reserve(processes.size() + 1);
    // 부모를 찾을 수 없는 프로세스도 붙일 수 있도록 맨 위에 PID 0인 자리를 만든다.
    // 이것은 화면용 가상 루트이며, Windows의 실제 System 프로세스와는 따로 둔다.
    tree.nodes.push_back({{L"[System Process]", 0, 0}, 0, {}});

    // PID만 알면 프로세스를 바로 찾을 수 있도록, PID와 목록의 위치를 짝지어 둔다.
    // 부모가 목록에 나중에 나와도 연결할 수 있게, 모든 항목을 먼저 등록한다.
    std::unordered_map<ProcessId, std::size_t> indexByPid;
    indexByPid.reserve(processes.size() + 1);
    indexByPid.emplace(0, 0);
    bool hasSnapshotPidZero = false;

    for (const ProcessInfo& process : processes) {
        if (process.pid == 0) {
            if (hasSnapshotPidZero) {
                throw std::invalid_argument("Duplicate PID 0 in snapshot.");
            }
            hasSnapshotPidZero = true;
            // PID 0은 맨 위에 이미 자리를 만들었으므로 화면에 또 추가하지 않는다.
            // 읽어 온 목록의 개수를 셀 때는 이미 포함되어 있다.
            continue;
        }

        const std::size_t index = tree.nodes.size();
        if (!indexByPid.emplace(process.pid, index).second) {
            throw std::invalid_argument("Duplicate PID in snapshot.");
        }
        tree.nodes.push_back({process, 0, {}});
    }

    // 부모의 PID 대신, 부모가 nodes의 몇 번째 자리에 있는지 저장한다.
    // 처음에는 모두 0으로 두어, 부모를 못 찾으면 맨 위에 붙도록 한다.
    std::vector<std::size_t> parentIndices(tree.nodes.size(), 0);
    for (std::size_t index = 1; index < tree.nodes.size(); ++index) {
        const ProcessInfo& process = tree.nodes[index].process;
        const auto parent = indexByPid.find(process.originalParentPid);
        if (parent != indexByPid.end() && process.originalParentPid != process.pid) {
            parentIndices[index] = parent->second;
        }
        // 부모가 목록에 없거나 자기 번호를 부모로 갖고 있으면 처음 값인 0을 유지한다.
        // 이런 프로세스도 빠지지 않도록 맨 위 아래에 붙이는 것이다.
    }

    RemoveParentCycles(tree.nodes, parentIndices);
    // 끝난 프로세스의 번호가 다른 프로세스에게 다시 배정됐는지는,
    // 지금 가져온 목록의 번호만으로는 정확히 알 수 없다.

    for (std::size_t index = 1; index < tree.nodes.size(); ++index) {
        const std::size_t parent = parentIndices[index];
        tree.nodes[index].treeParentPid = tree.nodes[parent].process.pid;
        // 부모마다 자기 자식들을 미리 모아 둔다.
        // 출력할 때마다 전체 목록을 뒤져서 자식을 다시 찾지 않아도 된다.
        tree.nodes[parent].children.push_back(index);
    }

    // 같은 부모의 자식들은 PID가 작은 순서로 나오도록 정렬한다.
    for (ProcessTreeNode& node : tree.nodes) {
        std::sort(node.children.begin(), node.children.end(),
            [&tree](std::size_t left, std::size_t right) {
                return tree.nodes[left].process.pid < tree.nodes[right].process.pid;
            });
    }

    // N은 읽어 온 프로세스 수이다. 부모를 찾아 연결하는 작업은 평균 O(N)이다.
    // 자식들을 PID 순서로 정렬하는 데에는 전체적으로 최악 O(N log N)이 필요하다.
    return tree;
}
