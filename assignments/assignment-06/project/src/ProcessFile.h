// 과제에서 제공한 ARRIVAL, CPU, I/O 형식의 파일을 읽는다.
#pragma once

#include "ProcessInfo.h"

#include <filesystem>
#include <vector>

// 파일의 줄 순서를 유지한다. 같은 시각에 도착한 프로세스의 우선순위에 쓰인다.
// 파일을 열거나 읽을 수 없으면 예외로 실패 이유를 전달한다.
std::vector<ProcessInfo> ReadProcesses(const std::filesystem::path& filename);
