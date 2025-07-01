#pragma once

#include "output/control-sequences/control-characters.hpp"
#include "output/control-sequences/count.hpp"
#include "output/control-sequences/write.hpp"
#include "output/stream.hpp"

#include "util/array.hpp"
#include "util/color.hpp"
#include "util/space.hpp"
#include "util/string.hpp"

#include <stdint.h>

namespace Tesix {

namespace Out {

namespace Ctrl {

static inline void streamCSI(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t csi[] = {ESC, '['};

    streamBytes(out_buf, csi, countArrayC(csi), fd);
}

static inline void streamParameterSeparator(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    return streamByte(out_buf, ';', fd);
}

static inline void streamLinefeed(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, LF};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static inline void streamReverseLinefeed(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, RI};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static inline void streamNewline(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    streamByte(out_buf, NEL, fd);
}

static void streamCursorUp(
    Array<uint8_t>& out_buf,
    const uintmax_t n,
    const uintmax_t fd
) {
    if(countOneParameterEsc(n) <= out_buf.remaining()) {
        appendCursorUp(out_buf, n);
        return;
    }

    {
        streamCSI(out_buf, fd);
        streamUInt(out_buf, n, fd);
        streamByte(out_buf, CUU, fd);
    }
}

static void streamCursorDown(
    Array<uint8_t>& out_buf,
    const uintmax_t n,
    const uintmax_t fd
) {
    if(countOneParameterEsc(n) <= out_buf.remaining()) {
        appendCursorDown(out_buf, n);
        return;
    }

    {
        streamCSI(out_buf, fd);
        streamUInt(out_buf, n, fd);
        streamByte(out_buf, CUD, fd);
    }
}

static void streamCursorForwards(
    Array<uint8_t>& out_buf,
    const uintmax_t n,
    const uintmax_t fd
) {
    if(countOneParameterEsc(n) <= out_buf.remaining()) {
        appendCursorForwards(out_buf, n);
        return;
    }

    {
        streamCSI(out_buf, fd);
        streamUInt(out_buf, n, fd);
        streamByte(out_buf, CUF, fd);
    }
}

static void streamCursorBackwards(
    Array<uint8_t>& out_buf,
    const uintmax_t n,
    const uintmax_t fd
) {
    if(countOneParameterEsc(n) <= out_buf.remaining()) {
        appendCursorBackwards(out_buf, n);
        return;
    }

    {
        streamCSI(out_buf, fd);
        streamUInt(out_buf, n, fd);
        streamByte(out_buf, CUB, fd);
    }
}

static void streamCursorPrecedingLine(
    Array<uint8_t>& out_buf,
    const uintmax_t n,
    const uintmax_t fd
) {
    if(countOneParameterEsc(n) <= out_buf.remaining()) {
        appendCursorPrecedingLine(out_buf, n);
        return;
    }

    {
        streamCSI(out_buf, fd);
        streamUInt(out_buf, n, fd);
        streamByte(out_buf, CPL, fd);
    }
}

static void streamCursorNextLine(
    Array<uint8_t>& out_buf,
    const uintmax_t n,
    const uintmax_t fd
) {
    if(countOneParameterEsc(n) <= out_buf.remaining()) {
        appendCursorNextLine(out_buf, n);
        return;
    }

    {
        streamCSI(out_buf, fd);
        streamUInt(out_buf, n, fd);
        streamByte(out_buf, CNL, fd);
    }
}

static void streamCursorLineAbsolute(
    Array<uint8_t>& out_buf,
    const uintmax_t line,
    const uintmax_t fd
) {
    if(countOneParameterEsc(line) <= out_buf.remaining()) {
        appendCursorLineAbsolute(out_buf, line);
        return;
    }

    {
        streamCSI(out_buf, fd);
        streamUInt(out_buf, line, fd);
        streamByte(out_buf, VPA, fd);
    }
}

static void streamCursorCharacterAbsolute(
    Array<uint8_t>& out_buf,
    const uintmax_t ch,
    const uintmax_t fd
) {
    if(countOneParameterEsc(ch) <= out_buf.remaining()) {
        appendCursorCharacterAbsolute(out_buf, ch);
        return;
    }

    {
        streamCSI(out_buf, fd);
        streamUInt(out_buf, ch, fd);
        streamByte(out_buf, CHA, fd);
    }
}


static void streamCursorPositionAbsolute(
    Array<uint8_t>& out_buf,
    const Position& pos,
    const uintmax_t fd
) {
    if(countTwoParameterEsc(pos._x, pos._y) <= out_buf.remaining()) {
        appendCursorPositionAbsolute(out_buf, pos);
        return;
    }

    {
        streamCSI(out_buf, fd);

        streamUInt(out_buf, pos._y, fd);
        streamParameterSeparator(out_buf, fd);
        streamUInt(out_buf, pos._x, fd);

        streamByte(out_buf, CUP, fd);
    }
}

static inline void streamSaveCursor(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, '[', 's'};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static inline void streamRestoreCursor(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, '[', 'u'};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static void streamEraseCharacters(
    Array<uint8_t>& out_buf,
    const uintmax_t n,
    const uintmax_t fd
) {
    if(countOneParameterEsc(n) <= out_buf.remaining()) {
        appendEraseCharacters(out_buf, n);
        return;
    }

    {
        streamCSI(out_buf, fd);
        streamUInt(out_buf, n, fd);
        streamByte(out_buf, ECH, fd);
    }
}

static void streamEraseLineForwards(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, '[', EL};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static void streamEraseLineBackwards(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '1', EL};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static void streamEraseLine(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '2', EL};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static void streamEraseDisplayForwards(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, '[', ED};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static void streamEraseDisplayBackwards(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '1', ED};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static void streamEraseDisplay(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '2', ED};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static void streamDeleteCharacters(
    Array<uint8_t>& out_buf,
    const uintmax_t n,
    const uintmax_t fd
) {
    if(countOneParameterEsc(n) <= out_buf.remaining()) {
        appendDeleteCharacters(out_buf, n);
        return;
    }

    {
        streamCSI(out_buf, fd);
        streamUInt(out_buf, n, fd);
        streamByte(out_buf, DCH, fd);
    }
}

static void streamDeleteLines(
    Array<uint8_t>& out_buf,
    const uintmax_t n,
    const uintmax_t fd
) {
    if(countOneParameterEsc(n) <= out_buf.remaining()) {
        appendDeleteLines(out_buf, n);
        return;
    }

    {
        streamCSI(out_buf, fd);
        streamUInt(out_buf, n, fd);
        streamByte(out_buf, DL, fd);
    }
}

static void streamInsertCharacters(
    Array<uint8_t>& out_buf,
    const uintmax_t n,
    const uintmax_t fd
) {
    if(countOneParameterEsc(n) <= out_buf.remaining()) {
        appendInsertCharacters(out_buf, n);
        return;
    }

    {
        streamCSI(out_buf, fd);
        streamUInt(out_buf, n, fd);
        streamByte(out_buf, ICH, fd);
    }
}

static void streamInsertLines(
    Array<uint8_t>& out_buf,
    const uintmax_t n,
    const uintmax_t fd
) {
    if(countOneParameterEsc(n) <= out_buf.remaining()) {
        appendInsertLines(out_buf, n);
        return;
    }

    {
        streamCSI(out_buf, fd);
        streamUInt(out_buf, n, fd);
        streamByte(out_buf, IL, fd);
    }
}

static void streamRepeat(
    Array<uint8_t>& out_buf,
    const uintmax_t n,
    const uintmax_t fd
) {
    if(countOneParameterEsc(n) <= out_buf.remaining()) {
        appendRepeat(out_buf, n);
        return;
    }

    {
        streamCSI(out_buf, fd);
        streamUInt(out_buf, n, fd);
        streamByte(out_buf, REP, fd);
    }
}

static void streamBoldOn(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '1', SGR};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static void streamBoldOff(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '2', '2', SGR};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static void streamItalicOn(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '3', SGR};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static void streamItalicOff(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '2', '3', SGR};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static void streamUnderlinedOn(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '4', SGR};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static void streamUnderlinedOff(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '2', '4', SGR};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static void streamBlinkingOn(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '5', SGR};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static void streamBlinkingOff(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '2', '5', SGR};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static void streamReverseOn(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '7', SGR};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static void streamReverseOff(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '2', '7', SGR};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static void streamStrikethroughOn(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '9', SGR};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static void streamStrikethroughOff(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '2', '9', SGR};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static void streamColorForeground(
    Array<uint8_t>& out_buf,
    const uintmax_t n,
    const uintmax_t fd
) {
    assert(n < 16);

    if(countColorForeground() <= out_buf.remaining()) {
        appendColorForeground(out_buf, n);
        return;
    }

    constexpr uint8_t style_op_lower[] = {ESC, '[', '3'};
    constexpr uint8_t style_op_upper[] = {ESC, '[', '9'};

    {
        if(n < 8) {
            streamBytes(out_buf, style_op_lower, countArrayC(style_op_lower), fd);

            streamByte(out_buf, '0' + n, fd);
        } else {
            streamBytes(out_buf, style_op_upper, countArrayC(style_op_upper), fd);

            streamByte(out_buf, '0' + (n - 8), fd);
        }

        streamByte(out_buf, SGR, fd);
    }
}

static void streamColorBackground(
    Array<uint8_t>& out_buf,
    const uintmax_t n,
    const uintmax_t fd
) {
    assert(n < 16);

    if(countColorBackground(n) <= out_buf.remaining()) {
        appendColorBackground(out_buf, n);
        return;
    }

    constexpr uint8_t style_op_lower[] = {ESC, '[', '4'};
    constexpr uint8_t style_op_upper[] = {ESC, '[', '1', '0'};

    {
        if(n <= 8) {
            streamBytes(out_buf, style_op_lower, countArrayC(style_op_lower), fd);

            streamByte(out_buf, '0' + n, fd);
        } else {
            streamBytes(out_buf, style_op_upper, countArrayC(style_op_upper), fd);

            streamByte(out_buf, '0' + (n - 8), fd);
        }

        streamByte(out_buf, SGR, fd);
    }
}

static void streamColorForegroundFull(
    Array<uint8_t>& out_buf,
    const Color24& color,
    const uintmax_t fd
) {
    if(countFullColor(color) <= out_buf.remaining()) {
        appendColorForegroundFull(out_buf, color);
        return;
    }

    constexpr uint8_t style_op[] = {ESC, '[', '3', '8', ';', '2', ';'};

    {
        streamBytes(out_buf, style_op, countArrayC(style_op), fd);

        streamUInt(out_buf, color._r, fd);
        streamParameterSeparator(out_buf, fd);
        streamUInt(out_buf, color._g, fd);
        streamParameterSeparator(out_buf, fd);
        streamUInt(out_buf, color._b, fd);

        streamByte(out_buf, SGR, fd);
    }
}

static void streamColorBackgroundFull(
    Array<uint8_t>& out_buf,
    const Color24& color,
    const uintmax_t fd
) {
    if(countFullColor(color) <= out_buf.remaining()) {
        appendColorBackgroundFull(out_buf, color);
        return;
    }

    constexpr uint8_t style_op[] = {ESC, '[', '4', '8', ';', '2', ';'};

    {
        streamBytes(out_buf, style_op, countArrayC(style_op), fd);

        streamUInt(out_buf, color._r, fd);
        streamParameterSeparator(out_buf, fd);
        streamUInt(out_buf, color._g, fd);
        streamParameterSeparator(out_buf, fd);
        streamUInt(out_buf, color._b, fd);

        streamByte(out_buf, SGR, fd);
    }
}

static void streamResetStyle(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '0', SGR};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }

}

static void streamSetPaletteColor(
    Array<uint8_t>& out_buf,
    const PaletteColor& color,
    const uintmax_t fd
) {
    uint8_t ctrl[] = {OSC, 'P', 0, 0, 0, 0, 0, 0, 0};

    {
        sprintf(asCStr(ctrl), "%x01\n", color._n);
        sprintf(asCStr(ctrl + 1), "%x02\n", color._r);
        sprintf(asCStr(ctrl + 3), "%x02\n", color._g);
        sprintf(asCStr(ctrl + 5), "%x02\n", color._b);
    }

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

static void streamResetPalette(
    Array<uint8_t>& out_buf,
    const uintmax_t fd
) {
    constexpr uint8_t ctrl[] = {OSC, 'R'};

    {
        streamBytes(out_buf, ctrl, countArrayC(ctrl), fd);
    }
}

}

}

}
