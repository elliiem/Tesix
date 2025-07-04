#pragma once

#include "util/range.hpp"

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdio>
#include <limits>

namespace Tesix {

template<typename T>
consteval size_t countBits() {
    return sizeof(T) * 8;
}

template<typename T>
constexpr inline bool isValidBitRange(
    const Range range
) {
    return countBits<T>() >= range.start + range.len;
}

template<typename T>
constexpr inline bool isValidBitIndex(
    const size_t i
) {
    return countBits<T>() > i;
}

template<typename T>
constexpr inline size_t remainingBits(
    const size_t bits
) {
    return countBits<T>() - bits;
}

template<typename T = size_t>
consteval inline T setMask() {
    return std::numeric_limits<T>::max();
}

consteval inline size_t unsetMask() {
    return 0;
}

constexpr inline size_t bitMask(
    const size_t i
) {
    assert(isValidBitIndex<size_t>(i));

    return (size_t)0b1 << i;
}

constexpr inline size_t firstBitsMask(
    const size_t n
) {
    assert(countBits<size_t>() >= n);

    return (n == countBits<size_t>()) ? setMask() : ((size_t)0b1 << n) - 1;
}

constexpr inline size_t lShiftExactOverflow(
    const size_t bits,
    const size_t shift
) {
    assert(isValidBitRange<size_t>(Range::fromZeroFor(shift)));

    return (shift == countBits<size_t>()) ? unsetMask() : (bits << shift);
}

constexpr inline size_t rShiftExactOverflow(
    const size_t bits,
    const size_t shift
) {
    assert(isValidBitRange<size_t>(Range::fromZeroFor(shift)));

    return (shift == countBits<size_t>()) ? unsetMask() : (bits >> shift);
}

constexpr inline size_t leftCutoffMask(
    const size_t lbits
) {
    assert(countBits<size_t>() >= lbits);

    return firstBitsMask(remainingBits<size_t>(lbits));
}

constexpr inline size_t cutoff(
    const size_t bits,
    const size_t lbits,
    const size_t rbits
) {
    assert(countBits<size_t>() >= lbits + rbits);

    return rShiftExactOverflow(lShiftExactOverflow(bits, lbits), lbits + rbits);
}

constexpr inline size_t cutoffLeft(
    const size_t bits,
    const size_t lbits
) {
    assert(countBits<size_t>() >= lbits);

    return bits & leftCutoffMask(lbits);
}

constexpr inline size_t cutoffRight(
    const size_t bits,
    const size_t rbits
) {
    assert(countBits<size_t>() >= rbits);

    return rShiftExactOverflow(bits, rbits);
}

constexpr inline size_t truncate(
    const size_t bits,
    const size_t count
) {
    assert(countBits<size_t>() >= count);

    return bits & firstBitsMask(count);
}

template<typename T = size_t>
constexpr inline T patternMask(
    const size_t pattern,
    const size_t pattern_c,
    size_t len = std::numeric_limits<size_t>::max()
) {
    len = std::min(len, countBits<T>());

    const size_t full_repeats = len / pattern_c;

    T ret = 0;

    for(size_t i = 0; i < full_repeats; i++) {
        ret |= pattern << i * pattern_c;
    }

    ret |= truncate(pattern, len - (full_repeats * pattern_c)) << full_repeats * pattern_c;

    return ret;
}

inline size_t rangeMask(
    const Range range
) {
    assert(isValidBitRange<size_t>(range));

    size_t lbits = countBits<size_t>() - (range.start + range.len);
    size_t rbits = range.start;

    return cutoff(setMask(), lbits, rbits) << range.start;
}

constexpr inline size_t valueMask(
    const size_t value,
    const Range range
) {
    size_t lbits = countBits<size_t>() - range.len;

    return cutoff(value, lbits, 0) << range.start;
}

constexpr inline bool getBit(
    const size_t bits,
    const size_t i
) {
    assert(isValidBitIndex<size_t>(i));

    const size_t lbits = countBits<size_t>() - (i + 1);
    const size_t rbits = i;

    return cutoff(bits, lbits, rbits);
}

constexpr inline size_t getBits(
    const size_t bits,
    const Range range
) {
    assert(isValidBitRange<size_t>(range));

    const size_t lbits = countBits<size_t>() - (range.start + range.len);
    const size_t rbits = range.start;

    return cutoff(bits, lbits, rbits);
}

template<typename T>
inline void setBit(
    T& bits,
    const size_t i
) {
    assert(isValidBitIndex<T>(i));

    bits |= bitMask(i);
}

template<typename T>
inline void unsetBit(
    T& bits,
    const size_t i
) {
    assert(isValidBitIndex<T>(i));

    bits &= ~bitMask(i);
}

template<typename T>
inline void setBitTo(
    T& bits,
    const size_t i,
    const bool value
) {
    if(value) {
        setBit(bits, i);
    } else {
        unsetBit(bits, i);
    }
}

template<typename T>
inline void setBitRange(
    T& bits,
    const Range range
) {
    assert(isValidBitRange<T>(range));

    bits |= rangeMask(range);
}

template<typename T>
inline void unsetBitRange(
    T& bits,
    const Range range
) {
    assert(isValidBitRange<T>(range));

    bits &= ~rangeMask(range);
}

template<typename T>
inline void putBits(
    T& bits,
    const size_t put,
    const size_t offset
) {
    assert(countBits<T>() >= offset);

    bits = truncate(bits, offset);
    bits |= lShiftExactOverflow(put, offset);
}

template<typename T>
inline void setBitRangeTo(
    T& bits,
    const size_t to,
    const Range range
) {
    assert(isValidBitRange<T>(range));

    unsetBitRange(bits, range);
    bits |= valueMask(to, range);
}

template<std::integral T>
struct BitRange {
    T& _bits;
    Range _range;

    T get(
        const size_t i
    ) {
        getBits(_bits, i);
    }

    void set(
        const size_t val
    ) {
        setBitRangeTo(_bits, val, _range);
    }
};

} // namespace Tesix
