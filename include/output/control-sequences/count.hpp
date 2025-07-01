#pragma once

#include "util/color.hpp"
#include "util/write.hpp"

#include <stdint.h>

namespace Tesix {

namespace Out {

namespace Ctrl {

static inline uintmax_t countOneParameterEsc(
    const uintmax_t x
) {
    return countDigits(x) + 3;
}

static inline uintmax_t countTwoParameterEsc(
    const uintmax_t x,
    const uintmax_t y
) {
    return countDigits(x) + countDigits(y) + 4;
}

static consteval uintmax_t countColorForeground() {
    return 5;
}

static inline uintmax_t countColorBackground(
    const uint8_t n
) {
    return (n < 8 ? 2 : 3) + 3;
}

static constexpr uintmax_t countModifierOn() {
    return 4;
}

static constexpr uintmax_t countModifierOff() {
    return 5;
}

static inline uintmax_t countFullColor(
    const Color24& color
) {
    return countDigits(color._r) + countDigits(color._g) + countDigits(color._b) + 8;
}

}

}

}
