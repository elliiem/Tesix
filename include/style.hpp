#pragma once

#include "bits.hpp"
#include "helpers.hpp"

#include <cstdint>
#include <utility>

namespace Tesix {

namespace Style {

enum class StyleEncoding {
    FCFM = 0, // Full Color Full Modifier Without Font

    //  NOTE: Maybe Implement these

    // RCFMF, // Reduced Color Full Modifier Font
    // RCIAFM // Reduced Color Increased Alpha Full Modifier Without Font
};

static inline StyleEncoding getEncoding(const uint64_t style) {
    return static_cast<StyleEncoding>(getBits(style, Range::fromFor(62, 2)));
}

static inline void setEnconding(uint64_t& style, const StyleEncoding encoding) {
    setBitRangeTo(style, static_cast<uint8_t>(encoding), Range::fromFor(62, 2));
}

enum class ColorMode {
    Default = 0,   // Restores Back to the default color using ^[[0m
    FullColor = 1, // 24bit colors
    Palette = 2,   // Palette colors
};

namespace FCFM {

static inline uint8_t getFCR(const uint32_t color) {
    return getBits(color, Range::fromFor(0, 8));
}

static inline uint8_t getFCG(const uint32_t color) {
    return getBits(color, Range::fromFor(8, 8));
}

static inline uint8_t getFCB(const uint32_t color) {
    return getBits(color, Range::fromFor(16, 8));
}

static inline uint8_t getFCA(const uint32_t color) {
    return getBits(color, Range::fromFor(24, 4));
}

static inline void setFCR(uint32_t& color, const uint8_t value) {
    setBitRangeTo(color, value, Range::fromFor(0, 8));
}

static inline void setFCG(uint32_t& color, const uint8_t value) {
    setBitRangeTo(color, value, Range::fromFor(8, 8));
}

static inline void setFCB(uint32_t& color, const uint8_t value) {
    setBitRangeTo(color, value, Range::fromFor(16, 8));
}

static inline void setFCA(uint32_t& color, const uint8_t value) {
    setBitRangeTo(color, value, Range::fromFor(14, 4));
}

static inline ColorMode getFgMode(const uint64_t style) {
    return static_cast<ColorMode>(getBits(style, Range::fromFor(24, 2)));
}

static inline uint8_t getPFg(const uint64_t style) {
    return getBits(style, Range::fromFor(0, 4));
}

static inline uint32_t getFCFg(const uint64_t style) {
    return getBits(style, Range::fromFor(0, 24));
}

static inline uint8_t getFCFgR(const uint64_t style) {
    return getFCR(getFCFg(style));
}

static inline uint8_t getFCFgG(const uint64_t style) {
    return getFCG(getFCFg(style));
}

static inline uint8_t getFCFgB(const uint64_t style) {
    return getFCB(getFCFg(style));
}

static inline ColorMode getBgMode(const uint64_t style) {
    return static_cast<ColorMode>(getBits(style, Range::fromFor(54, 2)));
}

static inline uint8_t getPBg(const uint64_t style) {
    return getBits(style, Range::fromFor(26, 4));
}

static inline uint32_t getFCBg(const uint64_t style) {
    return getBits(style, Range::fromFor(26, 28));
}

static inline uint8_t getFCBgR(const uint64_t style) {
    return getFCR(getFCBg(style));
}

static inline uint8_t getFCBgG(const uint64_t style) {
    return getFCG(getFCBg(style));
}

static inline uint8_t getFCBgB(const uint64_t style) {
    return getFCB(getFCBg(style));
}

static inline uint8_t getFCBgA(const uint64_t style) {
    return getFCA(getFCBg(style));
}

static inline bool getBold(const uint64_t style) {
    return getBit(style, 56);
}

static inline bool getItalic(const uint64_t style) {
    return getBit(style, 57);
}

static inline bool getUnderlined(const uint64_t style) {
    return getBit(style, 58);
}

static inline bool getBlinking(const uint64_t style) {
    return getBit(style, 59);
}

static inline bool getReversed(const uint64_t style) {
    return getBit(style, 60);
}

static inline bool getStrikethrough(const uint64_t style) {
    return getBit(style, 61);
}

static inline void setFgMode(uint64_t& style, const ColorMode mode) {
    setBitRangeTo(style, static_cast<uint8_t>(mode), Range::fromFor(24, 2));
}

static inline void setPFg(uint64_t& style, const uint8_t value) {
    assert(value < 16);

    setBitRangeTo(style, value, Range::fromFor(0, 4));
}

static inline void setFCFg(uint64_t& style, const uint32_t value) {
    setBitRangeTo(style, value, Range::fromFor(0, 24));
}

static inline void setFCFgR(uint64_t& style, const uint8_t value) {
    setBitRangeTo(style, value, Range::fromFor(0, 8));
}

static inline void setFCFgG(uint64_t& style, const uint8_t value) {
    setBitRangeTo(style, value, Range::fromFor(8, 8));
}

static inline void setFCFgB(uint64_t& style, const uint8_t value) {
    setBitRangeTo(style, value, Range::fromFor(16, 8));
}

static inline void setBgMode(uint64_t& style, const ColorMode mode) {
    setBitRangeTo(style, static_cast<uint8_t>(mode), Range::fromFor(54, 2));
}

static inline void setPBg(uint64_t& style, const uint8_t value) {
    assert(value < 16);

    setBitRangeTo(style, value, Range::fromFor(26, 4));
}

static inline void setFCBg(uint64_t& style, const uint32_t value) {
    setBitRangeTo(style, value, Range::fromFor(26, 28));
}

static inline void setFCBgR(uint64_t& style, const uint8_t value) {
    setBitRangeTo(style, value, Range::fromFor(26, 8));
}

static inline void setFCBgG(uint64_t& style, const uint8_t value) {
    setBitRangeTo(style, value, Range::fromFor(34, 8));
}

static inline void setFCBgB(uint64_t& style, const uint8_t value) {
    setBitRangeTo(style, value, Range::fromFor(42, 8));
}

static inline void setFCBgA(uint64_t& style, const uint8_t value) {
    setBitRangeTo(style, value, Range::fromFor(50, 4));
}

static inline void setBold(uint64_t& style, const bool value) {
    if(value) {
        setBit(style, 56);
    } else {
        unsetBit(style, 56);
    }
}

static inline void setItalic(uint64_t& style, const bool value) {
    if(value) {
        setBit(style, 57);
    } else {
        unsetBit(style, 57);
    }
}

static inline void setUnderlined(uint64_t& style, const bool value) {
    if(value) {
        setBit(style, 58);
    } else {
        unsetBit(style, 58);
    }
}

static inline void setBlinking(uint64_t& style, const bool value) {
    if(value) {
        setBit(style, 59);
    } else {
        unsetBit(style, 59);
    }
}

static inline void setReversed(uint64_t& style, const bool value) {
    if(value) {
        setBit(style, 60);
    } else {
        unsetBit(style, 60);
    }
}

static inline void setStrikethrough(uint64_t& style, const bool value) {
    if(value) {
        setBit(style, 61);
    } else {
        unsetBit(style, 61);
    }
}

} // namespace FCFM

struct FCFMFgFC {
    uint8_t _r = 255;
    uint8_t _g = 255;
    uint8_t _b = 255;

