// 부모를 먼저 보여 주고 그 아래에 자식들을 출력한다.
// +--와 | 선을 이어서, 각 프로세스가 어느 부모 아래에 있는지 알아볼 수 있게 한다.
#include "ProcessTreePrinter.h"

#include <iomanip>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

// 지금 출력 중인 부모와 다음에 출력할 자식의 위치를 기억한다.
// 자식들을 다 보여 준 뒤, 이전 부모로 돌아가기 위해 필요한 정보이다.
struct TraversalFrame {
    std::size_t nodeIndex;
    std::size_t nextChild = 0;
};

// 한 줄에 순번, 연결선, 이름(PID), (원래 부모 번호:화면에서 붙인 부모 번호)를 쓴다.
// 이름은 Windows에서 받은 그대로 출력하며, 긴 이름을 자르거나 .exe를 덧붙이지 않는다.
void PrintNode(
    const ProcessTreeNode& node, std::size_t sequence, int sequenceWidth,
    const std::wstring& prefix, std::wostream& output) {
    output << std::setw(sequenceWidth) << sequence << L' '
           << prefix << L"+--" << node.process.name
           << L" (" << node.process.pid << L") ("
           << node.process.originalParentPid << L':' << node.treeParentPid
           << L")\n";
}

}

// 맨 위부터 시작해 부모 아래의 자식을 차례로 보여 준다. 내려갔던 위치는 따로 기억한다.
void PrintProcessTree(const ProcessTree& tree, std::wostream& output) {
    if (tree.nodes.empty()) {
        throw std::invalid_argument("Process tree has no root.");
    }

    output << L"############### Process Tree ###############\n"
           << L"Number of Running Processes = " << tree.runningProcessCount << L'\n';

    // 순번의 자릿수가 달라도 옆에 있는 트리의 시작 위치가 맞도록 너비를 정한다.
    const int sequenceWidth = static_cast<int>(std::to_wstring(tree.nodes.size()).size());
    std::size_t sequence = 1;
    PrintNode(tree.nodes[0], sequence++, sequenceWidth, L"", output);

    // stack에는 자식을 보여 준 뒤 돌아올 부모들을 차례로 기억해 둔다.
    // 같은 함수를 계속 다시 부르는 대신 반복문으로 처리해, 깊은 트리에서도 호출이 쌓이지 않게 했다.
    std::vector<TraversalFrame> stack;
    stack.push_back({0, 0});
    // 자식은 부모보다 한 단계 안쪽에 보이도록 네 칸씩 들여쓴다.
    std::wstring prefix = L"    ";

    while (!stack.empty()) {
        TraversalFrame& frame = stack.back();
        const auto& children = tree.nodes[frame.nodeIndex].children;
        // 이 부모의 자식을 모두 출력했으면 이전 부모로 돌아간다.
        if (frame.nextChild == children.size()) {
            stack.pop_back();
            if (!stack.empty()) {
                // 한 단계 올라왔으므로 들여쓰기도 네 칸 줄인다.
                prefix.resize(prefix.size() - 4);
            }
            continue;
        }

        // 아직 보여 주지 않은 자식 하나를 고르고, 다음에 볼 자식의 위치도 기억한다.
        const std::size_t childIndex = children[frame.nextChild++];
        const bool isLastChild = frame.nextChild == children.size();
        PrintNode(tree.nodes[childIndex], sequence++, sequenceWidth, prefix, output);

        // 이 자식에게도 자식이 있으면, 그 아래로 내려가 먼저 보여 준다.
        if (!tree.nodes[childIndex].children.empty()) {
            // 뒤에 같은 부모의 다른 자식이 남아 있으면 | 선을 계속 이어 준다.
            // 마지막 자식이면 더 이어질 선이 없으므로 공백을 넣는다.
            prefix += isLastChild ? L"    " : L"|   ";
            stack.push_back({childIndex, 0});
        }
    }
    // 각 프로세스는 한 번씩 보여 주지만, 깊게 내려갈수록 앞에 붙는 공백과 선이 길어진다.
    // 따라서 실제 출력에 걸리는 시간은 출력하는 전체 문자 수에 따라 달라진다.
}
