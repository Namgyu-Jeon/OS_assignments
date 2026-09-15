// 이 파일은 Windows에서 읽어 온 프로세스 수와 현재 시각의 자료형을 정의합니다.
// Windows API 사용법은 구현 파일에 감추어 다른 코드가 결과만 간단히 쓰게 합니다.

#pragma once

#include <cstddef>

namespace WindowsSystemInfo
{
// GetLocalTime 결과 중 화면에 표시할 연도부터 초까지의 값만 담습니다.
struct LocalDateTime final
{
    unsigned int year{};
    unsigned int month{};
    unsigned int day{};
    unsigned int hour{};
    unsigned int minute{};
    unsigned int second{};
};

// 조회 성공 여부와 프로세스 수를 함께 전달합니다.
// success가 false일 때는 windowsErrorCode로 Windows API의 실패 원인을 확인합니다.
struct ProcessCountResult final
{
    bool success{};
    std::size_t count{};
    unsigned long windowsErrorCode{};
};

// 별도 입력 없이 Windows에 현재 PID 목록을 요청하고 그 개수를 반환합니다.
// API가 실패하면 오류 코드를 결과에 담고, 메모리 할당 실패는 main의 예외 처리로 전달합니다.
[[nodiscard]] ProcessCountResult QueryProcessCount();

// 별도 입력 없이 현재 로컬 시각을 읽어 LocalDateTime으로 반환합니다.
// GetLocalTime은 실패 값을 반환하지 않는 API이므로 이 함수도 예외 없이 동작합니다.
[[nodiscard]] LocalDateTime GetCurrentLocalDateTime() noexcept;
} // namespace WindowsSystemInfo
