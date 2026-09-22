// 이 파일은 WindowsCpuReader에 선언된 기능을 실제 Windows API로 구현한다.
// Windows가 제공하는 FILETIME을 계산하기 쉬운 64비트 숫자로 바꾸고,
// 두 시점의 차이에서 CPU가 일한 비율을 구한다. Windows 전용 코드는 이 파일에
// 집중시켜 프로그램의 나머지 부분이 운영체제 세부 형식에 덜 의존하게 했다.

#include "WindowsCpuReader.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <system_error>

namespace {

// FILETIME 하나는 큰 시간 값을 32비트 두 조각에 나누어 보관한다.
// dwLowDateTime에는 아래쪽 32비트, dwHighDateTime에는 위쪽 32비트가 들어 있다.
// 둘 중 하나만 사용하거나 단순히 더하면 원래 숫자가 되지 않는다.
// ULARGE_INTEGER는 이 두 조각을 올바른 자리에 배치해 QuadPart라는 하나의
// 64비트 값으로 볼 수 있게 해 주는 Windows 자료형이다.
// 입력은 API가 채운 FILETIME이고 반환값은 같은 시간을 나타내는 64비트 정수다.
// 단순한 비트 결합만 하므로 실패하거나 예외가 생길 조건이 없다.
[[nodiscard]] std::uint64_t fileTimeToUint64(const FILETIME& fileTime) noexcept {
    ULARGE_INTEGER value{};
    value.LowPart = fileTime.dwLowDateTime;
    value.HighPart = fileTime.dwHighDateTime;
    return value.QuadPart;
}

}  // namespace

unsigned int WindowsCpuReader::logicalProcessorCount() {
    // {}로 초기화해 구조체의 모든 필드를 먼저 0으로 만든다.
    // 그러면 Windows가 사용하지 않는 필드에 우연한 쓰레기 값이 남지 않는다.
    SYSTEM_INFO systemInfo{};

    // 컴퓨터마다 CPU 구성이 다르므로 4, 8 같은 숫자를 소스에 직접 쓰지 않는다.
    // GetSystemInfo()가 현재 실행 중인 Windows 환경의 정보를 구조체에 채운다.
    ::GetSystemInfo(&systemInfo);

    // GetSystemInfo() 자체는 성공 여부를 반환하지 않는다. 하지만 논리 프로세서가
    // 0개인 정상 시스템은 있을 수 없으므로 0은 사용할 수 없는 정보로 판단한다.
    if (systemInfo.dwNumberOfProcessors == 0) {
        throw std::runtime_error("GetSystemInfo returned zero logical processors.");
    }
    return systemInfo.dwNumberOfProcessors;
}

CpuTimes WindowsCpuReader::readCpuTimes() {
    // API가 세 결과를 채워 줄 공간을 준비한다. 모두 0으로 초기화해 둔다.
    FILETIME idleTime{};
    FILETIME kernelTime{};
    FILETIME userTime{};

    // GetSystemTimes()는 세 누적시간을 한 번에 읽는다.
    // 중요한 점은 이 값들이 현재 사용률이 아니라 컴퓨터가 켜진 뒤 쌓인 시간이라는 것이다.
    // 예를 들어 첫 호출이 1,000이고 다음 호출이 1,100이라면 최근 구간에 늘어난 시간은
    // 100이다. 그래서 프로그램은 반드시 이전 값과 현재 값의 차이를 사용한다.
    if (!::GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        // Windows 오류 번호는 다음 API 호출로 바뀔 수 있으므로 실패 직후 바로 읽는다.
        // std::system_error에 번호와 설명을 함께 넣어 상위 코드가 사용자에게 보여 주게 한다.
        const DWORD errorCode = ::GetLastError();
        throw std::system_error(
            static_cast<int>(errorCode),
            std::system_category(),
            "GetSystemTimes failed");
    }

    // Windows 전용 FILETIME 세 개를 프로그램에서 다루기 쉬운 CpuTimes로 바꿔 반환한다.
    return {
        fileTimeToUint64(idleTime),
        fileTimeToUint64(kernelTime),
        fileTimeToUint64(userTime)};
}