    bool operator==(const FCFMFgFC& other) const {
        return _r == other._r && _g == other._g && _b == other._b;
    }
};

union FCFMFgUnion {
    FCFMFgFC FC;
    uint8_t P;
};

struct FCFMFg {
    ColorMode _tag = ColorMode::Default;
    FCFMFgUnion _value;
};

union Fg {
    FCFMFg FCFM = {};
};

struct FCFMBgFC {
    uint8_t _r = 0;
    uint8_t _g = 0;
    uint8_t _b = 0;
    uint8_t _a = 127;

    bool operator==(const FCFMBgFC& other) const {
        return _r == other._r && _g == other._g && _b == other._b && _a == other._a;
    }
};

union FCFMBgUnion {
    FCFMBgFC FC;
    uint8_t P;
};

struct FCFMBg {
    ColorMode _tag = ColorMode::Default;
    FCFMBgUnion _value;
};

union Bg {
    FCFMBg FCFM = {};
};

struct FCFMMod {
    bool _bold = false;
    bool _italic = false;
    bool _underlined = false;
    bool _blinking = false;
    bool _reversed = false;
    bool _strikethrough = false;
};

union Mod {
    FCFMMod FCFM = {};
};

struct FullColor {
    uint8_t _r;
    uint8_t _g;
    uint8_t _b;
    uint8_t _a;
};

struct Style {
    StyleEncoding _tag;

