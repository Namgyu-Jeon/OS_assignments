// 실제 Windows 스레드를 사용하면서 생성 수, 1초 대기, 실패 시 정리를 확인한다.
// 검사용으로만 포함하는 파일이며 mm.exe 프로젝트에는 들어가지 않는다.
#include <Windows.h>

#include <chrono>
#include <iostream>
#include <set>
#include <stdexcept>
#include <system_error>

namespace {

LONG created = 0;
LONG closed = 0;
LONG failAfter = -1;
LONG waits = 0;
bool failNextWait = false;
bool failNextClose = false;
volatile LONG sleepCalls = 0;
volatile LONG finishedSleeps = 0;
volatile LONG invalidSleeps = 0;
DWORD threadIds[10000]{};

HANDLE WINAPI TracedCreateThread(LPSECURITY_ATTRIBUTES attributes, SIZE_T size,
    LPTHREAD_START_ROUTINE routine, LPVOID parameter, DWORD flags, LPDWORD id) {
    if (failAfter >= 0 && created == failAfter) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return nullptr;
    }
    const HANDLE handle = ::CreateThread(attributes, size, routine, parameter, flags, id);
    if (handle != nullptr) {
        ++created;
    }
    return handle;
}

VOID WINAPI TracedSleep(const DWORD milliseconds) {
    const LONG index = InterlockedIncrement(&sleepCalls) - 1;
    if (index < 10000) {
        threadIds[index] = GetCurrentThreadId();
    }
    if (milliseconds != 1000) {
        InterlockedIncrement(&invalidSleeps);
    }
    ::Sleep(milliseconds);
    InterlockedIncrement(&finishedSleeps);
}

BOOL WINAPI TracedCloseHandle(const HANDLE handle) {
    if (failNextClose) {
        failNextClose = false;
        SetLastError(ERROR_ACCESS_DENIED);
        return FALSE;
    }
    const BOOL result = ::CloseHandle(handle);
    if (result) {
        ++closed;
    }
    return result;
}

DWORD WINAPI TracedWaitForSingleObject(const HANDLE handle, const DWORD timeout) {
    ++waits;
    if (failNextWait) {
        failNextWait = false;
        SetLastError(ERROR_INVALID_HANDLE);
        return WAIT_FAILED;
    }
    return ::WaitForSingleObject(handle, timeout);
}

void Reset(const LONG failure = -1) {
    created = closed = 0;
    sleepCalls = finishedSleeps = invalidSleeps = 0;
    failAfter = failure;
    waits = 0;
    failNextWait = failNextClose = false;
}

void Check(const bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

} // namespace

// 작업 코드 자체를 포함해서 API 호출만 관찰한다. 제품 소스에 검사용 분기를 넣지 않는다.
#define CreateThread TracedCreateThread
#define Sleep TracedSleep
#define CloseHandle TracedCloseHandle
#define WaitForSingleObject TracedWaitForSingleObject
#include "../src/ThreadMultiplier.cpp"
#undef CreateThread
#undef Sleep
#undef CloseHandle
#undef WaitForSingleObject

int main() {
    try {
        InputMatrix a(2, 2);
        InputMatrix b(2, 3);
        for (std::size_t i = 0; i < 2; ++i) {
            for (std::size_t j = 0; j < 2; ++j) {
                a.At(i, j) = 1;
            }
            for (std::size_t j = 0; j < 3; ++j) {
                b.At(i, j) = 1;
            }
        }
        const auto left = ToResultMatrix(a);
        Reset();
        auto start = std::chrono::steady_clock::now();
        const auto result = MultiplyWithThreads(left, b);
        const double seconds = std::chrono::duration<double>(
            std::chrono::steady_clock::now() - start).count();
        Check(created == 6 && closed == 6, "Expected six created and closed threads.");
        Check(waits == 6, "A finished worker must not be waited on twice.");
        Check(sleepCalls == 6 && finishedSleeps == 6 && invalidSleeps == 0,
            "Each of the six workers must call Sleep(1000) exactly once.");
        const std::set<DWORD> ids(threadIds, threadIds + 6);
        Check(ids.size() == 6, "Each cell must use a different worker thread.");
        Check(seconds >= 0.99, "The actual one-second wait was missing.");
        for (std::size_t i = 0; i < 2; ++i) {
            for (std::size_t j = 0; j < 3; ++j) {
                Check(result.At(i, j).ToString() == "2", "Incorrect cell calculation.");
            }
        }
        std::cout << "PASS six cells: six distinct threads, six Sleep(1000), correct values\n";

        Reset(3);
        bool rejected = false;
        try {
            const auto unused = MultiplyWithThreads(left, b);
            (void)unused;
        }
        catch (const std::system_error& error) {
            rejected = error.code().value() == ERROR_NOT_ENOUGH_MEMORY;
        }
        Check(rejected, "Injected CreateThread failure was not reported.");
        Check(created == 3 && closed == 3 && finishedSleeps == 3,
            "Started threads must finish and close before failure is propagated.");
        std::cout << "PASS partial creation failure: all three workers joined and closed\n";

        Reset(0);
        rejected = false;
        try {
            const auto unused = MultiplyWithThreads(left, b);
            (void)unused;
        }
        catch (const std::system_error&) {
            rejected = true;
        }
        Check(rejected && created == 0 && closed == 0 && sleepCalls == 0,
            "Failure before the first worker was not handled.");
        std::cout << "PASS first creation failure: no worker or handle left behind\n";

        for (const bool closeFailure : {false, true}) {
            Reset();
            failNextWait = !closeFailure;
            failNextClose = closeFailure;
            rejected = false;
            try {
                const auto unused = MultiplyWithThreads(left, b);
                (void)unused;
            }
            catch (const std::system_error&) {
                rejected = true;
            }
            Check(rejected && created == 6 && closed == 6 && finishedSleeps == 6,
                "Wait or close failure must clean up all remaining workers.");
            std::cout << "PASS " << (closeFailure ? "close" : "wait")
                      << " failure: remaining workers joined and closed\n";
        }

        InputMatrix large(100, 100);
        for (std::size_t i = 0; i < 100; ++i) {
            for (std::size_t j = 0; j < 100; ++j) {
                large.At(i, j) = 3;
            }
        }
        Reset();
        const auto maximum = MultiplyWithThreads(ToResultMatrix(large), large);
        Check(created == 10000 && closed == 10000 && sleepCalls == 10000 &&
            finishedSleeps == 10000 && invalidSleeps == 0,
            "A 100x100 result must create and finish exactly 10000 workers.");
        Check(waits == 10000, "Maximum-size workers were waited on more than once.");
        for (std::size_t i = 0; i < 100; ++i) {
            for (std::size_t j = 0; j < 100; ++j) {
                Check(maximum.At(i, j).ToString() == "900", "Incorrect maximum-size result.");
            }
        }
        std::cout << "PASS 100x100: 10000 threads, 10000 Sleep(1000), all handles closed\n";
        return 0;
    }
    catch (const std::exception& error) {
        std::cerr << "FAIL " << error.what() << '\n';
        return 1;
    }
}
