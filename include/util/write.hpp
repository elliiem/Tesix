#pragma once

#include "util/string.hpp"

#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

namespace Tesix {

static inline uint8_t* const writeBytes(
    uint8_t* const buf,
    const uint8_t* const str,
    const uintmax_t str_c
) {
    memcpy(buf, str, str_c);
    return buf + str_c;
}

static inline uint8_t* const writeByteMulti(
    uint8_t* const buf,
    const uint8_t byte,
    const uintmax_t byte_c
) {
    memset(buf, byte, byte_c);
    return buf + byte_c;
}

static inline uint8_t* const writeByte(
    uint8_t* const buf,
    const uint8_t byte
) {
    buf[0] = byte;
    return buf + 1;
}

static inline uintmax_t countDigits(
    const uintmax_t value
) {
    return value > 0 ? (int) log10((double) value) + 1 : 1;
}

static uint8_t* writeUInt(
    uint8_t* const dest,
    const uintmax_t val
) {
    return dest + sprintf(asCStr(dest), "%lu", val);
}

}
