#pragma once

#include "util/string.hpp"
#include "util/array.hpp"

#include <stdint.h>
#include <math.h>
#include <stdio.h>

namespace Tesix {

namespace Out {

static inline uintmax_t countDigits(
    const uintmax_t value
) {
    return value > 0 ? (int) log10((double) value) + 1 : 1;
}

static inline void appendUInt(
    Array<uint8_t>& dest,
    const uintmax_t val
) {
    dest._n += sprintf(asCStr(dest.end()), "%lu", val);
}

}

}
