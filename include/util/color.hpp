#pragma once

#include <cstdint>
#include <stdint.h>

namespace Tesix {

struct Color24 {
    uint8_t _r;
    uint8_t _g;
    uint8_t _b;

    inline bool operator==(
        const Color24& other
    ) const {
        return _r == other._r && _g == other._g && _b == other._b;
    }
};

struct Color32 {
    uint8_t _r;
    uint8_t _g;
    uint8_t _b;
    uint8_t _a;

    inline bool operator==(
        const Color32& other
    ) const {
        return _r == other._r && _g == other._g && _b == other._b && _a == other._a;
    }

    Color24 truncate() const {
        return {._r = _r, ._g = _g, ._b = _b};
    }
};

struct PaletteColor {
    uint8_t _n;
    uint8_t _r;
    uint8_t _g;
    uint8_t _b;

    inline bool operator==(
        const PaletteColor& other
    ) const {
        return _n == other._n && _r == other._r && _g == other._g && _b == other._b;
    }
};

}
