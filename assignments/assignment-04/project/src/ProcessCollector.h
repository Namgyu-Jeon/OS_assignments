// 현재 프로세스 목록을 가져오는 함수와, 실패했을 때 알려 줄 오류 정보를 모아 둔다.
#pragma once

#include "ProcessInfo.h"

#include <stdexcept>
#include <string>
#include <vector>

// 어느 Windows 함수에서 실패했는지, 오류 번호와 설명을 함께 담는다.
// main에서는 이 정보를 받아 실패한 이유를 화면에 보여 준다.
class ProcessCollectionError : public std::runtime_error {
public:
    ProcessCollectionError(const char* apiName, std::uint32_t errorCode);

    std::uint32_t ErrorCode() const noexcept;
    const std::wstring& Description() const noexcept;

private:
    std::uint32_t errorCode_;
    std::wstring description_;
};

// 실행 시점의 프로세스 목록을 한 번 받아 끝까지 읽은 뒤 돌려준다.
// 중간에 읽기가 실패하면, 덜 읽은 목록을 완성된 결과처럼 돌려주지 않고 오류를 알린다.
std::vector<ProcessInfo> CollectProcesses();