    Fg _fg;
    Bg _bg;
    Mod _mod;

    static Style fromEncoding(const uint64_t style) {
        switch(getEncoding(style)) {
            case StyleEncoding::FCFM: {
                FCFMFg fg = {};

                switch(FCFM::getFgMode(style)) {
                    case ColorMode::Default: {
                        fg._tag = ColorMode::Default;
                    } break;
                    case ColorMode::FullColor: {
                        fg._tag = ColorMode::FullColor;

                        fg._value.FC._r = FCFM::getFCFgR(style);
                        fg._value.FC._g = FCFM::getFCFgG(style);
                        fg._value.FC._b = FCFM::getFCFgB(style);
                    } break;
                    case ColorMode::Palette: {
                        fg._tag = ColorMode::Palette;

                        fg._value.P = FCFM::getPFg(style);
                    } break;
                }

                FCFMBg bg = {};

                switch(FCFM::getBgMode(style)) {
                    case ColorMode::Default: {
                        bg._tag = ColorMode::Default;
                    } break;
                    case ColorMode::FullColor: {
                        bg._tag = ColorMode::FullColor;

                        bg._value.FC._r = FCFM::getFCBgR(style);
                        bg._value.FC._g = FCFM::getFCBgG(style);
                        bg._value.FC._b = FCFM::getFCBgB(style);
                        bg._value.FC._a = FCFM::getFCBgA(style);
                    } break;
                    case ColorMode::Palette: {
                        bg._tag = ColorMode::Palette;

                        bg._value.P = FCFM::getPBg(style);
                    } break;
                }

                FCFMMod mod = {
                    ._bold = FCFM::getBold(style),
                    ._italic = FCFM::getItalic(style),
                    ._underlined = FCFM::getUnderlined(style),
                    ._blinking = FCFM::getBlinking(style),
                    ._reversed = FCFM::getReversed(style),
                    ._strikethrough = FCFM::getStrikethrough(style),
                };

                return {
                    ._tag = StyleEncoding::FCFM,
                    ._fg = {.FCFM = fg},
                    ._bg = {.FCFM = bg},
                    ._mod = {.FCFM = mod},
                };
            } break;
        }

        return {};
    }

    inline uint64_t toEncoding() {
        uint64_t style = 0;

        switch(_tag) {
            case StyleEncoding::FCFM: {
                setEnconding(style, StyleEncoding::FCFM);

                switch(_fg.FCFM._tag) {
                    case ColorMode::Default: {
                        FCFM::setFgMode(style, ColorMode::Default);
                    } break;
                    case ColorMode::FullColor: {
                        FCFM::setFgMode(style, ColorMode::FullColor);

                        FCFM::setFCFgR(style, _fg.FCFM._value.FC._r);
                        FCFM::setFCFgG(style, _fg.FCFM._value.FC._g);
                        FCFM::setFCFgB(style, _fg.FCFM._value.FC._b);
                    } break;
                    case ColorMode::Palette: {
                        FCFM::setFgMode(style, ColorMode::Palette);

                        FCFM::setPFg(style, _fg.FCFM._value.P);
                    } break;
                }

                switch(_bg.FCFM._tag) {
                    case ColorMode::Default: {
                        FCFM::setBgMode(style, ColorMode::Default);
                    } break;
                    case ColorMode::FullColor: {
                        FCFM::setBgMode(style, ColorMode::FullColor);

                        FCFM::setFCBgR(style, _bg.FCFM._value.FC._r);
                        FCFM::setFCBgG(style, _bg.FCFM._value.FC._g);
                        FCFM::setFCBgB(style, _bg.FCFM._value.FC._b);
                        FCFM::setFCBgA(style, _bg.FCFM._value.FC._a);
                    } break;
                    case ColorMode::Palette: {
                        FCFM::setBgMode(style, ColorMode::Palette);

                        FCFM::setPBg(style, _bg.FCFM._value.P);
                    } break;
                }

                FCFM::setBold(style, _mod.FCFM._bold);
                FCFM::setItalic(style, _mod.FCFM._italic);
                FCFM::setUnderlined(style, _mod.FCFM._underlined);
                FCFM::setBlinking(style, _mod.FCFM._blinking);
                FCFM::setReversed(style, _mod.FCFM._reversed);
                FCFM::setStrikethrough(style, _mod.FCFM._strikethrough);
            } break;
        }

        return style;
    }

