
#include "bits.hpp"
#include "helpers.hpp"

#include <cstdint>

namespace Tesix {

namespace Style {

constexpr inline uint32_t getForeground(const uint64_t style) {
    return getBitRange(style, Range::fromTo(0, 23));
}

constexpr inline uint8_t getForegroundR(const uint64_t style) {
    return getBitRange(style, Range::fromTo(0, 7));
}

constexpr inline uint8_t getForegroundG(const uint64_t style) {
    return getBitRange(style, Range::fromTo(8, 15));
}

constexpr inline uint8_t getForegroundB(const uint64_t style) {
    return getBitRange(style, Range::fromTo(16, 23));
}

constexpr inline void setForeground(uint64_t& style, const uint32_t value) {
    setBitRangeTo(style, value, Range::fromTo(0, 23));
}

constexpr inline void setForegroundR(const uint64_t style, const uint8_t value) {
    setBitRangeTo(style, value, Range::fromTo(0, 7));
}

constexpr inline void setForegroundG(const uint64_t style, const uint8_t value) {
    setBitRangeTo(style, value, Range::fromTo(8, 15));
}

constexpr inline void setForegroundB(uint64_t& style, const uint8_t value) {
    setBitRangeTo(style, value, Range::fromTo(16, 23));
}

constexpr inline uint32_t getBackground(const uint64_t style) {
    return getBitRange(style, Range::fromTo(24, 47));
}

constexpr inline void setBackground(uint64_t& style, const uint32_t value) {
    setBitRangeTo(style, value, Range::fromTo(24, 47));
}

constexpr inline uint8_t getTransparency(const uint64_t style) {
    return getBitRange(style, Range::fromTo(48, 55));
}

constexpr inline void setTransparency(uint64_t& style, const uint8_t value) {
    setBitRangeTo(style, value, Range::fromTo(48, 55));
}

constexpr inline bool isBold(const uint64_t style) {
    return getBit(style, 56);
}

constexpr inline void boldOn(uint64_t& style) {
    setBit(style, 56);
}

constexpr inline void boldOff(uint64_t& style) {
    unsetBit(style, 56);
}

constexpr inline bool isItalic(const uint64_t style) {
    return getBit(style, 57);
}

constexpr inline void italicOn(uint64_t& style) {
    setBit(style, 57);
}

constexpr inline void italicOff(uint64_t& style) {
    unsetBit(style, 57);
}

constexpr inline bool isUnderlined(const uint64_t style) {
    return getBit(style, 58);
}

constexpr inline void underlineOn(uint64_t& style) {
    setBit(style, 58);
}

constexpr inline void underlineOff(uint64_t& style) {
    unsetBit(style, 58);
}

constexpr inline bool isBlinking(const uint64_t style) {
    return getBit(style, 59);
}
constexpr inline void blinkingOn(uint64_t& style) {
    setBit(style, 59);
}

constexpr inline void blinkingOff(uint64_t& style) {
    unsetBit(style, 59);
}

} // namespace Style

} // namespace Tesix
