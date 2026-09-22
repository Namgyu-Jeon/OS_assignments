// 이 파일은 최근 CPU 부하 표본을 보관하고 이동평균을 계산하는 클래스를 선언한다.
// 이동평균은 매초 새 값이 들어올 때마다 계산 범위가 한 칸씩 앞으로 움직이는 평균이다.
// 예를 들어 여섯 번째 표본에서 5초 평균은 1~5번이 아니라 2~6번을 사용한다.
// 과제에서 필요한 가장 긴 범위가 15초이므로 오래된 값을 끝없이 저장하지 않는다.

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

// CpuLoadHistory는 15칸짜리 배열을 원형으로 반복 사용한다.
// 원형 버퍼는 15개의 좌석에 번호를 붙이고 새 사람이 올 때 가장 오래 앉아 있던
// 사람의 자리를 다시 쓰는 것과 비슷하다. 배열 끝에 도착하면 0번 칸으로 돌아간다.
// 따라서 프로그램을 며칠 실행해도 저장 공간은 15칸에서 늘어나지 않는다.
// 또한 최근 값을 매번 처음부터 모두 더하지 않고, 범위에서 빠지는 값을 빼고
// 새 값을 더하는 이동합을 사용한다. 표본 하나를 처리하는 연산 수가 항상 비슷하므로
// 추가와 평균 조회의 시간복잡도는 O(1), 추가 공간복잡도도 O(1)이다.
class CpuLoadHistory final {
public:
    // 새 CPU 부하 백분율 하나를 기록한다. loadPercent는 정상적인 0~100 사이 값이어야 한다.
    // NaN, 무한대, 음수, 100보다 큰 값은 평균을 망가뜨리므로 std::invalid_argument로 거부한다.
    // 전체 표본 번호가 64비트 최대값에 도달하면 더한 뒤 0으로 돌아가지 않도록
    // std::overflow_error를 발생시킨다. 두 검사는 배열과 합계를 바꾸기 전에 수행하므로
    // 실패해도 이미 저장된 정상 기록은 그대로 유지된다.
    void add(double loadPercent);

    // 각각 최근 5개, 10개, 15개 표본의 평균을 반환한다.
    // std::optional은 "평균값이 있거나 아직 없을 수 있음"을 표현하는 자료형이다.
    // 요청한 수만큼 표본이 쌓이지 않았으면 일부 값만으로 잘못된 이름의 평균을
    // 만들지 않고 std::nullopt를 반환한다. 이 함수들은 상태를 바꾸지 않고 예외도 없다.
    [[nodiscard]] std::optional<double> average5() const noexcept;
    [[nodiscard]] std::optional<double> average10() const noexcept;
    [[nodiscard]] std::optional<double> average15() const noexcept;

    // 프로그램 시작 뒤 add()에 성공한 전체 표본 수를 반환한다.
    // 배열에는 최근 15개만 남지만 이 번호는 계속 증가하므로 화면의 측정 번호로 쓸 수 있다.
    // 상태를 읽기만 하므로 const이고, 단순 정수 반환이라 예외가 없어 noexcept다.
    [[nodiscard]] std::uint64_t totalSamples() const noexcept;

private:
    // 과제에서 필요한 가장 긴 평균이 15초이므로 저장 공간도 정확히 15칸이면 충분하다.
    static constexpr std::size_t kCapacity = 15;

    // 새 값을 쓸 nextIndex_를 기준으로, 지정한 평균 범위에서 이번에 빠질
    // 오래된 표본의 배열 위치를 계산한다. window에는 5, 10, 15 중 하나가 들어온다.
    // 나머지 연산(%)을 사용하므로 배열 끝을 지나면 자동으로 앞쪽 인덱스로 돌아간다.
    // add()가 기존 칸을 덮어쓰기 전에 호출해야 아직 오래된 값을 읽을 수 있다.
    [[nodiscard]] std::size_t indexLeavingWindow(std::size_t window) const noexcept;

    // samples_는 실제 CPU 부하를 담는 15칸 배열이다. 한 바퀴 돈 뒤에는 배열의
    // 물리적인 순서와 시간 순서가 같지 않을 수 있으므로 nextIndex_가 중요하다.
    std::array<double, kCapacity> samples_{};

    // 다음 새 값을 기록할 배열 위치다. 0에서 14까지 이동한 뒤 다시 0이 된다.
    std::size_t nextIndex_ = 0;

    // 배열 안에 실제로 채워진 유효 표본 수다. 처음에는 증가하다가 15에서 멈춘다.
    std::size_t validCount_ = 0;

    // 프로그램 시작 뒤 성공적으로 받은 전체 표본 수다. 배열이 순환해도 계속 증가한다.
    std::uint64_t totalSamples_ = 0;

    // 다음 세 이동합은 항상 각각 "현재 시점의 최근 최대 5개, 10개, 15개"의 합이다.
    // 이 약속을 계속 유지하면 평균을 구할 때 배열을 다시 돌지 않고 나누기만 하면 된다.
    double sum5_ = 0.0;
    double sum10_ = 0.0;
    double sum15_ = 0.0;
};
