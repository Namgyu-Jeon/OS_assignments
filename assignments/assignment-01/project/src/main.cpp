// 이 파일은 프로그램을 시작하고 Windows 콘솔의 UTF-8 출력 환경을 준비하는 역할을 합니다.
// 입력 검사와 표 출력의 세부 내용은 각 전용 모듈에 맡겨 시작 지점을 단순하게 유지합니다.

#include <Windows.h>
#include <iostream>

#include "app/MultiplicationTableApp.h"

// 프로그램의 시작점입니다.
// 운영체제가 전달한 argc와 argv를 앱에 그대로 전달하고, 앱이 정한 종료 코드를 반환합니다.
// 한국어 오류 안내가 코드 페이지 949 같은 기존 콘솔 설정에서 깨지지 않도록 출력 코드 페이지를 UTF-8로 맞춥니다.
// 이 호출은 Windows 기본 콘솔 기능만 사용하며 외부 라이브러리를 추가하지 않습니다.
int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);

    // C 표준 입출력과 iostream의 자동 동기화를 끄면 매 행을 출력할 때의 불필요한 조정 비용이 줄어듭니다.
    // 프로그램은 iostream만 사용하므로 순서가 바뀌지 않으며, 출력량이 큰 입력에서 특히 효과가 있습니다.
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    MultiplicationTableApp app;
    return app.Run(argc, argv);
}
