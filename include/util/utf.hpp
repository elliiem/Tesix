#pragma once

#include "bits.hpp"
#include "array.hpp"

#include <stdint.h>
#include <stdexcept>

namespace Tesix {

namespace UTF {


}

namespace UTF32 {

static size_t countUTF8Single(
    const uint32_t codepoint
) {
    return 1 + (codepoint > 0x007F) + (codepoint > 0x07FF) + (codepoint > 0xFFFF);
}

static size_t countUTF8(
    uint32_t* const utf32,
    const uintmax_t utf32_c
) {
    size_t len = 0;

    for(size_t i = 0; i < utf32_c; i++) {
        len += countUTF8Single(utf32[i]);
    }

    return len;
}

/**
 * @brief encodes a 32bit codepoint to a 8bit codepoint
 * at maximum writes 4 bytes use codepointLen() to check if there is space for this codepoint
 **/
static size_t toUTF8Single(
    uint8_t* const dest,
    const uint32_t codepoint
) {
    const size_t octet_c = countUTF8Single(codepoint);

    switch(octet_c) {
        case 1: {
            dest[0] = codepoint;
        } break;
        case 2: {
            dest[0] = 0b11000000 | getBits(codepoint, Range::fromFor(6, 5));
            dest[1] = 0b10000000 | truncate(codepoint, 6);


        } break;
        case 3: {
            dest[0] = 0b11100000 | getBits(codepoint, Range::fromFor(12, 4));
            dest[1] = 0b10000000 | getBits(codepoint, Range::fromFor(6, 6));
            dest[2] = 0b10000000 | truncate(codepoint, 6);
        } break;
        case 4: {
            dest[0] = 0b11110000 | getBits(codepoint, Range::fromFor(18, 3));
            dest[1] = 0b10000000 | getBits(codepoint, Range::fromFor(12, 6));
            dest[2] = 0b10000000 | getBits(codepoint, Range::fromFor(6, 6));
            dest[3] = 0b10000000 | truncate(codepoint, 6);
        } break;
        default: {
        } break;
    }

    return octet_c;
}

static void toUTF8(
    uint8_t* const dest,
    const uint32_t* const utf32,
    const size_t utf32_c
) {
    uintmax_t cur = 0;

    for(uintmax_t i = 0; i < utf32_c; i++) {
        cur += toUTF8Single(dest + cur, utf32[i]);
    }
}

}

namespace UTF8 {

static uintmax_t octetCount(
    const uint8_t* const codepoint
) {
    if(rShiftExactOverflow(codepoint[0], 7) == 0b0) {
        return 1;
    }

    if(rShiftExactOverflow(codepoint[0], 5) == 0b110) {
        return 2;
    }

    if(rShiftExactOverflow(codepoint[0], 4) == 0b1110) {
        return 3;
    }

    if(rShiftExactOverflow(codepoint[0], 3) == 0b11110) {
        return 4;
    }

    throw std::runtime_error("invalid UTF8");
}

static uintmax_t countUTF32(
    const uint8_t* const utf8,
    const uintmax_t utf8_c
) {
    uintmax_t utf32_c = 0;
    uintmax_t utf8_cur = 0;

    while (utf8_cur < utf8_c) {
        const uintmax_t octet_c = octetCount(utf8 + utf8_cur);

        utf8_cur += octet_c;
        utf32_c++;
    }

    return utf32_c;
}

static void toUTF32Single(
    uint32_t* const dest,
    const uint8_t* const utf8,
    const uintmax_t octet_c
) {
    switch(octet_c) {
        case 1: {
            dest[0] = getBits(utf8[0], Range::fromFor(0, 7));
        } break;
        case 2: {
            uint32_t codepoint = 0;

            codepoint |= valueMask(utf8[0], Range::fromFor(6, 5));
            codepoint |= getBits(utf8[1], Range::fromFor(0, 6));

            dest[0] = codepoint;
        } break;
        case 3: {
            uint32_t codepoint = 0;

            codepoint |= valueMask(utf8[0], Range::fromFor(12, 4));
            codepoint |= valueMask(utf8[1], Range::fromFor(6, 6));
            codepoint |= getBits(utf8[2], Range::fromFor(0, 6));

            dest[0] = codepoint;
        } break;
        case 4: {
            uint32_t codepoint = 0;

            codepoint |= valueMask(utf8[0], Range::fromFor(18, 4));
            codepoint |= valueMask(utf8[1], Range::fromFor(12, 6));
            codepoint |= valueMask(utf8[2], Range::fromFor(6, 6));
            codepoint |= getBits(utf8[3], Range::fromFor(0, 6));

            dest[0] = codepoint;
        } break;
        default: {
        } break;
    }

}

static Array<uint32_t> toUTF32(
    const uint8_t* const utf8,
    const uintmax_t utf8_c
) {
    const uintmax_t utf32_c = countUTF32(utf8, utf8_c);

    uint32_t* const utf32 = Array<uint32_t>::allocRaw(utf32_c);

    uintmax_t utf8_cur = 0;
    uintmax_t utf32_cur = 0;

    while(utf32_cur < utf32_c) {
        const uintmax_t octet_c = octetCount(utf8 + utf8_cur);

        toUTF32Single(utf32 + utf32_cur, utf8 + utf8_cur, octet_c);

        utf8_cur += octet_c;
        utf32_cur++;
    }

    return Array<uint32_t>::fromRawFull(utf32, utf32_c);

}



} // namespace UTF8

} // namespace Tesix
