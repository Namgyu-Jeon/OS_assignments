// 큰 정수를 9자리씩 나누어 계산한다. 작업 스레드 안에서는 메모리를 할당하지 않는다.
#include "MatrixValue.h"

MatrixValue::MatrixValue(const int value) noexcept {
    // 이 생성자로 들어오는 값은 검사를 마친 입력 원소(-3~3)뿐이다.
    sign_ = (value > 0) - (value < 0);
    digits_[0] = static_cast<std::uint32_t>(value < 0 ? -value : value);
}

void MatrixValue::AddProduct(const MatrixValue& value, const int factor) noexcept {
    // 더할 값이 0이면 현재 합계도 그대로다.
    if (factor == 0 || value.sign_ == 0) {
        return;
    }

    // 먼저 value * factor를 별도 공간에서 만든다. 부호와 절댓값은 따로 계산한다.
    MatrixValue product;
    product.sign_ = factor < 0 ? -value.sign_ : value.sign_;
    const auto multiplier = static_cast<std::uint32_t>(factor < 0 ? -factor : factor);
    // 낮은 자리부터 곱하고, 9자리를 넘는 부분은 다음 묶음으로 올린다.
    // 임시 계산은 64비트로 하여 묶음의 곱과 올림을 함께 안전하게 담는다.
    std::uint64_t carry = 0;
    for (std::size_t i = 0; i < digits_.size(); ++i) {
        const std::uint64_t part =
            static_cast<std::uint64_t>(value.digits_[i]) * multiplier + carry;
        product.digits_[i] = static_cast<std::uint32_t>(part % kBase);
        carry = part / kBase;
    }

    if (sign_ == 0) {
        // 기존 합계가 0이면 곱한 값을 그대로 받는다.
        *this = product;
        return;
    }

    if (sign_ == product.sign_) {
        // 부호가 같으면 절댓값끼리 더하고, 여기서도 자리올림을 전달한다.
        carry = 0;
        for (std::size_t i = 0; i < digits_.size(); ++i) {
            const std::uint64_t part =
                static_cast<std::uint64_t>(digits_[i]) + product.digits_[i] + carry;
            digits_[i] = static_cast<std::uint32_t>(part % kBase);
            carry = part / kBase;
        }
        return;
    }

    // 부호가 다르면 절댓값이 큰 쪽에서 작은 쪽을 빼고, 큰 쪽의 부호를 쓴다.
    // 큰 자리부터 비교해야 절댓값의 대소를 알 수 있다.
    int comparison = 0;
    for (std::size_t i = digits_.size(); i-- > 0;) {
        if (digits_[i] != product.digits_[i]) {
            comparison = digits_[i] > product.digits_[i] ? 1 : -1;
            break;
        }
    }
    if (comparison == 0) {
        // 크기가 같은 반대 부호의 값은 정확히 상쇄된다. 음수 0은 만들지 않는다.
        *this = MatrixValue{};
        return;
    }

    const MatrixValue larger = comparison > 0 ? *this : product;
    const MatrixValue smaller = comparison > 0 ? product : *this;
    // 낮은 자리에서 뺄 수 없으면 다음 묶음에서 1을 빌린다.
    // 한 묶음의 1은 아래 묶음에서는 kBase이므로 그만큼 더해서 계산한다.
    std::int64_t borrow = 0;
    for (std::size_t i = 0; i < digits_.size(); ++i) {
        std::int64_t part = static_cast<std::int64_t>(larger.digits_[i]) -
            smaller.digits_[i] - borrow;
        borrow = part < 0 ? 1 : 0;
        if (part < 0) {
            part += kBase;
        }
        digits_[i] = static_cast<std::uint32_t>(part);
    }
    sign_ = larger.sign_;
}

std::string MatrixValue::ToString() const {
    if (sign_ == 0) {
        return "0";
    }

    // 사용하지 않는 높은 자리의 0을 건너뛰어 숫자 앞에 불필요한 0이 붙지 않게 한다.
    std::size_t top = digits_.size() - 1;
    while (top > 0 && digits_[top] == 0) {
        --top;
    }
    std::string text = sign_ < 0 ? "-" : "";
    text += std::to_string(digits_[top]);
    // 맨 앞 묶음 뒤부터는 꼭 9자리로 맞춘다. 예를 들어 [1, 2]는
    // 21이 아니라 2,000,000,001을 뜻하므로 낮은 묶음 앞에 0을 채워야 한다.
    while (top > 0) {
        const std::string part = std::to_string(digits_[--top]);
        text.append(9 - part.size(), '0');
        text += part;
    }
    return text;
}
