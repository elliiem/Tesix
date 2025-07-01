#pragma once

#include "output/debug/symbols.hpp"
#include "output/common.hpp"

#include "util/string.hpp"

#include <inttypes.h>

namespace Tesix {

namespace Debug {

enum class MsgType {
    Debug,
    Error,
};


template<MsgType Type>
static uint8_t* writeType(
    uint8_t* const buf
);

template<>
uint8_t* writeType<MsgType::Debug>(
    uint8_t* const buf
) {
    auto buf_rem = buf;

    buf_rem = writeBytes(buf_rem, asByteStr(MSG_TYPE_DEBUG), cstrlen(MSG_TYPE_DEBUG));
    buf_rem = writeBytes(buf_rem, asByteStr(MSG_TYPE_EQL), cstrlen(MSG_TYPE_EQL));

    return buf_rem;
}

template<>
uint8_t* writeType<MsgType::Error>(
    uint8_t* const buf
) {
    auto buf_rem = buf;

    buf_rem = writeBytes(buf_rem, asByteStr(MSG_TYPE_ERROR), cstrlen(MSG_TYPE_ERROR));
    buf_rem = writeBytes(buf_rem, asByteStr(MSG_TYPE_EQL), cstrlen(MSG_TYPE_EQL));

    return buf_rem;
}


template<MsgType Type>
constexpr static inline uintmax_t countHeader(
    const uintmax_t msg_c
);

template<>
constexpr inline uintmax_t countHeader<MsgType::Debug>(
    const uintmax_t msg_c
) {
    return cstrlen(MSG_TYPE_DEBUG) + cstrlen(MSG_TYPE_EQL) + msg_c + cstrlen(HEADER_TERM);
}

template<>
constexpr inline uintmax_t countHeader<MsgType::Error>(
    const uintmax_t msg_c
) {
    return cstrlen(MSG_TYPE_ERROR) + cstrlen(MSG_TYPE_EQL) + msg_c + cstrlen(HEADER_TERM);
}


template<MsgType Type>
static uint8_t* const writeHeader(
    uint8_t* const buf,
    const uint8_t* const msg,
    const uintmax_t msg_c
) {
    auto buf_rem = buf;

    buf_rem = writeType<Type>(buf_rem);
    buf_rem = writeBytes(buf_rem, msg, msg_c);
    buf_rem = writeBytes(buf_rem, asByteStr(HEADER_TERM), cstrlen(HEADER_TERM));

    return buf_rem;
}

}

}
