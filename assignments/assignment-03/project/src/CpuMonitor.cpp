// 이 파일은 CpuMonitor의 실제 실행 순서를 구현한다.
// 쉽게 말하면 "1초 기다리기 -> CPU 값 읽기 -> 부하 계산하기 -> 평균에 넣기
// -> 한 줄 출력하기"를 계속 반복한다. 운영체제에서 값을 가져오는 세부 과정과
// 평균 자료구조의 세부 과정은 각각 전담 클래스에 맡겨 이 파일에서는
// 프로그램 전체 흐름을 한눈에 볼 수 있게 했다.

#include "CpuMonitor.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <system_error>
#include <thread>

void CpuMonitor::run() {
    // GetSystemInfo()로 읽은 현재 컴퓨터의 논리 프로세서 개수다.
    // 이 값은 사용자에게 시스템 정보를 보여 주기 위한 것이며,
    // GetSystemTimes()가 준 전체 CPU 부하를 이 개수로 다시 나누면 안 된다.
    const unsigned int processorCount = WindowsCpuReader::logicalProcessorCount();
    std::cout << "Number of CPUs : " << processorCount << '\n';

    // GetSystemTimes()는 "지금 CPU가 몇 퍼센트인가"를 직접 주지 않고,
    // 컴퓨터가 켜진 뒤 지금까지 쌓인 시간을 준다. 자동차 주행거리계만 보고는
    // 최근 1시간에 몇 km를 갔는지 모르는 것과 같다. 두 시점의 주행거리 차이가
    // 필요한 것처럼 CPU 부하도 두 시점의 누적시간 차이가 필요하다.
    // 따라서 첫 값은 계산 결과가 아니라 다음 값과 비교할 출발점으로만 저장한다.
    CpuTimes previousTimes = WindowsCpuReader::readCpuTimes();

    // 막 시작한 시점에는 두 번째 CPU 값이 없으므로 부하를 표시하지 않는다.
    // 대신 프로그램이 정상적으로 시작되었음을 보여 주는 0번째 시각만 출력한다.
    printInitialLine(WindowsCpuReader::readLocalDateTime());

    // 실제 날짜·시간은 사용자가 바꾸거나 인터넷 시간 동기화로 앞뒤로 움직일 수 있다.
    // steady_clock은 그런 조정과 관계없이 일정한 방향으로만 흐르는 측정용 시계이므로
    // "1초 간격"을 관리하는 데 사용한다. 화면에 보여 줄 시각은 별도로 GetLocalTime()을 쓴다.
    using SteadyClock = std::chrono::steady_clock;
    constexpr auto kMeasurementInterval = std::chrono::seconds(1);

    // 첫 CPU 부하는 기준값을 얻은 뒤 약 1초가 지나야 계산할 수 있다.
    auto nextMeasurementTime = SteadyClock::now() + kMeasurementInterval;

    // 사용자가 Ctrl+C로 끝낼 때까지 표본 수에 제한 없이 같은 과정을 반복한다.
    for (;;) {
        // sleep_for(1초)를 쓰면 "1초 대기 + 계산 시간 + 출력 시간"이 한 주기가 되어
        // 매번 조금씩 늦어진다. sleep_until()은 미리 정한 다음 시각까지 기다리므로
        // 계산과 출력에 걸린 짧은 시간이 다음 주기에 계속 더해지는 현상을 줄인다.
        // 잠든 동안에는 CPU를 붙잡고 반복 확인하지 않으므로 busy waiting도 발생하지 않는다.
        std::this_thread::sleep_until(nextMeasurementTime);

        // API 호출이나 계산이 실패할 수도 있으므로 성공 여부를 따로 기록한다.
        // 실패했는데 초기값 0.0을 실제 CPU 부하로 착각해 평균에 넣지 않기 위한 장치다.
        CpuTimes currentTimes{};
        double loadPercent = 0.0;
        bool measurementSucceeded = false;

        try {
            // 약 1초가 지난 현재 시점의 누적 CPU 시간을 다시 읽는다.
            currentTimes = WindowsCpuReader::readCpuTimes();

            // 이전 누적값과 현재 누적값의 차이가 최근 측정 구간의 CPU 시간이다.
            // 누적값 자체를 백분율로 사용하면 컴퓨터를 켠 뒤의 전체 시간이 섞이므로 틀리다.
            loadPercent = WindowsCpuReader::calculateCpuLoad(previousTimes, currentTimes);
            measurementSucceeded = true;
        } catch (const std::system_error& error) {
            // Windows가 CPU 시간을 주지 못한 경우다. 오류 번호를 함께 출력하면
            // 실패 원인을 찾기 쉽다. 이 한 번의 표본은 버리지만 프로그램은 다음 초에
            // 다시 시도하여 일시적인 API 오류 때문에 전체 프로그램이 바로 끝나지 않게 한다.
            std::cerr << "CPU measurement error (Windows code "
                      << error.code().value() << "): " << error.what() << '\n';
        } catch (const std::domain_error& error) {
            // API 호출은 성공했지만 누적시간이 거꾸로 갔거나 전체 시간이 0인 등
            // 정상적인 백분율을 만들 수 없는 경우다. 잘못된 값을 평균에 섞지 않고 버린다.
            std::cerr << "CPU calculation error: " << error.what() << '\n';
        }

        if (measurementSucceeded) {
            // 계산에 성공한 현재 누적값만 다음 측정의 출발점으로 사용한다.
            // 그리고 정상적인 부하만 history_에 추가한다. 실패한 표본을 0% 등으로
            // 대신 넣지 않기 때문에 5·10·15초 평균도 실제 측정값만 반영한다.
            previousTimes = currentTimes;
            history_.add(loadPercent);

            // totalSamples()는 성공한 표본만 세므로 화면 번호와 평균의 표본 수가 일치한다.
            // 출력 도중 생긴 오류는 위 try-catch 바깥에서 발생해 main()까지 전달된다.
            // 따라서 화면에 쓸 수 없는데도 측정만 무한히 계속하는 상황을 막는다.
            printSampleLine(
                history_.totalSamples(),
                WindowsCpuReader::readLocalDateTime(),
                loadPercent,
                history_.average5(),
                history_.average10(),
                history_.average15());
        }

        // 다음 예정 시각은 "작업이 끝난 지금부터 1초 뒤"가 아니라
        // 원래 예정 시각에 정확히 1초를 더해 계산한다. 이것이 장시간 누적 지연을 줄인다.
        nextMeasurementTime += kMeasurementInterval;
        const auto afterWork = SteadyClock::now();
        if (nextMeasurementTime <= afterWork) {
            // 컴퓨터가 잠들었거나 매우 바빠서 다음 예정 시각까지 이미 지나간 경우,
            // 놓친 횟수를 따라잡으려고 여러 줄을 쉬지 않고 출력하면 각 줄이 1초 측정값처럼
            // 보이는 문제가 생긴다. 이때는 현재 시각을 새 기준으로 삼아 1초 뒤부터 다시 잰다.
            nextMeasurementTime = afterWork + kMeasurementInterval;
        }
    }
}

