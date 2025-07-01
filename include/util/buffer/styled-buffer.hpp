#pragma once

#include "util/buffer.hpp"

#include <stdint.h>

namespace Tesix {

struct StyledBufferArea {
    BufferArea<uint32_t> _ch;
    BufferArea<Style::StyleContainer> _style;

    inline StyledBufferArea area(
        const FloatingBox& area
    ) {
        return {._ch = _ch.area(area), ._style = _style.area(area)};
    }
};

struct StyledBuffer {
    Buffer<uint32_t> _ch;
    Buffer<Style::StyleContainer> _style;

    static inline StyledBuffer init(
        const uintmax_t width,
        const uintmax_t height
    ) {
        return {
            ._ch = Buffer<uint32_t>::init(width, height),
            ._style = Buffer<Style::StyleContainer>::init(width, height)
        };
    }

    inline StyledBufferArea area(
        const FloatingBox& area
    ) {
        return {._ch = _ch.area(area), ._style = _style.area(area)};
    }

    inline StyledBufferArea all() {
        return {._ch = _ch.all(), ._style = _style.all()};
    }
};

}
