// 이 파일은 Windows API로 현재 PID 목록과 로컬 시각을 가져옵니다.
// PID 버퍼가 모자라면 안전한 범위에서 늘리고, 확보한 공간은 다음 조회에도 재사용합니다.

#include "WindowsSystemInfo.h"

#include <Windows.h>
#include <Psapi.h>

#include <limits>
#include <vector>

namespace WindowsSystemInfo
{
namespace
{
constexpr std::size_t kInitialProcessCapacity = 256;
constexpr std::size_t kMaximumProcessCapacity = 1024 * 1024;
} // namespace

ProcessCountResult QueryProcessCount()
{
    // 한 번 늘린 버퍼를 다음 측정에서도 사용해 매초 같은 메모리를 다시 할당하지 않습니다.
    // 이 프로그램은 QueryProcessCount를 주 실행 스레드 한 곳에서만 호출합니다.
    static std::vector<DWORD> processIds(kInitialProcessCapacity);

    while (true)
    {
        const std::size_t capacity = processIds.size();

        if (capacity > kMaximumProcessCapacity ||
            capacity > std::numeric_limits<DWORD>::max() / sizeof(DWORD))
        {
            // 예상 밖의 큰 목록이 들어와도 크기 계산이 넘치거나 메모리를 과도하게 쓰지 않게 막습니다.
            return {false, 0, ERROR_NOT_ENOUGH_MEMORY};
        }

        const auto bufferBytes = static_cast<DWORD>(capacity * sizeof(DWORD));
        DWORD bytesReturned = 0;

        // 저장 공간은 재사용하지만 PID 목록 자체는 매번 EnumProcesses()로 새로 받습니다.
        if (EnumProcesses(processIds.data(), bufferBytes, &bytesReturned) == FALSE)
        {
            // 다른 API 호출로 값이 바뀌기 전에 곧바로 Windows 오류 코드를 저장합니다.
            return {false, 0, GetLastError()};
        }

        if (bytesReturned < bufferBytes)
        {
            // PID 하나가 DWORD 하나이므로 받은 바이트 수를 DWORD 크기로 나누면 개수가 됩니다.
            return {true, bytesReturned / sizeof(DWORD), ERROR_SUCCESS};
        }

        // 버퍼가 정확히 가득 찼다면 뒤가 잘렸을 수 있어 두 배로 늘린 뒤 다시 요청합니다.
        if (capacity > kMaximumProcessCapacity / 2)
        {
            return {false, 0, ERROR_NOT_ENOUGH_MEMORY};
        }

        processIds.resize(capacity * 2);
    }
}

LocalDateTime GetCurrentLocalDateTime() noexcept
{
    SYSTEMTIME systemTime{};

    // 저장해 둔 시각을 재사용하지 않고, 출력할 때마다 Windows의 로컬 시각을 새로 읽습니다.
    GetLocalTime(&systemTime);

    return {
        systemTime.wYear,
        systemTime.wMonth,
        systemTime.wDay,
        systemTime.wHour,
        systemTime.wMinute,
        systemTime.wSecond};
}
} // namespace WindowsSystemInfo