    inline Style& mode(const StyleEncoding mode) {
        _tag = mode;
        return *this;
    }

    inline Style& fgFullColor(const FullColor& value) {
        switch(_tag) {
            case StyleEncoding::FCFM: {
                _fg.FCFM._tag = ColorMode::FullColor;

                _fg.FCFM._value.FC._r = value._r;
                _fg.FCFM._value.FC._g = value._g;
                _fg.FCFM._value.FC._b = value._b;
            } break;
        }

        return *this;
    }

    inline Style& fgPalette(const uint8_t value) {
        switch(_tag) {
            case StyleEncoding::FCFM: {
                _fg.FCFM._tag = ColorMode::Palette;
                _fg.FCFM._value.P = value;

            } break;
        }

        return *this;
    }

    inline Style& fgDefault() {
        switch(_tag) {
            case StyleEncoding::FCFM: {
                _fg.FCFM._tag = ColorMode::Default;
            } break;
        }

        return *this;
    }

    inline Style& bgFullColor(const FullColor& value) {
        switch(_tag) {
            case StyleEncoding::FCFM: {
                _bg.FCFM._tag = ColorMode::FullColor;

                _bg.FCFM._value.FC._r = value._r;
                _bg.FCFM._value.FC._g = value._g;
                _bg.FCFM._value.FC._b = value._b;
                _bg.FCFM._value.FC._a = value._a;
            } break;
        }

        return *this;
    }

    inline Style& bgPalette(const uint8_t value) {
        switch(_tag) {
            case StyleEncoding::FCFM: {
                _bg.FCFM._tag = ColorMode::Palette;
                _bg.FCFM._value.P = value;
            } break;
        }

        return *this;
    }

    inline Style& bgDefault() {
        switch(_tag) {
            case StyleEncoding::FCFM: {
                _bg.FCFM._tag = ColorMode::Default;
            } break;
        }

        return *this;
    }

    inline Style& bold(const bool value = true) {
        switch(_tag) {
            case StyleEncoding::FCFM: {
                _mod.FCFM._bold = value;
            } break;
        }

        return *this;
    }

    inline Style& italic(const bool value = true) {
        switch(_tag) {
            case StyleEncoding::FCFM: {
                _mod.FCFM._italic = value;
            } break;
        }

        return *this;
    }

    inline Style& underlined(const bool value = true) {
        switch(_tag) {
            case StyleEncoding::FCFM: {
                _mod.FCFM._underlined = value;
            } break;
        }

        return *this;
    }

    inline Style& blinking(const bool value = true) {
        switch(_tag) {
            case StyleEncoding::FCFM: {
                _mod.FCFM._blinking = value;
            } break;
        }

        return *this;
    }

    inline Style& reverse(const bool value = true) {
        switch(_tag) {
            case StyleEncoding::FCFM: {
                _mod.FCFM._reversed = value;
            } break;
        }

        return *this;
    }

    inline Style& strikethrough(const bool value = true) {
        switch(_tag) {
            case StyleEncoding::FCFM: {
                _mod.FCFM._strikethrough = value;
            } break;
        }

        return *this;
    }
};

} // namespace Style

enum class StyleContainerTag {
    Value,
    Ptr,
};

union StyleContainerU {
    uint64_t V;
    const uint64_t* P;
};

struct StyleContainer {
    StyleContainerTag _tag;
    StyleContainerU _value;

    static inline StyleContainer createPtr(const uint64_t* style) {
        return {._tag = StyleContainerTag::Ptr, ._value = {.P = style}};
    }

    static inline StyleContainer createValue(const uint64_t style) {
        return {._tag = StyleContainerTag::Value, ._value = {.V = style}};
    }

    inline uint64_t value() const {
        switch(_tag) {
            case StyleContainerTag::Ptr: {
                return *_value.P;
            } break;
            case StyleContainerTag::Value: {
                return _value.V;
            } break;
        }
    }
};


} // namespace Tesix
