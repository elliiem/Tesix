#pragma once

#include "bits.hpp"
#include "helpers.hpp"

#include <cstddef>
#include <cstdint>

namespace Tesix {

namespace UTF8 {

static size_t codepointLen(uint32_t codepoint) {
    return 1 + (codepoint > 0x007F) + (codepoint > 0x07FF) + (codepoint > 0xFFFF);
}

static size_t codepointStringLen(uint32_t* str, size_t str_c) {
    size_t len = 0;

    for(size_t i = 0; i < str_c; i++) {
        len += codepointLen(str[i]);
    }

    return len;
}

static void encodeCodepointOneByte(uint32_t codepoint, uint8_t* buffer, size_t buffer_i) {
    buffer[buffer_i] = codepoint;
}

static void encodeCodepointTwoByte(uint32_t codepoint, uint8_t* buffer, size_t buffer_i) {
    buffer[buffer_i] = 0b11000000 | getBits(codepoint, Range::fromFor(6, 5));
    buffer[buffer_i + 1] = 0b10000000 | truncate(codepoint, 6);
}

static void encodeCodepointThreeByte(uint32_t codepoint, uint8_t* buffer, size_t buffer_i) {
    buffer[buffer_i] = 0b11100000 | getBits(codepoint, Range::fromFor(12, 4));
    buffer[buffer_i + 1] = 0b10000000 | getBits(codepoint, Range::fromFor(6, 6));
    buffer[buffer_i + 2] = 0b10000000 | truncate(codepoint, 6);
}

static void encodeCodepointFourByte(uint32_t codepoint, uint8_t* buffer, size_t buffer_i) {
    buffer[buffer_i] = 0b11110000 | getBits(codepoint, Range::fromFor(18, 3));
    buffer[buffer_i + 1] = 0b10000000 | getBits(codepoint, Range::fromFor(12, 6));
    buffer[buffer_i + 2] = 0b10000000 | getBits(codepoint, Range::fromFor(6, 6));
    buffer[buffer_i + 3] = 0b10000000 | truncate(codepoint, 6);
}

/**
 * @brief encodes a 32bit codepoint to a 8bit codepoint
 * at maximum writes 4 bytes use codepointLen() to check if there is space for this codepoint
 **/
static size_t encodeCodepoint(const uint32_t codepoint, uint8_t* buffer, const size_t buffer_i = 0) {
    const size_t octet_c = codepointLen(codepoint);

    if(octet_c == 2) {
        encodeCodepointTwoByte(codepoint, buffer, buffer_i);
        return 2;
    } else if(octet_c == 3) {
        encodeCodepointThreeByte(codepoint, buffer, buffer_i);
        return 3;
    } else if(octet_c == 4) {
        encodeCodepointFourByte(codepoint, buffer, buffer_i);
        return 4;
    } else {
        encodeCodepointOneByte(codepoint, buffer, buffer_i);
        return 1;
    }
}

static void encodeCodepointStr(const uint32_t* str, const size_t str_len, uint8_t* buffer) {
    size_t buffer_i = 0;

    for(size_t i = 0; i < str_len; i++) {
        buffer_i += encodeCodepoint(str[i], buffer, buffer_i);
    }
}

} // namespace UTF8

} // namespace Tesix
