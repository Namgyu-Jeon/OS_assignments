// 이 파일은 프로그램이 가장 먼저 시작되는 곳이다.
// 실제 CPU 측정 작업은 CpuMonitor가 담당하고, main은 그 작업을 시작한 뒤
// 프로그램 전체에서 처리하지 못한 오류를 마지막으로 받아 사용자에게 알려 준다.
// 이렇게 시작 부분을 짧게 유지하면 "프로그램을 실행하는 일"과
// "CPU 부하를 측정하는 일"이 섞이지 않아 전체 구조를 이해하기 쉽다.

#include "CpuMonitor.h"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <system_error>

// main()은 운영체제가 프로그램을 시작할 때 자동으로 호출하는 함수다.
// 입력값은 없으며, 정상적으로 끝나면 EXIT_SUCCESS를 반환하고 오류가 발생하면
// EXIT_FAILURE를 반환한다. 이 프로그램은 사용자가 Ctrl+C를 누를 때까지 계속
// 실행되므로, 아래의 정상 반환문은 예외 없이 반복문이 끝나는 특수한 경우를 위한 것이다.
int main() {
    try {
        // 보통 cout은 화면 출력에 실패해도 오류 상태만 표시하고 프로그램을 계속 실행한다.
        // 여기서는 failbit 또는 badbit가 생기면 예외를 발생시키도록 설정한다.
        // 예를 들어 출력 대상이 갑자기 닫힌 경우, 측정은 계속하면서 결과만 사라지는
        // 상황보다 프로그램을 오류로 종료하여 문제를 분명히 알리는 편이 안전하다.
        std::cout.exceptions(std::ios::failbit | std::ios::badbit);

        // CpuMonitor 객체가 CPU 개수 확인, 시간 측정, 평균 계산, 화면 출력을 모두 지휘한다.
        CpuMonitor monitor;
        monitor.run();
    } catch (const std::ios_base::failure& error) {
        // cout으로 결과를 쓸 수 없을 때 발생한다. CPU 계산 문제가 아니라
        // 출력 장치 또는 출력 연결의 문제이므로 별도의 메시지로 구분한다.
        std::cerr << "Fatal output error: " << error.what() << '\n';
        return EXIT_FAILURE;
    } catch (const std::system_error& error) {
        // 시작 단계에서 Windows API가 실패하면 오류 번호까지 출력한다.
        // 번호가 있으면 사용자가 Windows 문서나 오류 검색으로 원인을 찾기 쉽다.
        std::cerr << "Fatal Windows error (code " << error.code().value()
                  << "): " << error.what() << '\n';
        return EXIT_FAILURE;
    } catch (const std::exception& error) {
        // 위에서 따로 처리하지 않은 표준 C++ 오류를 놓치지 않는 마지막 안전망이다.
        // 아무 내용도 출력하지 않는 빈 catch를 사용하지 않아 실패 원인을 숨기지 않는다.
        std::cerr << "Fatal error: " << error.what() << '\n';
        return EXIT_FAILURE;
    }

    // run()이 정상적으로 돌아온 경우 운영체제에 성공 종료를 알린다.
    return EXIT_SUCCESS;
}
