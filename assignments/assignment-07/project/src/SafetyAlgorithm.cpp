// 은행원 알고리즘의 Safety Algorithm을 구현한다.
// 프로세스가 필요한 자원을 받은 뒤 완료한다고 가정하며, 실제 프로세스를 실행하지는 않는다.
#include "SafetyAlgorithm.h"

#include <limits>
#include <stdexcept>
#include <vector>

// 남은 자원으로 완료할 수 있는 프로세스를 하나씩 찾는다.
// 모두 찾으면 그 순서가 안전 순서이며, 더 찾지 못하면 미완료 프로세스가 남게 된다.
SafetyResult CheckSafety(const ResourceState& state) {
    // Work는 검사 도중 사용할 수 있는 자원이다. 처음에는 Available과 같고,
    // 프로세스 완료를 가정할 때마다 그 프로세스가 반납한 자원이 더해진다.
    // 복사본으로 계산해야 나중에 입력 표를 출력할 때 원래 Available을 보여 줄 수 있다.
    ResourceVector work = state.available;
    // Finish[i]가 true이면 i번째 프로세스의 완료를 이미 가정했다는 뜻이다.
    // 한 프로세스의 자원을 두 번 반납받거나 결과에 중복해서 넣는 일을 막는다.
    std::vector<bool> finish(state.processNames.size(), false);
    SafetyResult result;

    while (result.safeSequence.size() < finish.size()) {
        // 유효한 행 번호는 0부터 size-1까지다. size를 초기값으로 쓰면
        // 아래 탐색에서 완료 가능한 프로세스를 찾았는지 구분할 수 있다.
        std::size_t selected = finish.size();
        for (std::size_t i = 0; i < finish.size(); ++i) {
            if (finish[i]) {
                continue;
            }

            // Need는 이 프로세스가 앞으로 더 요구할 수 있는 자원이다.
            // A 자원이 충분하더라도 B 자원이 부족하면 완료를 보장할 수 없으므로
            // 모든 열에서 Need = Max - Allocation <= Work를 만족해야 한다.
            bool canFinish = true;
            for (std::size_t j = 0; j < work.size(); ++j) {
                const ResourceCount need = state.maximum[i][j] - state.allocation[i][j];
                if (need > work[j]) {
                    canFinish = false;
                    break;
                }
            }
            if (canFinish) {
                selected = i;
                // 안전 순서는 여러 개일 수 있다. 여기서는 입력 순서가 앞선 후보를
                // 선택해 같은 파일을 실행할 때마다 동일한 순서를 얻도록 한다.
                break;
            }
        }

        if (selected == finish.size()) {
            // 미완료 프로세스를 모두 살펴봐도 후보가 없으면 Work도 더 늘지 않는다.
            // 같은 상태로 반복해도 결과가 바뀌지 않으므로 여기서 검사를 끝낸다.
            break;
        }
        for (std::size_t j = 0; j < work.size(); ++j) {
            const ResourceCount released = state.allocation[selected][j];
            // 많은 자원을 합쳤을 때 정수 범위를 넘으면 잘못된 값이 될 수 있다.
            // 덧셈을 하기 전에 남은 표현 범위를 확인하고 넘치는 경우 오류를 알린다.
            if (released > std::numeric_limits<ResourceCount>::max() - work[j]) {
                throw std::overflow_error("The resource total exceeds the 64-bit range.");
            }
            // 추가로 빌린 Need도 완료 후 반납되므로 빌리기 전과 비교하면 상쇄된다.
            // 따라서 Work에는 원래 가지고 있던 Allocation만 더하면 된다.
            work[j] += released;
        }
        // 반납 처리를 마친 프로세스를 완료로 표시하고 안전 순서 뒤에 기록한다.
        finish[selected] = true;
        result.safeSequence.push_back(selected);
        // 자원이 늘면 앞에서 건너뛴 프로세스도 완료할 수 있다. 다음 반복에서는
        // 마지막으로 선택한 위치 다음이 아니라 첫 프로세스부터 다시 살펴본다.
    }

    // 부분적으로 완료한 프로세스는 제외하고 Finish가 false인 행만 모은다.
    // 이 목록은 현재 교착 상태라는 뜻이 아니라 완료를 보장하지 못했다는 뜻이다.
    for (std::size_t i = 0; i < finish.size(); ++i) {
        if (!finish[i]) {
            result.unsafeProcesses.push_back(i);
        }
    }
    return result;
}
