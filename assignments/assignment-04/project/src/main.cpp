// 프로세스 목록을 읽고, 부모와 자식을 연결해서 한 번 출력한 뒤 종료한다.
// 작업 중 문제가 생기면 이유를 보여 주고 실패했다는 종료 값을 돌려준다.
#include "ProcessCollector.h"
#include "ProcessTree.h"
#include "ProcessTreePrinter.h"

#include <fcntl.h>
#include <io.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>

int main() {
    // 한글이나 다른 언어가 들어 있는 이름도 깨지지 않도록 출력 방식을 설정한다.
    // 화면 대신 파일에 출력을 저장하면 UTF-16LE 형식으로 기록된다.
    if (_setmode(_fileno(stdout), _O_U16TEXT) == -1 ||
        _setmode(_fileno(stderr), _O_U16TEXT) == -1) {
        std::fprintf(stderr, "Unicode output initialization failed (errno=%d).\n", errno);
        return EXIT_FAILURE;
    }

    // 정보를 모두 읽은 다음 트리를 만들고 출력한다. 덜 읽은 목록은 출력하지 않는다.
    try {
        const auto processes = CollectProcesses();
        const auto tree = BuildProcessTree(processes);
        PrintProcessTree(tree, std::wcout);
        // 마지막 내용까지 출력한 뒤, 출력 중 문제가 없었는지 확인한다.
        std::wcout.flush();
        if (!std::wcout) {
            std::wcerr << L"프로세스 트리를 출력하지 못했습니다. 출력 대상을 확인하세요.\n";
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    // 목록을 읽는 데 실패하면, 실패한 Windows 함수와 오류 번호, 설명을 보여 준다.
    catch (const ProcessCollectionError& error) {
        std::wcerr << L"프로세스 수집 실패 (" << error.what()
                   << L"). Windows 오류 코드 = " << error.ErrorCode()
                   << L"\n" << error.Description() << L'\n';
    }
    catch (const std::exception& error) {
        std::wcerr << L"프로세스 트리 처리 실패: " << error.what() << L'\n';
    }
    return EXIT_FAILURE;
}