// 프로그램 시작 시각을 "0 YYYY.MM.DD HH:MM:SS :" 형태로 출력한다.
// 부하와 평균을 쓰지 않는 이유는 아직 두 CPU 누적값 사이의 시간 구간이 없기 때문이다.
void CpuMonitor::printInitialLine(const LocalDateTime& dateTime) {
    std::cout << std::setfill(' ') << std::setw(2) << 0 << ' ';
    printDateTime(dateTime);
    std::cout << " :\n";
}

// 한 번의 정상 측정값을 과제 예시와 같은 한 줄로 출력한다.
// optional 평균은 "값이 없을 수 있는 변수"다. 예를 들어 표본이 4개뿐이면
// average5에는 값이 없으므로 5초 평균이라는 잘못된 이름으로 부분 평균을 출력하지 않는다.
void CpuMonitor::printSampleLine(
    const std::uint64_t sampleNumber,
    const LocalDateTime& dateTime,
    const double loadPercent,
    const std::optional<double> average5,
    const std::optional<double> average10,
    const std::optional<double> average15) {
    std::cout << std::setfill(' ') << std::setw(2) << sampleNumber << ' ';
    printDateTime(dateTime);
    std::cout << " :" << std::fixed << std::setprecision(2)
              << " [CPU Load:" << std::setw(6) << loadPercent << "%]";

    // 각 optional에 실제 값이 있을 때만 해당 평균 항목을 이어서 출력한다.
    // 따라서 5번째 표본부터 5초 평균, 10번째부터 10초 평균,
    // 15번째부터 15초 평균이 자연스럽게 나타난다.
    if (average5) {
        std::cout << " [5sec avg:" << std::setw(6) << *average5 << "%]";
    }
    if (average10) {
        std::cout << " [10sec avg:" << std::setw(6) << *average10 << "%]";
    }
    if (average15) {
        std::cout << " [15sec avg:" << std::setw(6) << *average15 << "%]";
    }
    std::cout << '\n';
}

// 날짜와 시각을 담당하는 공통 출력 함수다.
// setfill('0')과 setw()를 함께 사용해 9월을 09로, 3초를 03으로 표시한다.
// 마지막에 채움 문자를 공백으로 되돌려 뒤의 CPU 백분율 정렬에 0이 붙지 않게 한다.
void CpuMonitor::printDateTime(const LocalDateTime& dateTime) {
    std::cout << std::setfill('0')
              << std::setw(4) << dateTime.year << '.'
              << std::setw(2) << dateTime.month << '.'
              << std::setw(2) << dateTime.day << ' '
              << std::setw(2) << dateTime.hour << ':'
              << std::setw(2) << dateTime.minute << ':'
              << std::setw(2) << dateTime.second
              << std::setfill(' ');
}
