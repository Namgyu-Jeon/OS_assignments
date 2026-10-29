// 프로그램 전체 순서와 실패 처리를 담당한다. 읽기·출력과 스레드 계산은 전담 함수에 맡긴다.
// 사용법: mm <행렬 파일> <행렬 파일> ... (2~10개). 입력이 없으면 사용법을 알리고 종료한다.
#include "MatrixFile.h"
#include "ThreadMultiplier.h"

#include <chrono>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

// 넓은 문자열을 받는 Windows 시작점을 사용해 한글이 포함된 입력 경로도 전달받는다.
// argv[0]은 실행 파일 이름이라 입력 개수에서 제외한다. 성공은 0, 실패는 0이 아닌 값이다.
int wmain(const int argc, wchar_t* argv[]) {
    // 입력 읽기, 스레드 생성과 대기, 계산, 결과 저장 및 화면 출력을 모두 잰다.
    // 측정용 시계는 날짜·시각을 바꾸어도 거꾸로 흐르지 않는 steady_clock을 선택했다.
    const auto started = std::chrono::steady_clock::now();
    if (argc - 1 < kMinOperandCount || argc - 1 > kMaxOperandCount) {
        std::cerr << "Usage: mm <matrix file> <matrix file> [matrix file ...]\n"
                  << "Provide 2 to 10 input matrix files.\n";
        return EXIT_FAILURE;
    }

    try {
        // 과제에서 지정한 출력 이름이다. 실행 파일 위치가 아니라 현재 작업 폴더에 저장한다.
        const std::filesystem::path outputPath = L"result.txt";
        std::error_code pathError;
        const bool outputExists = std::filesystem::exists(outputPath, pathError);
        if (pathError) {
            throw std::runtime_error("Cannot inspect result.txt before calculation.");
        }
        // 전체 입력을 먼저 읽고 이웃 행렬의 크기를 확인한다. 뒤쪽 입력에 문제가 있는데
        // 앞쪽 곱셈을 먼저 시작하거나 기존 결과 파일을 바꾸는 일을 피하기 위해서다.
        std::vector<InputMatrix> inputs;
        inputs.reserve(static_cast<std::size_t>(argc - 1));
        for (int i = 1; i < argc; ++i) {
            const std::filesystem::path inputPath(argv[i]);
            inputs.push_back(ReadMatrix(inputPath));
            // 이름만 비교하면 같은 파일의 다른 경로를 놓칠 수 있어 실제 파일을 비교한다.
            // result.txt 자체나 같은 파일의 다른 이름을 입력으로 쓰면 덮어쓰지 않는다.
            if (outputExists) {
                const bool sameFile = std::filesystem::equivalent(inputPath, outputPath, pathError);
                if (pathError) {
                    throw std::runtime_error("Cannot check whether result.txt is also an input file.");
                }
                if (sameFile) {
                    throw std::runtime_error("result.txt must not also be an input file.");
                }
            }
            if (inputs.size() >= 2 &&
                inputs[inputs.size() - 2].Columns() != inputs.back().Rows()) {
                throw std::invalid_argument("Adjacent matrix dimensions do not match.");
            }
        }

        // A*B의 작업 스레드가 모두 끝난 뒤 그 결과에 C를 곱한다.
        // 순서를 바꾸지 않아 매 단계의 오른쪽 피연산자는 항상 원래 입력 행렬이다.
        ResultMatrix result = ToResultMatrix(inputs.front());
        // 교수님 예제처럼 각 단계의 입력·결과 크기를 보여 준다.
        // 먼저 문자열에 모아 두고, 전체 계산과 저장이 성공한 뒤 화면에 출력한다.
        std::ostringstream steps;
        for (std::size_t i = 1; i < inputs.size(); ++i) {
            steps << '(' << result.Rows() << 'x' << result.Columns()
                  << ") x (" << inputs[i].Rows() << 'x' << inputs[i].Columns()
                  << ") = (" << result.Rows() << 'x' << inputs[i].Columns() << ")\n";
            result = MultiplyWithThreads(result, inputs[i]);
        }
        if (!steps) {
            throw std::runtime_error("Cannot format the multiplication steps.");
        }

        // 모든 단계가 성공했을 때만 결과를 저장한다. 파일은 행렬만 담고,
        // 화면에는 단계별 크기, 같은 최종 행렬, 전체 처리 시간을 차례로 보여 준다.
        const std::string formatted = FormatMatrix(result);
        SaveMatrix(formatted, outputPath);
        // 명령과 단계 안내, 단계 안내와 행렬 사이에 빈 줄을 하나씩 둔다.
        // 단계 안내와 빈 줄은 화면용이므로 결과 파일에는 넣지 않는다.
        std::cout << '\n' << steps.str() << '\n' << formatted;
        std::cout.flush();
        if (!std::cout) {
            throw std::runtime_error("Cannot finish printing the result matrix.");
        }

        // 행렬이 화면 출력 버퍼에 남지 않도록 flush한 뒤 측정을 끝낸다.
        // 마지막 시간 안내문을 출력하는 시간 자체는 이 값에 포함하지 않는다.
        const double seconds = std::chrono::duration<double>(
            std::chrono::steady_clock::now() - started).count();
        std::cout << "\nProcessing Time: " << std::fixed << std::setprecision(3)
                  << seconds << " sec\n\n" << std::flush;
        if (!std::cout) {
            throw std::runtime_error("Cannot print the processing time.");
        }
        return EXIT_SUCCESS;
    }
    // 실패한 하위 함수의 설명을 한 곳에서 출력한다. 오류를 정상 결과로 돌려주지 않는다.
    catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
    }
    return EXIT_FAILURE;
}
