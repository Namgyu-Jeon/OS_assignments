// 이 파일은 프로그램의 시작점입니다.
// 실제 작업은 ProcessMonitor에 맡기고, 밖으로 전달된 예외를 마지막으로 처리합니다.

#include "ProcessMonitor.h"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <new>

// 별도의 명령행 입력은 받지 않습니다.
// 정상적으로 끝나면 0을, 실행 중 문제가 생기면 0이 아닌 값을 반환합니다.
// Run 안에서 처리하지 못한 예외도 여기서 메시지를 남긴 뒤 안전하게 종료합니다.
int main()
{
    try
    {
        ProcessMonitor monitor;
        return monitor.Run();
    }
    catch (const std::bad_alloc&)
    {
        // PID 목록을 담을 공간조차 확보하지 못한 경우에는 더 진행할 수 없습니다.
        std::cerr << "Error: Not enough memory to query the process list.\n";
    }
    catch (const std::exception& exception)
    {
        // 표준 예외가 알려 주는 원인을 그대로 보여 주어 문제를 찾기 쉽게 합니다.
        std::cerr << "Error: " << exception.what() << '\n';
    }
    catch (...)
    {
        // 종류를 알 수 없는 예외도 아무 설명 없이 끝나지 않도록 처리합니다.
        std::cerr << "Error: An unexpected failure occurred.\n";
    }

    std::cout << "Process counter terminated.\n" << std::flush;
    return EXIT_FAILURE;
}
