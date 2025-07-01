#pragma once

#include <stdint.h>

namespace Tesix {

static consteval uintmax_t countCStrC(
    const char* const str
) {
    return str[0] != '\0' ? 1 + countCStrC(str + 1) : 0;
}

template<typename T, uintmax_t N>
static consteval uintmax_t countArrayC(
    T const (&)[N]
) {
    return N;
}

static inline const uint8_t* const asByteStr(
    const char* const str
) {
    return reinterpret_cast<const uint8_t* const>(str);
}

static inline uint8_t* const asByteStr(
    char* const str
) {
    return reinterpret_cast<uint8_t* const>(str);
}

static inline const char* const asCStr(
    const uint8_t* const str
) {
    return reinterpret_cast<const char* const>(str);
}

static inline char* const asCStr(
    uint8_t* const str
) {
    return reinterpret_cast<char* const>(str);
}

}
