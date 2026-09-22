// 이 파일은 Windows 운영체제와 직접 대화하는 부분을 선언한다.
// 다른 파일이 Windows API의 복잡한 자료형을 모두 알 필요가 없도록,
// 필요한 정보를 평범한 C++ 구조체로 바꾸어 전달한다. 이 클래스 덕분에
// CpuMonitor는 "값을 어떻게 읽는가"보다 "읽은 값으로 무엇을 하는가"에 집중할 수 있다.

#pragma once

#include <cstdint>

// CpuTimes는 GetSystemTimes()에서 얻은 세 종류의 CPU 시간을 한 묶음으로 보관한다.
// idle은 CPU가 쉬었던 시간, kernel은 Windows 운영체제 코드와 idle을 포함한 시간,
// user는 일반 프로그램 코드가 실행된 시간이다. 세 값은 컴퓨터가 켜진 뒤부터
// 계속 증가하는 누적값이며, 어떤 한 순간의 CPU 사용률이 아니다.
// Windows가 주는 단위는 100나노초지만 같은 단위끼리 빼고 나누므로 초로 바꿀 필요가 없다.
// 32비트 범위를 쉽게 넘는 누적시간이므로 넉넉한 64비트 정수에 저장한다.
struct CpuTimes final {
    std::uint64_t idle = 0;
    std::uint64_t kernel = 0;
    std::uint64_t user = 0;
};

// LocalDateTime은 화면에 날짜와 시각을 쓰는 데 필요한 값만 모은 구조체다.
// Windows의 SYSTEMTIME 전체를 프로그램 곳곳에 전달하지 않고 필요한 여섯 필드만
// 복사하면 각 필드의 의미가 분명해지고 Windows 전용 자료형의 사용 범위도 작아진다.
struct LocalDateTime final {
    unsigned short year = 0;
    unsigned short month = 0;
    unsigned short day = 0;
    unsigned short hour = 0;
    unsigned short minute = 0;
    unsigned short second = 0;
};

// WindowsCpuReader는 과제에서 반드시 사용해야 하는 Windows API 세 개와
// CPU 부하 계산식을 한곳에 모은 도구 클래스다. 객체마다 따로 저장할 상태가 없으므로
// 모든 함수가 static이며 WindowsCpuReader 객체를 만들 필요가 없다.
// API 호출 실패는 Windows 오류 번호를 담은 std::system_error로 전달한다.
// API 값의 관계가 비정상이라 계산할 수 없으면 std::domain_error를 전달한다.
// 호출자는 이 두 오류를 구분해 잘못된 값을 평균에 넣지 않을 수 있다.
class WindowsCpuReader final {
public:
    // 현재 Windows가 사용할 수 있다고 알려 주는 논리 프로세서 개수를 반환한다.
    // 물리적인 CPU 칩 개수나 코어 개수와 완전히 같은 뜻은 아니며,
    // 작업 스케줄링에 사용되는 논리적인 처리 단위의 수다.
    // 입력값은 없다. GetSystemInfo() 결과가 비정상적으로 0이면 계속 실행하지 않고
    // std::runtime_error를 발생시킨다. 이 값은 정보 출력에만 쓰며 부하를 나누지 않는다.
    [[nodiscard]] static unsigned int logicalProcessorCount();

    // 호출한 시점의 idle/kernel/user 누적시간을 CpuTimes로 반환한다.
    // 입력값은 없고, 성공하면 세 누적값을 모두 채운 결과를 돌려준다.
    // GetSystemTimes()가 실패하면 GetLastError()를 즉시 읽어 Windows 오류 번호가
    // 사라지지 않게 보존한 뒤 std::system_error를 발생시킨다.
    [[nodiscard]] static CpuTimes readCpuTimes();

    // 호출한 바로 그 시점의 컴퓨터 로컬 날짜와 시간을 반환한다.
    // 측정할 때마다 새로 호출하므로 화면의 시각이 프로그램 시작 시각에 멈추지 않는다.
    // GetLocalTime()은 성공 여부를 반환하지 않는 Windows API라 별도 오류 번호가 없으며,
    // 함수 안에서 예외를 발생시키는 작업도 없으므로 noexcept로 표시했다.
    [[nodiscard]] static LocalDateTime readLocalDateTime() noexcept;

    // 이전 측정값 previous와 현재 측정값 current 사이의 CPU 부하를 계산해
    // 0~100 사이의 백분율로 반환한다. 두 값 자체가 아니라 각 항목의 차이를 사용한다.
    // 정상 예: 전체 시간 차이가 100이고 그중 쉰 시간 차이가 30이면 부하는 70%다.
    // 누적값이 감소했거나, 전체 시간 차이가 0이거나, 쉰 시간이 전체보다 큰 경우에는
    // 믿을 수 있는 결과가 아니므로 std::domain_error를 발생시킨다.
    [[nodiscard]] static double calculateCpuLoad(
        const CpuTimes& previous,
        const CpuTimes& current);

private:
    // 이 클래스는 상태를 가진 객체가 아니라 관련 함수들의 묶음이다.
    // 실수로 의미 없는 객체를 만들지 못하도록 기본 생성자를 삭제했다.
    WindowsCpuReader() = delete;
};
