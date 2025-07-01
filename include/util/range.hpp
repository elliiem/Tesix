#pragma once


#include <cstddef>

namespace Tesix {

struct Range {
    size_t start;
    size_t len;

    constexpr static Range fromFor(const size_t start, const size_t len) {
        return {.start = start, .len = len};
    }

    constexpr static Range fromZeroFor(const size_t len) {
        return {.start = 0, .len = len};
    }

    constexpr static Range fromTo(const size_t start, const size_t end) {
        return {.start = start, .len = end - start};
    }

    constexpr inline bool isInside(const size_t i) {
        return i >= start && i < start + len;
    }

    constexpr inline bool isInside(const Range child) {
        return child.start >= start && child.start + child.len < start + len;
    }
};


} // namespace Tesix
