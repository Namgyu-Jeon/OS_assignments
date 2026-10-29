// 스레드를 전부 만든 뒤 종료를 기다려, 각 칸의 1초 대기가 겹쳐서 진행되게 한다.
#include "ThreadMultiplier.h"

#include <Windows.h>

#include <exception>
#include <stdexcept>
#include <system_error>
#include <vector>

namespace {

// 한 스레드가 계산에 필요한 입력과 자신이 담당할 결과 위치를 받는 구조다.
// 입력 행렬 전체를 스레드마다 복사하지 않고, 계산이 끝날 때까지 유지되는 주소를 전달한다.
struct CellTask {
    const ResultMatrix* left;
    const InputMatrix* right;
    MatrixValue* destination; // 이 스레드만 기록할 결과 칸이다.
    std::size_t row;
    std::size_t column;
};

// Windows가 요구하는 함수 형태로 작업을 시작한다. parameter는 CellTask의 주소다.
// 성공하면 0을 반환한다. 파일 처리나 메모리 할당은 이 함수 밖에서 끝내 둔다.
DWORD WINAPI CalculateCell(void* parameter) noexcept {
    const auto& task = *static_cast<const CellTask*>(parameter);

    // 과제 조건: 각 스레드가 자기 칸의 계산을 시작하기 직전에 1초 쉰다.
    Sleep(1000);
    // 왼쪽의 한 행과 오른쪽의 한 열을 곱해 더하면 담당 칸의 값이 된다.
    // 합계는 자기 스레드 안에서만 사용하고, 마지막에 자기 칸에 한 번 기록한다.
    MatrixValue sum;
    for (std::size_t k = 0; k < task.left->Columns(); ++k) {
        sum.AddProduct(task.left->At(task.row, k), task.right->At(k, task.column));
    }
    *task.destination = sum;
    return 0;
}

// 생성한 스레드의 핸들(Windows 작업을 가리키는 값)을 소유하고 끝까지 정리한다.
// 객체가 범위를 벗어날 때 정리하게 하여, 예외가 발생한 경로에서도 작업이 남지 않게 한다.
class WorkerThreads {
public:
    // 생성할 개수를 미리 아니까 핸들 저장 공간을 한 번에 확보한다.
    explicit WorkerThreads(const std::size_t count) { handles_.reserve(count); }
    // 핸들을 복사하면 같은 대상을 두 번 닫을 수 있으므로 객체 복사를 막는다.
    WorkerThreads(const WorkerThreads&) = delete;
    WorkerThreads& operator=(const WorkerThreads&) = delete;

    ~WorkerThreads() noexcept {
        // 중간에 스레드 생성이 실패해도 이미 시작한 스레드는 끝까지 기다린다.
        // 이 객체를 작업 인수보다 나중에 선언하여, 인수가 먼저 사라지지 않게 한다.
        for (const HANDLE handle : handles_) {
            if (WaitForSingleObject(handle, INFINITE) != WAIT_OBJECT_0) {
                // 소유한 유효 핸들의 대기에 실패하면, 살아 있는 스레드가 참조하는
                // 메모리를 풀고 계속 실행할 수 없으므로 프로세스를 안전하게 끝낸다.
                std::terminate();
            }
            // 핸들을 닫는 것은 스레드를 강제로 끝내는 일이 아니다.
            // 먼저 작업 종료를 확인한 뒤 Windows에 보관 자원을 돌려준다.
            CloseHandle(handle);
        }
    }

    // 과제 조건에 맞게 이 칸을 위한 새 스레드를 만든다. 기존 스레드를 재사용하지 않는다.
    void Start(CellTask& task) {
        // 100x100이면 스레드 10,000개다. 작은 스택으로 주소 공간 낭비를 줄인다.
        // 작업 함수에는 재귀, 파일 출력, 동적 메모리 할당이 없다.
        // 이 크기는 과제 지정값이 아니라 이번 작업 함수에 맞춘 구현 선택이다.
        constexpr SIZE_T kStackReserve = 64 * 1024;
        const HANDLE handle = CreateThread(nullptr, kStackReserve, CalculateCell, &task,
            STACK_SIZE_PARAM_IS_A_RESERVATION, nullptr);
        if (handle == nullptr) {
            throw std::system_error(static_cast<int>(GetLastError()),
                std::system_category(), "CreateThread failed");
        }
        // 공간은 생성자에서 확보했으므로 핸들을 받은 뒤 재할당하지 않는다.
        handles_.push_back(handle);
    }

    void Wait() {
        // 모든 스레드는 이미 실행 중이다. 하나씩 기다려도 계산이 순차 실행되지는 않는다.
        // 한 번에 많은 핸들을 기다리는 함수는 최대 64개 제한이 있어서,
        // 이 구현에서는 개별 대기 함수를 사용한다. 확인하고 닫은 핸들은 목록에서
        // 빼서 소멸자가 다시 기다리지 않게 한다. 실패하면 남은 것만 소멸자가 정리한다.
        while (!handles_.empty()) {
            const HANDLE handle = handles_.back();
            if (WaitForSingleObject(handle, INFINITE) != WAIT_OBJECT_0) {
                throw std::system_error(static_cast<int>(GetLastError()),
                    std::system_category(), "Waiting for a worker thread failed");
            }
            if (!CloseHandle(handle)) {
                throw std::system_error(static_cast<int>(GetLastError()),
                    std::system_category(), "Closing a worker thread handle failed");
            }
            handles_.pop_back();
        }
    }

private:
    std::vector<HANDLE> handles_;
};

} // namespace

ResultMatrix ToResultMatrix(const InputMatrix& input) {
    ResultMatrix result(input.Rows(), input.Columns());
    for (std::size_t i = 0; i < input.Rows(); ++i) {
        for (std::size_t j = 0; j < input.Columns(); ++j) {
            result.At(i, j) = MatrixValue(input.At(i, j));
        }
    }
    return result;
}

ResultMatrix MultiplyWithThreads(const ResultMatrix& left, const InputMatrix& right) {
    if (left.Columns() != right.Rows()) {
        throw std::invalid_argument("Adjacent matrix dimensions do not match.");
    }

    // m*k와 k*n의 결과는 m*n이다. 각 칸마다 전용 스레드를 하나씩 만든다.
    ResultMatrix result(left.Rows(), right.Columns());
    const std::size_t count = result.Rows() * result.Columns();
    // 계산 도중 주소가 바뀌지 않도록, 모든 작업 인수의 자리를 먼저 확보한다.
    // result와 tasks보다 workers를 나중에 선언해야 예외가 나도 workers가 먼저
    // 작업 종료를 기다리고, 그 뒤에 스레드가 참조하던 메모리를 해제할 수 있다.
    std::vector<CellTask> tasks(count);
    WorkerThreads workers(count);
    for (std::size_t i = 0; i < result.Rows(); ++i) {
        for (std::size_t j = 0; j < result.Columns(); ++j) {
            auto& task = tasks[i * result.Columns() + j];
            task = CellTask{&left, &right, &result.At(i, j), i, j};
            workers.Start(task);
        }
    }
    // 입력은 읽기 전용이고 각 스레드가 쓰는 칸은 겹치지 않아 잠금이 필요 없다.
    // 대신 모든 작업이 끝나기 전에는 결과를 반환하거나 다음 곱셈을 시작하지 않는다.
    workers.Wait();
    return result;
}
