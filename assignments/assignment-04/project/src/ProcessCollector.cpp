// Windows에서 프로세스 목록을 받아 하나씩 읽고, 사용한 목록은 마지막에 닫아 준다.
#include "ProcessCollector.h"

#include <Windows.h>
#include <TlHelp32.h>

#include <array>

namespace {

// 한 번 받아 둔 프로세스 목록(스냅샷)을 사용이 끝나면 자동으로 닫아 주는 클래스이다.
// 도중에 오류가 나도 닫히게 해서, 사용한 뒤 정리하는 일을 빠뜨리지 않도록 했다.
class SnapshotHandle {
public:
    explicit SnapshotHandle(HANDLE handle) noexcept : handle_(handle) {}

    ~SnapshotHandle() {
        if (handle_ != INVALID_HANDLE_VALUE) {
            CloseHandle(handle_);
        }
    }

    // 같은 스냅샷을 두 객체가 갖게 되면 두 번 닫을 수 있으므로 복사는 막아 둔다.
    SnapshotHandle(const SnapshotHandle&) = delete;
    SnapshotHandle& operator=(const SnapshotHandle&) = delete;

    HANDLE Get() const noexcept {
        return handle_;
    }

private:
    HANDLE handle_;
};

// 오류 번호를 Windows에 넘겨 사람이 읽을 수 있는 설명으로 바꾼다.
// 해당 번호의 설명을 받지 못하면, 설명이 없다는 문장을 대신 돌려준다.
std::wstring DescribeWindowsError(std::uint32_t errorCode) {
    std::array<wchar_t, 2048> buffer{};
    const DWORD length = FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, errorCode, 0, buffer.data(),
        static_cast<DWORD>(buffer.size()), nullptr);

    if (length == 0) {
        return L"Windows에서 오류 설명을 제공하지 않았습니다.";
    }

    // 설명 끝의 줄바꿈과 공백을 빼서, 출력할 때 불필요한 빈 줄이 생기지 않게 한다.
    std::wstring description(buffer.data(), length);
    const auto lastCharacter = description.find_last_not_of(L"\r\n ");
    if (lastCharacter != std::wstring::npos) {
        description.resize(lastCharacter + 1);
    }
    return description;
}

}

// 실패한 함수 이름과 오류 번호를 저장하고, 그 번호에 맞는 설명도 준비한다.
ProcessCollectionError::ProcessCollectionError(
    const char* apiName, std::uint32_t errorCode)
    : std::runtime_error(apiName),
      errorCode_(errorCode),
      description_(DescribeWindowsError(errorCode)) {}

std::uint32_t ProcessCollectionError::ErrorCode() const noexcept {
    return errorCode_;
}

const std::wstring& ProcessCollectionError::Description() const noexcept {
    return description_;
}

// 이름과 PID, PPID를 모두 읽어 목록으로 돌려준다. 읽는 데 실패하면 오류를 알린다.
std::vector<ProcessInfo> CollectProcesses() {
    // 목록을 읽는 사이에도 프로세스는 시작되거나 종료될 수 있다.
    // 서로 다른 시점의 정보가 섞이지 않도록, 처음에 받은 목록 하나만 사용한다.
    const HANDLE rawSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (rawSnapshot == INVALID_HANDLE_VALUE) {
        // 목록 자체를 받지 못한 경우이다. 실패 직후의 오류 번호를 바로 저장한다.
        const DWORD errorCode = GetLastError();
        throw ProcessCollectionError("CreateToolhelp32Snapshot", errorCode);
    }
    const SnapshotHandle snapshot(rawSnapshot);

    PROCESSENTRY32W entry{};
    // entry{}로 값을 먼저 비워 두고, Windows가 이 구조체를 읽을 수 있도록 크기를 적는다.
    // 첫 프로세스를 읽기 전에 dwSize를 설정해야 한다.
    entry.dwSize = sizeof(entry);

    std::vector<ProcessInfo> processes;
    // 먼저 첫 번째 프로세스를 읽는다. 항목이 하나도 없다는 응답은 실패로 보지 않는다.
    if (!Process32FirstW(snapshot.Get(), &entry)) {
        const DWORD errorCode = GetLastError();
        if (errorCode == ERROR_NO_MORE_FILES) {
            return processes;
        }
        throw ProcessCollectionError("Process32FirstW", errorCode);
    }

    // 읽어 온 항목의 이름과 PID, PPID를 저장한다.
    // 그다음 항목은 Process32NextW로 가져오며, 끝까지 같은 스냅샷을 사용한다.
    while (true) {
        processes.push_back({
            entry.szExeFile, entry.th32ProcessID, entry.th32ParentProcessID});

        if (!Process32NextW(snapshot.Get(), &entry)) {
            const DWORD errorCode = GetLastError();
            if (errorCode == ERROR_NO_MORE_FILES) {
                break; // 더 읽을 항목이 없으면 목록을 모두 읽은 것이므로 반복을 끝낸다.
            }
            // 다른 오류라면 읽기에 실패한 것이다. 지금까지 읽은 일부 목록도 내보내지 않는다.
            throw ProcessCollectionError("Process32NextW", errorCode);
        }
    }

    return processes;
}
