// 입력 행렬과 계산 결과에 공통으로 필요한 크기와 원소 저장 공간을 관리한다.
// 파일을 읽거나 스레드를 만드는 일은 다른 파일에 맡겨, 행렬 자체의 역할만 둔다.
#pragma once

#include "MatrixValue.h"

#include <cstddef>
#include <stdexcept>
#include <vector>

// PDF에서 정한 입력 제한이다. 한 행렬의 행과 열은 각각 최대 100개다.
constexpr std::size_t kMaxMatrixSize = 100;
constexpr int kMinOperandCount = 2;
constexpr int kMaxOperandCount = 10;

// 저장하는 값의 종류만 바꿔 같은 행렬 구조를 재사용한다.
// 입력은 작은 정수지만 곱셈 결과는 커질 수 있어서 두 종류를 구분한다.
template <typename Value>
class Matrix {
public:
    // 유효한 크기의 행렬을 만들고, 모든 원소를 0으로 초기화한다.
    // 크기를 확인한 뒤 공간을 잡아 잘못된 크기로 메모리를 요청하지 않게 한다.
    Matrix(const std::size_t rows, const std::size_t columns)
        : rows_(rows), columns_(columns) {
        if (rows == 0 || rows > kMaxMatrixSize ||
            columns == 0 || columns > kMaxMatrixSize) {
            throw std::invalid_argument("Matrix dimensions must be between 1 and 100.");
        }
        values_.resize(rows * columns);
    }

    std::size_t Rows() const noexcept { return rows_; }
    std::size_t Columns() const noexcept { return columns_; }

    // 행과 열은 0부터 센다. 호출하는 쪽에서 범위 안의 좌표인지 보장한다.
    // 별도 행 포인터 없이 row * columns_ + column 위치의 원소를 찾는다.
    Value& At(const std::size_t row, const std::size_t column) noexcept {
        return values_[row * columns_ + column];
    }
    const Value& At(const std::size_t row, const std::size_t column) const noexcept {
        return values_[row * columns_ + column];
    }

private:
    std::size_t rows_;
    std::size_t columns_;
    // 한 줄로 이어진 공간에 행 순서대로 저장한다. 생성 후에는 크기를 바꾸지 않아
    // 작업 스레드가 받은 원소의 주소가 계산 도중 바뀌지 않는다.
    std::vector<Value> values_;
};

// 입력값은 -3~3이라 int면 충분하고, 중간 결과에는 더 큰 정수형이 필요하다.
using InputMatrix = Matrix<int>;
using ResultMatrix = Matrix<MatrixValue>;
