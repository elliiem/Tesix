#pragma once


#include <cstddef>

namespace Tesix {

struct Box {
    size_t _x = 0;
    size_t _y = 0;
    size_t _width = 0;
    size_t _height = 0;
};

struct Range {
    size_t _start;
    size_t _len;

    constexpr static Range fromFor(const size_t start, const size_t len) {
        return {._start = start, ._len = len};
    }

    constexpr static Range fromZeroFor(const size_t len) {
        return {._start = 0, ._len = len};
    }

    constexpr static Range fromTo(const size_t start, const size_t end) {
        return {._start = start, ._len = end - start};
    }

    constexpr inline bool isInside(const size_t i) {
        return i >= _start && i < _start + _len;
    }

    constexpr inline bool isInside(const Range child) {
        return child._start >= _start && child._start + child._len < _start + _len;
    }
};


} // namespace Tesix
