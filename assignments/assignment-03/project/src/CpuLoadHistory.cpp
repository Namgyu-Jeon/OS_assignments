// 이 파일은 CpuLoadHistory의 원형 버퍼와 이동합 계산을 실제로 구현한다.
// 핵심 생각은 "새 값이 들어오면 평균 범위에서 가장 오래된 값 하나를 빼고
// 새 값 하나를 더한다"는 것이다. 덕분에 5개, 10개, 15개를 매초 다시
// 처음부터 합산하지 않아도 항상 최신 이동평균을 만들 수 있다.

#include "CpuLoadHistory.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

void CpuLoadHistory::add(const double loadPercent) {
    // 평균은 정상적인 백분율만 받아야 의미가 있다. std::isfinite()는 값이
    // 일반적인 유한 숫자인지 확인해 NaN과 양·음의 무한대를 걸러 준다.
    // 검사를 가장 먼저 하므로 잘못된 입력은 기존 배열이나 합계를 전혀 바꾸지 않는다.
    if (!std::isfinite(loadPercent) || loadPercent < 0.0 || loadPercent > 100.0) {
        throw std::invalid_argument("CPU load must be a finite value from 0 to 100.");
    }
    // totalSamples_는 64비트라 실제로 끝에 도달하기 매우 어렵지만,
    // 최대값에서 1을 더하면 0으로 되돌아가 화면 번호가 잘못될 수 있어 미리 막는다.
    if (totalSamples_ == std::numeric_limits<std::uint64_t>::max()) {
        throw std::overflow_error("CPU load sample counter overflow.");
    }

    // 먼저 각 이동평균 범위에서 밀려날 오래된 값을 합계에서 뺀다.
    // 5초 합계라면 이미 표본이 5개 이상 있을 때만 하나가 빠진다.
    // 예를 들어 10,20,30,40,50 뒤에 60이 오면 5초 범위에서 10을 빼고 60을 더해
    // 최근 다섯 값 20,30,40,50,60의 합을 만든다. 표본이 4개뿐이면 빠질 값이 없다.
    // indexLeavingWindow()는 nextIndex_에서 5, 10, 15칸 전 위치를 원형으로 찾는다.
    // 아직 samples_를 덮어쓰기 전이므로 그 위치에서 오래된 값을 안전하게 읽을 수 있다.
    if (validCount_ >= 5) {
        sum5_ -= samples_[indexLeavingWindow(5)];
    }
    if (validCount_ >= 10) {
        sum10_ -= samples_[indexLeavingWindow(10)];
    }
    if (validCount_ >= 15) {
        sum15_ -= samples_[indexLeavingWindow(15)];
    }

    // 빠져나갈 값을 뺀 다음 새 값을 세 합계에 공통으로 더한다.
    // sum5_에는 최근 최대 5개, sum10_에는 최근 최대 10개,
    // sum15_에는 최근 최대 15개의 합이 남는다. 각 범위에서 뺄셈 한 번과
    // 덧셈 한 번만 하므로 표본 수가 늘어도 처리 시간은 O(1)로 일정하다.
    sum5_ += loadPercent;
    sum10_ += loadPercent;
    sum15_ += loadPercent;

    samples_[nextIndex_] = loadPercent;

    // 방금 사용한 칸의 다음 칸을 새 기록 위치로 정한다.
    // % kCapacity 때문에 14 다음은 (14 + 1) % 15 = 0이 되어 배열 처음으로 돌아간다.
    // 이때 가장 오래된 값을 덮어쓰므로 실행 시간이 길어져도 배열은 15칸 그대로다.
    nextIndex_ = (nextIndex_ + 1) % kCapacity;

    // validCount_는 실제로 채워진 칸 수라 15까지만 증가한다.
    // totalSamples_는 화면 번호를 위한 전체 성공 횟수라 배열 순환 뒤에도 증가한다.
    validCount_ = std::min(validCount_ + 1, kCapacity);
    ++totalSamples_;
}

std::optional<double> CpuLoadHistory::average5() const noexcept {
    // 표본이 다섯 개보다 적으면 일부 값의 평균을 5초 평균이라고 부를 수 없다.
    if (validCount_ < 5) {
        return std::nullopt;
    }
    // sum5_는 add()가 항상 최근 다섯 값의 합으로 유지하므로 나누기만 하면 된다.
    return sum5_ / 5.0;
}

std::optional<double> CpuLoadHistory::average10() const noexcept {
    // 열 개가 모이기 전에는 "값 없음"을 반환해 출력 코드가 항목을 생략하게 한다.
    if (validCount_ < 10) {
        return std::nullopt;
    }
    // 배열을 다시 순회하지 않아 평균 조회도 O(1)이다.
    return sum10_ / 10.0;
}

std::optional<double> CpuLoadHistory::average15() const noexcept {
    // 가장 긴 평균은 정확히 열다섯 개 표본이 모인 뒤부터 사용할 수 있다.
    if (validCount_ < 15) {
        return std::nullopt;
    }
    // sum15_에는 원형 버퍼에 남아 있는 최근 열다섯 값의 합이 유지된다.
    return sum15_ / 15.0;
}

std::uint64_t CpuLoadHistory::totalSamples() const noexcept {
    // 저장된 최근 표본 수(validCount_)가 아니라 프로그램 시작 뒤 성공한 전체 횟수다.
    return totalSamples_;
}

std::size_t CpuLoadHistory::indexLeavingWindow(const std::size_t window) const noexcept {
    // nextIndex_에서 window칸 뒤로 가고 싶지만 size_t는 음수를 표현하지 못한다.
    // 먼저 배열 크기 15를 더한 다음 window를 빼면 항상 음수가 되지 않는다.
    // 마지막의 % 15가 결과를 0~14 범위로 접어 주어 배열 끝과 처음을 자연스럽게 잇는다.
    return (nextIndex_ + kCapacity - window) % kCapacity;
}
