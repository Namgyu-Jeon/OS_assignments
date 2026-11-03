// 파일 읽기, 스케줄링, 화면 출력을 차례로 연결하고 실패 이유를 한 곳에서 알린다.
// 사용법: sjf.exe <입력 파일>. 결과 파일을 따로 만들지 않고 화면에만 출력한다.
#include "ProcessFile.h"
#include "SchedulePrinter.h"
#include "SjfScheduler.h"

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>

// Windows의 넓은 문자열 시작점을 사용해 한글·공백이 포함된 경로도 받는다.
int wmain(const int argc, wchar_t* argv[]) {
    if (argc != 2) {
        std::cerr << "\nUsage: sjf.exe <input file>\n\n";
        return EXIT_FAILURE;
    }

    try {
        const auto processes = ReadProcesses(std::filesystem::path(argv[1]));
        const auto waitingTimes = SimulateSjf(processes);
        PrintSchedule(processes, waitingTimes, std::cout);
        return EXIT_SUCCESS;
    }
    catch (const std::exception& error) {
        std::cerr << "\nError: " << error.what() << "\n\n";
        return EXIT_FAILURE;
    }
}