LocalDateTime WindowsCpuReader::readLocalDateTime() noexcept {
    SYSTEMTIME localTime{};

    // 측정 주기를 관리하는 steady_clock은 간격만 재는 시계라 날짜가 없다.
    // 사용자에게 보여 줄 실제 달력 시각은 과제에서 지정한 GetLocalTime()으로 얻는다.
    // 이 함수를 매 출력 시점마다 호출하므로 현재 시각이 한 번 읽은 값에 고정되지 않는다.
    ::GetLocalTime(&localTime);

    // SYSTEMTIME에는 요일과 밀리초도 있지만 과제 출력에는 필요하지 않다.
    // 필요한 연·월·일·시·분·초만 자체 구조체에 복사한다.
    return {
        localTime.wYear,
        localTime.wMonth,
        localTime.wDay,
        localTime.wHour,
        localTime.wMinute,
        localTime.wSecond};
}

double WindowsCpuReader::calculateCpuLoad(
    const CpuTimes& previous,
    const CpuTimes& current) {
    // 정상적인 누적시간은 시간이 지날수록 같거나 커져야 한다.
    // 현재 값이 이전 값보다 작으면 운영체제 값이 비정상이거나 기준이 달라진 것이다.
    // 부호 없는 정수에서 작은 수 - 큰 수를 계산하면 음수가 아니라 매우 큰 양수로
    // 돌아가므로, 뺄셈 전에 먼저 검사해야 터무니없는 CPU 부하를 막을 수 있다.
    if (current.idle < previous.idle ||
        current.kernel < previous.kernel ||
        current.user < previous.user) {
        throw std::domain_error("Cumulative CPU time moved backwards.");
    }

    // 각 delta는 프로그램 시작 이후의 전체 시간이 아니라 바로 두 측정 사이에
    // 새로 늘어난 시간이다. 이 세 차이가 최근 약 1초 구간을 설명한다.
    const std::uint64_t idleDelta = current.idle - previous.idle;
    const std::uint64_t kernelDelta = current.kernel - previous.kernel;
    const std::uint64_t userDelta = current.user - previous.user;

    // Windows 정의에서 kernel 시간에는 운영체제가 실제로 일한 시간뿐 아니라
    // CPU가 쉬고 있던 idle 시간도 들어 있다. 따라서 전체 시간은
    // kernelDelta + userDelta이고, 실제로 일한 시간은 거기서 idleDelta를 뺀 값이다.
    // idle을 빼지 않으면 쉬었던 시간까지 CPU가 일한 것으로 계산하는 오류가 생긴다.

    // 두 64비트 수를 더하기 전에 최대 범위를 넘는지 확인한다.
    // 현실적인 1초 측정에서는 거의 일어나지 않지만, 잘못된 값이 조용히 0 근처로
    // 되돌아가는 정수 오버플로를 방지하기 위한 안전 검사다.
    if (kernelDelta > (std::numeric_limits<std::uint64_t>::max)() - userDelta) {
        throw std::domain_error("CPU time delta overflowed.");
    }
    const std::uint64_t totalTime = kernelDelta + userDelta;

    // 분모가 0이면 나눗셈을 할 수 없다. 아주 짧은 구간이나 비정상 값에 대비한다.
    if (totalTime == 0) {
        throw std::domain_error("Total CPU time delta is zero.");
    }
    // 쉰 시간이 전체 시간보다 클 수는 없다. 이 관계가 깨진 값으로 계산하면
    // busyTime 뺄셈도 잘못되므로 표본 전체를 버린다.
    if (idleDelta > totalTime) {
        throw std::domain_error("Idle CPU time exceeds total CPU time.");
    }

    // CPU가 실제로 일한 시간 / 전체 시간에 100을 곱해 백분율을 만든다.
    // 정수끼리 먼저 나누면 소수 부분이 사라지므로 double로 바꾼 뒤 나눈다.
    const std::uint64_t busyTime = totalTime - idleDelta;
    const double loadPercent =
        (static_cast<double>(busyTime) / static_cast<double>(totalTime)) * 100.0;

    // 위 관계 검사를 통과한 결과는 원칙적으로 0~100% 안에 있어야 한다.
    // 다만 정수를 double로 바꾸고 나누는 과정에서 100.0000000001처럼 경계를
    // 극히 조금 벗어나는 부동소수점 오차가 생길 수 있어 아주 작은 허용 범위를 둔다.
    // 허용 범위를 크게 벗어나면 단순 반올림 문제가 아니므로 잘못된 계산으로 처리한다.
    constexpr double kFloatingPointTolerance = 1e-9;
    if (loadPercent < -kFloatingPointTolerance ||
        loadPercent > 100.0 + kFloatingPointTolerance) {
        throw std::domain_error("Calculated CPU load is outside 0 to 100 percent.");
    }
    // clamp는 허용된 작은 오차만 정확한 0 또는 100으로 맞춰 최종 값을 보장한다.
    return std::clamp(loadPercent, 0.0, 100.0);
}
