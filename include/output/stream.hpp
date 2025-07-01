#pragma once

#include "util/array.hpp"
#include "util/string.hpp"

#include <assert.h>
#include <stdint.h>
#include <unistd.h>

namespace Tesix {

namespace Out {

static inline void emptyOutBuffer(
    Array<uint8_t>& buf,
    const uintmax_t fd
) {
    write(fd, buf._ptr, buf._n);
    buf._n = 0;
}

static void const streamByte(
    Array<uint8_t>& buf,
    const uint8_t byte,
    const uintmax_t fd
) {
    assert(buf._cap > 0);

    const uintmax_t out_buf_rem = buf.remaining();

    if(out_buf_rem >= 1) {
        buf.append(byte);
        return;
    }

    emptyOutBuffer(buf, fd);

    buf.append(byte);
}

static void streamBytes(
    Array<uint8_t>& buf,
    const uint8_t* const src,
    const uintmax_t src_c,
    const uintmax_t fd
) {
    assert(buf._cap > 0);

    const uint8_t* src_cur = src;

    const uintmax_t out_buf_rem = buf.remaining();

    if(out_buf_rem >= src_c) {
        buf.appendMulti(src_cur, src_c);
        return;
    }

    buf.appendMulti(src_cur, out_buf_rem);
    src_cur += out_buf_rem;

    emptyOutBuffer(buf, fd);

    for(uintmax_t i = 0; i < (src_c - out_buf_rem) / buf._cap; i++) {
        buf.appendMulti(src_cur, buf._cap);
        src_cur += buf._cap;

        emptyOutBuffer(buf, fd);
    }

    const uintmax_t src_rem = (src_c - out_buf_rem) % buf._cap;

    buf.appendMulti(src_cur, src_rem);
}

static void streamUInt(
    Array<uint8_t>& buf,
    const uintmax_t val,
    const uintmax_t fd
) {
    uint8_t number_buf[21];

    const uintmax_t digits = sprintf(asCStr(number_buf), "%lu", val);

    return streamBytes(buf, number_buf, digits, fd);
}

}

}
