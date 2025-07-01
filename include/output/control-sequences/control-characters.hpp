#pragma once

#include "util/range.hpp"
#include "util/bits.hpp"

#include <inttypes.h>
#include <stddef.h>

namespace Tesix {

consteval uint8_t decodeRowColumn(
    const size_t row,
    const size_t column
) {
    return (unsetMask() | valueMask(column, Range::fromFor(0, 4))) | valueMask(row, Range::fromFor(4, 4));
}

constexpr uint8_t DECSC = '7';
constexpr uint8_t DECRC = '8';
constexpr uint8_t DECSTBM = 'r';

constexpr uint8_t LF = decodeRowColumn(0, 10);

constexpr uint8_t ESC = decodeRowColumn(1, 11);

constexpr uint8_t ICH = decodeRowColumn(4, 0);
constexpr uint8_t CUU = decodeRowColumn(4, 1);
constexpr uint8_t CUD = decodeRowColumn(4, 2);
constexpr uint8_t CUF = decodeRowColumn(4, 3);
constexpr uint8_t CUB = decodeRowColumn(4, 4);
constexpr uint8_t CNL = decodeRowColumn(4, 5);
constexpr uint8_t CPL = decodeRowColumn(4, 6);
constexpr uint8_t CHA = decodeRowColumn(4, 7);
constexpr uint8_t CUP = decodeRowColumn(4, 8);
constexpr uint8_t ED = decodeRowColumn(4, 10);
constexpr uint8_t EL = decodeRowColumn(4, 11);
constexpr uint8_t DL = decodeRowColumn(4, 13);
constexpr uint8_t IL = decodeRowColumn(4, 14);

constexpr uint8_t DCH = decodeRowColumn(5, 0);
constexpr uint8_t ECH = decodeRowColumn(5, 8);

constexpr uint8_t REP = decodeRowColumn(6, 2);
constexpr uint8_t RIS = decodeRowColumn(6, 3);
constexpr uint8_t VPA = decodeRowColumn(6, 4);
constexpr uint8_t SGR = decodeRowColumn(6, 13);

constexpr uint8_t NEL = decodeRowColumn(8, 5);
constexpr uint8_t HTS = decodeRowColumn(8, 8);
constexpr uint8_t RI = decodeRowColumn(8, 13);

constexpr uint8_t CSI = decodeRowColumn(9, 11);
constexpr uint8_t OSC = decodeRowColumn(9, 13);

// G0 (
// G1 )

// Keypad



}


