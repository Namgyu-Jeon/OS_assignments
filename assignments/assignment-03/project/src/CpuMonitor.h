// 이 파일은 CPU 모니터의 전체 진행 순서를 담당하는 CpuMonitor를 선언한다.
// Windows에서 값을 읽는 자세한 방법은 WindowsCpuReader에 맡기고,
// 최근 측정값을 보관하고 평균을 내는 일은 CpuLoadHistory에 맡긴다.
// CpuMonitor는 두 부품을 연결하고 언제 측정하고 무엇을 출력할지 결정하는
// "진행 관리자" 역할을 한다.

#pragma once

#include "CpuLoadHistory.h"
#include "WindowsCpuReader.h"

#include <cstdint>
#include <optional>

// CpuMonitor는 다음 순서를 관리한다.
// 1. 논리 CPU 개수를 한 번 출력한다.
// 2. CPU 누적시간의 첫 기준값을 저장하고 0번째 줄을 출력한다.
// 3. 약 1초마다 새 누적시간을 읽어 CPU 부하를 계산한다.
// 4. 정상 부하만 기록하고, 표본이 충분하면 5·10·15초 평균도 출력한다.
// run()에는 끝나는 횟수가 정해져 있지 않으며 사용자가 Ctrl+C로 종료할 때까지 반복한다.
// 시작 자체를 계속할 수 없는 치명적 오류는 main()이 처리할 수 있도록 밖으로 전달한다.
class CpuMonitor final {
public:
    // CPU 모니터링을 시작하는 공개 함수다.
    // 입력값과 반환값은 없다. 정상 실행 중에는 계속 반복하며, 출력 실패나
    // 복구할 수 없는 오류가 생기면 예외를 호출자인 main()에 전달한다.
    void run();

private:
    // 프로그램을 켠 즉시 보이는 0번째 줄을 출력한다.
    // dateTime은 그 순간의 실제 로컬 시각이다. 아직 비교할 이전 CPU 시간이 없으므로
    // 이 줄에는 날짜와 시각만 쓰고, 계산할 수 없는 CPU 부하는 억지로 0%라고 표시하지 않는다.
    static void printInitialLine(const LocalDateTime& dateTime);

    // 정상적으로 계산된 한 번의 측정 결과를 과제 형식에 맞춰 출력한다.
    // sampleNumber는 몇 번째 유효 표본인지, dateTime은 출력 시각,
    // loadPercent는 방금 측정한 CPU 부하를 뜻한다. average5/10/15는 각 구간의
    // 평균이 준비되었을 때만 값을 가지며, 준비되지 않았으면 해당 항목을 출력하지 않는다.
    // 화면 출력에 실패하면 cout의 예외가 발생하고 main()에서 처리한다.
    static void printSampleLine(
        std::uint64_t sampleNumber,
        const LocalDateTime& dateTime,
        double loadPercent,
        std::optional<double> average5,
        std::optional<double> average10,
        std::optional<double> average15);

    // LocalDateTime의 각 숫자를 YYYY.MM.DD HH:MM:SS 형식으로 출력한다.
    // 월·일·시·분·초가 한 자리여도 앞에 0을 붙여 열이 흐트러지지 않게 한다.
    // 이 함수는 공통 형식을 한곳에 모아 0번째 줄과 측정 줄이 다르게 보이는 실수를 막는다.
    static void printDateTime(const LocalDateTime& dateTime);

    // 지금까지 정상적으로 계산된 최근 CPU 부하를 보관한다.
    // CpuMonitor가 살아 있는 동안 같은 객체를 계속 사용해야 이동평균이 이어진다.
    CpuLoadHistory history_;
};
