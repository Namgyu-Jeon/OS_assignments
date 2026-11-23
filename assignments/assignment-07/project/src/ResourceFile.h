// 입력 파일의 표와 자원 개수를 읽어 ResourceState로 변환하는 함수를 선언한다.
#pragma once

#include "ResourceState.h"

#include <filesystem>

// 자원 종류 수와 프로세스 수, Allocation, Max, Available 순서로 읽는다.
// 파일을 열 수 없거나 필요한 값이 없으면 예외를 발생시켜 main에서 오류를 알린다.
ResourceState ReadResourceState(const std::filesystem::path& filename);
