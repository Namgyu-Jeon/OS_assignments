// 파일 형식을 해석하고 결과를 출력한다. 계산과 Windows 스레드 관리는 담당하지 않는다.
#pragma once

#include "Matrix.h"

#include <filesystem>
#include <string>

// 크기 설명 없이 숫자만 들어 있는 파일을 읽어 행과 열을 알아낸다.
// 읽을 수 없거나 값·크기·행 길이가 잘못되면 예외로 실패 이유를 전달한다.
InputMatrix ReadMatrix(const std::filesystem::path& filename);
// 행렬만 담은 문자열을 만든다. 화면과 파일이 이 문자열을 공유해 형식도 같게 유지한다.
std::string FormatMatrix(const ResultMatrix& matrix);
// 임시 파일 쓰기가 끝난 뒤 기존 결과를 교체한다. 저장에 실패하면 예외를 전달한다.
void SaveMatrix(const std::string& text, const std::filesystem::path& filename);
