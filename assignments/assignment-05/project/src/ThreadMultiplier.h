// 결과 행렬의 각 칸을 서로 다른 Windows 작업 스레드에 맡긴다.
#pragma once

#include "Matrix.h"

// 첫 입력을 큰 정수를 담는 결과 행렬로 복사한다. 값과 행·열 크기는 그대로 유지한다.
ResultMatrix ToResultMatrix(const InputMatrix& input);
// left * right를 계산한 새 행렬을 반환한다. 차원이 맞지 않거나 Windows 호출이
// 실패하면 예외를 전달하며, 이미 시작한 작업은 끝날 때까지 기다린 뒤 정리한다.
ResultMatrix MultiplyWithThreads(const ResultMatrix& left, const InputMatrix& right);
