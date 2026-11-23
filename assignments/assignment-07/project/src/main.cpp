// 명령행에서 받은 입력 파일을 읽고, 자원 상태를 검사한 뒤 결과를 화면에 출력한다.
// 입력 처리와 계산, 출력은 각각의 함수에 맡기고 이 파일에서는 실행 순서를 정한다.
// 사용 예: safety.exe input1.txt 또는 safety.exe input-custom.txt
#include "ResourceFile.h"
#include "SafetyAlgorithm.h"
#include "StatePrinter.h"

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>

// argc는 실행 파일 이름을 포함한 인수 개수이고, argv[1]은 검사할 파일의 경로다.
// 한글 경로도 전달받을 수 있도록 Windows에서 wchar_t 인수를 받는 wmain을 사용한다.
int wmain(const int argc, wchar_t* argv[]) {
    // 이번 과제는 실행할 때 입력 파일 하나를 지정한다. 파일을 빠뜨리거나
    // 여러 개를 전달한 경우에는 계산을 시작하지 않고 올바른 사용법을 보여 준다.
    if (argc != 2) {
        std::cerr << "\nUsage: safety.exe <input file>\n\n";
        return EXIT_FAILURE;
    }

    try {
        // 파일의 숫자를 읽어 상태를 만든 다음 그 상태로 안전성을 계산한다.
        // 검사 함수가 반환한 완료 순서와 미완료 목록을 출력 함수에 전달한다.
        const auto state = ReadResourceState(std::filesystem::path(argv[1]));
        const auto result = CheckSafety(state);
        PrintSafetyResult(state, result, std::cout);
        // Safe와 UnSafe는 둘 다 정상적인 검사 결과이므로 성공 값인 0을 반환한다.
        // 파일을 열지 못하는 등의 실행 오류만 아래에서 실패로 처리한다.
        return EXIT_SUCCESS;
    }
    catch (const std::exception& error) {
        // 읽기, 계산, 출력 중 발생한 예외를 여기서 받아 원인을 알리고 종료한다.
        std::cerr << "\nError: " << error.what() << "\n\n";
        return EXIT_FAILURE;
    }
}
