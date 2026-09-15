// 이 파일은 1초 간격의 측정과 콘솔 출력을 반복합니다.
// 기다리는 동안에도 키를 자주 확인해 ESC를 누르면 곧바로 끝낼 수 있게 합니다.

#include "ProcessMonitor.h"

#include "WindowsSystemInfo.h"

#include <algorithm>
#include <chrono>
#include <conio.h>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <thread>

namespace
{
constexpr int kEscapeKey = 27;
constexpr auto kSampleInterval = std::chrono::seconds(1);
constexpr auto kKeyboardPollInterval = std::chrono::milliseconds(20);

// deadline은 다음 측정을 시작할 steady_clock 기준 시각입니다.
// 그때까지 짧게 나누어 쉬면서 키를 확인하고, ESC를 발견하면 true를 반환합니다.
// 다른 키는 읽어서 비우되 종료 신호로 사용하지 않으며, 시간이 되면 false를 반환합니다.
[[nodiscard]] bool WaitUntilNextSampleOrEscape(
    const std::chrono::steady_clock::time_point deadline)
{
    while (true)
    {
        // _kbhit()으로 입력 유무만 확인하므로 키를 기다리느라 출력이 멈추지 않습니다.
        while (_kbhit() != 0)
        {
            // 키가 있을 때만 _getch()를 호출해 Enter 없이 ESC를 바로 알아냅니다.
            if (_getch() == kEscapeKey)
            {
                return true;
            }
        }

        const auto now = std::chrono::steady_clock::now();
        if (now >= deadline)
        {
            return false;
        }

        const auto wakeTime = std::min(deadline, now + kKeyboardPollInterval);

        // 다음 확인 시각까지 실제로 잠들어 바쁜 반복을 피하고 CPU 사용을 낮춥니다.
        std::this_thread::sleep_until(wakeTime);
    }
}

// time과 processCount를 과제에서 지정한 한 줄 형식으로 출력합니다.
// 반환값은 없으며, 쓰기에 실패했는지는 이 함수를 부른 Run에서 확인합니다.
void PrintSample(
    const WindowsSystemInfo::LocalDateTime& time,
    const std::size_t processCount)
{
    // 월, 일, 시, 분, 초 앞의 빈자리를 0으로 채워 항상 두 자리로 보이게 합니다.
    std::cout << std::setfill('0')
              << std::setw(4) << time.year << '.'
              << std::setw(2) << time.month << '.'
              << std::setw(2) << time.day << ' '
              << std::setw(2) << time.hour << ':'
              << std::setw(2) << time.minute << ':'
              << std::setw(2) << time.second
              << " : " << std::setfill(' ') << processCount << '\n'
              << std::flush;
}
} // namespace

int ProcessMonitor::Run() const
{
    // 안내가 다른 출력보다 늦게 나타나지 않도록 시작할 때 한 번 출력하고 비웁니다.
    std::cout << "Process counter is running. Press ESC to exit.\n" << std::flush;
    if (!std::cout)
    {
        std::cerr << "Error: Failed to write the startup message.\n";
        return EXIT_FAILURE;
    }

    auto nextSampleTime = std::chrono::steady_clock::now();
    int exitCode = EXIT_SUCCESS;

    while (true)
    {
        const auto processResult = WindowsSystemInfo::QueryProcessCount();
        if (!processResult.success)
        {
            // 같은 오류를 매초 쏟아내지 않고 Windows 오류 코드와 함께 한 번만 알립니다.
            std::cerr << "Error: Unable to query the process list. Windows error code: "
                      << processResult.windowsErrorCode << '\n';
            exitCode = EXIT_FAILURE;
            break;
        }

        const auto localTime = WindowsSystemInfo::GetCurrentLocalDateTime();
        PrintSample(localTime, processResult.count);
        if (!std::cout)
        {
            std::cerr << "Error: Failed to write a process count sample.\n";
            exitCode = EXIT_FAILURE;
            break;
        }

        // 직전 목표 시각에 1초를 더하므로 조회와 출력에 걸린 시간이 주기에 누적되지 않습니다.
        nextSampleTime += kSampleInterval;
        const auto afterSample = std::chrono::steady_clock::now();
        if (nextSampleTime <= afterSample)
        {
            // 한 주기 이상 늦어졌다면 밀린 결과를 몰아서 찍지 않고 지금부터 다시 셉니다.
            nextSampleTime = afterSample + kSampleInterval;
        }

        if (WaitUntilNextSampleOrEscape(nextSampleTime))
        {
            break;
        }
    }

    // ESC로 끝났든 오류가 났든 종료 안내는 한 번만 보여 줍니다.
    std::cout << "Process counter terminated.\n" << std::flush;
    if (!std::cout && exitCode == EXIT_SUCCESS)
    {
        std::cerr << "Error: Failed to write the termination message.\n";
        return EXIT_FAILURE;
    }

    return exitCode;
}
