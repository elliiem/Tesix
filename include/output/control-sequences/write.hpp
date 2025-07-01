#pragma once

#include "output/control-sequences/control-characters.hpp"
#include "output/number.hpp"

#include "util/array.hpp"
#include "util/space.hpp"
#include "util/string.hpp"
#include "util/color.hpp"

#include <cstdint>
#include <stdint.h>
#include <sys/types.h>

namespace Tesix {

namespace Out {

namespace Ctrl {

static void appendCSI(
    Array<uint8_t>& dest
) {
    constexpr uint8_t csi[] = {ESC, '['};

    {
        dest.appendMulti(csi, countArrayC(csi));
    }
}

static void appendParameterSeparator(
    Array<uint8_t>& dest
) {
    dest.append(';');
}

static void appendLinefeed(
    Array<uint8_t>& dest
) {
    constexpr uint8_t esc[] = {ESC, LF};

    {
        dest.appendMulti(esc, countArrayC(esc));
    }
}

static void appendReverseLinefeed(
    Array<uint8_t>& dest
) {
    constexpr uint8_t esc[] = {ESC, RI};

    {
        return dest.appendMulti(esc, countArrayC(esc));
    }
}

static void appendNewline(
    Array<uint8_t>& dest
) {
    return dest.append(NEL);
}

static void appendCursorUp(
    Array<uint8_t>& dest,
    const uintmax_t n
) {
    appendCSI(dest);
    appendUInt(dest, n);
    dest.append(CUU);
}

static void appendCursorDown(
    Array<uint8_t>& dest,
    const uintmax_t n
) {
    appendCSI(dest);
    appendUInt(dest, n);
    dest.append(CUD);
}

static void appendCursorForwards(
    Array<uint8_t>& dest,
    const uintmax_t n
) {
    appendCSI(dest);
    appendUInt(dest, n);
    dest.append(CUF);
}

static void appendCursorBackwards(
    Array<uint8_t>& dest,
    const uintmax_t n
) {
    appendCSI(dest);
    appendUInt(dest, n);
    dest.append(CUB);
}

static void appendCursorPrecedingLine(
    Array<uint8_t>& dest,
    const uintmax_t n
) {
    appendCSI(dest);
    appendUInt(dest, n);
    dest.append(CPL);
}

static void appendCursorNextLine(
    Array<uint8_t>& dest,
    const uintmax_t n
) {
    appendCSI(dest);
    appendUInt(dest, n);
    dest.append(CNL);
}

static void appendCursorLineAbsolute(
    Array<uint8_t>& dest,
    const uintmax_t line
) {
    appendCSI(dest);
    appendUInt(dest, line);
    dest.append(VPA);
}

static void appendCursorCharacterAbsolute(
    Array<uint8_t>& dest,
    const uintmax_t ch
) {
    appendCSI(dest);
    appendUInt(dest, ch);
    dest.append(CHA);
}

static void appendCursorPositionAbsolute(
    Array<uint8_t>& dest,
    const Position& pos
) {
    appendCSI(dest);

    appendUInt(dest, pos._y);
    appendParameterSeparator(dest);
    appendUInt(dest, pos._x);

    dest.append(CUP);
}

static void appendSaveCursor(
    Array<uint8_t>& dest
) {
    constexpr uint8_t ctrl[] = {ESC, '[', 's'};

    {
        dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}

static void appendRestoreCursor(
    Array<uint8_t>& dest
) {
    constexpr uint8_t esc[] = {ESC, '[', 'u'};

    {
        dest.appendMulti(esc, countArrayC(esc));
    }
}

static void appendEraseCharacters(
    Array<uint8_t>& dest,
    const uintmax_t n
) {
    appendCSI(dest);
    appendUInt(dest, n);
    dest.append(ECH);
}

static void appendEraseLineForwards(
    Array<uint8_t>& dest
) {
    constexpr uint8_t ctrl[] = {ESC, '[', EL};

    {
        return dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}

static void appendEraseLineBackwards(
    Array<uint8_t>& dest
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '1', EL};

    {
        return dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}

static void appendEraseLine(
    Array<uint8_t>& dest
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '2', EL};

    {
        return dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}

static void appendEraseDisplayForwards(
    Array<uint8_t>& dest
) {
    constexpr uint8_t ctrl[] = {ESC, '[', ED};

    {
        return dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}

static void appendEraseDisplayBackwards(
    Array<uint8_t>& dest
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '1', ED};

    {
        return dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}

static void appendEraseDisplay(
    Array<uint8_t>& dest
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '2', ED};

    {
        return dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}

static void appendDeleteCharacters(
    Array<uint8_t>& dest,
    const uintmax_t n
) {
    appendCSI(dest);
    appendUInt(dest, n);
    dest.append(DL);
}

static void appendDeleteLines(
    Array<uint8_t>& dest,
    const uintmax_t n
) {
    appendCSI(dest);
    appendUInt(dest, n);
    dest.append(DL);
}

static void appendInsertCharacters(
    Array<uint8_t>& dest,
    const uintmax_t n
) {
    appendCSI(dest);
    appendUInt(dest, n);
    dest.append(ICH);
}

static void appendInsertLines(
    Array<uint8_t>& dest,
    const uintmax_t n
) {
    appendCSI(dest);
    appendUInt(dest, n);
    dest.append(IL);
}

static void appendRepeat(
    Array<uint8_t>& dest,
    const uintmax_t n
) {
    appendCSI(dest);
    appendUInt(dest, n);
    dest.append(REP);
}

static void appendBoldOn(
    Array<uint8_t>& dest
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '1', SGR};

    {
        dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}

static void appendBoldOff(
    Array<uint8_t>& dest
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '2', '2', SGR};

    {
        dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}

static void appendItalicOn(
    Array<uint8_t>& dest
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '3', SGR};

    {
        dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}

static void appendItalicOff(
    Array<uint8_t>& dest
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '2', '3', SGR};

    {
        dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}

static void appendUnderlinedOn(
    Array<uint8_t>& dest
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '4', SGR};

    {
        dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}

static void appendUnderlinedOff(
    Array<uint8_t>& dest
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '2', '4', SGR};

    {
        dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}

static void appendBlinkingOn(
    Array<uint8_t>& dest
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '5', SGR};

    {
        dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}

static void appendBlinkingOff(
    Array<uint8_t>& dest
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '2', '5', SGR};

    {
        dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}

static void appendReverseOn(
    Array<uint8_t>& dest
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '7', SGR};

    {
        dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}

static void appendReverseOff(
    Array<uint8_t>& dest
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '2', '7', SGR};

    {
        dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}

static void appendStrikethroughOn(
    Array<uint8_t>& dest
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '9', SGR};

    {
        dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}

static void appendStrikethroughOff(
    Array<uint8_t>& dest
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '2', '9', SGR};

    {
        dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}


static void appendColorForeground(
    Array<uint8_t>& dest,
    const uint8_t n
) {
    assert(n < 16);

    constexpr uint8_t style_op_lower[] = {ESC, '[', '3'};
    constexpr uint8_t style_op_upper[] = {ESC, '[', '9'};

    {
        if(n < 8 ) {
            dest.appendMulti(style_op_lower, countArrayC(style_op_lower));

            dest.append('0' + n);
        } else {
            dest.appendMulti(style_op_upper, countArrayC(style_op_upper));

            dest.append('0' + (n - 8));
        }

        dest.append(SGR);
    }
}

static void appendColorBackground(
    Array<uint8_t>& dest,
    const uint8_t n
) {
    assert(n < 16);

    constexpr uint8_t style_op_lower[] = {ESC, '[', '4'};
    constexpr uint8_t style_op_upper[] = {ESC, '[', '1', '0'};

    {
        if(n < 8 ) {
            dest.appendMulti(style_op_lower, countArrayC(style_op_lower));

            dest.append('0' + n);
        } else {
            dest.appendMulti(style_op_upper, countArrayC(style_op_upper));

            dest.append('0' + (n - 8));
        }

        dest.append(SGR);
    }
}

static void appendColorForegroundFull(
    Array<uint8_t>& dest,
    const Color24& color
) {
    constexpr uint8_t style_op[] = {ESC, '[', '3', '8', ';', '2', ';'};

    {
        dest.appendMulti(style_op, countArrayC(style_op));

        appendUInt(dest, color._r);
        appendParameterSeparator(dest);
        appendUInt(dest, color._g);
        appendParameterSeparator(dest);
        appendUInt(dest, color._b);

        dest.append(SGR);
    }
}

static void appendColorBackgroundFull(
    Array<uint8_t>& dest,
    const Color24& color
) {
    constexpr uint8_t style_op[] = {ESC, '[', '4', '8', ';', '2', ';'};

    {
        dest.appendMulti(style_op, countArrayC(style_op));

        appendUInt(dest, color._r);
        appendParameterSeparator(dest);
        appendUInt(dest, color._g);
        appendParameterSeparator(dest);
        appendUInt(dest, color._b);

        dest.append(SGR);
    }
}

static void appendResetStyle(
    Array<uint8_t>& dest
) {
    constexpr uint8_t ctrl[] = {ESC, '[', '0', SGR};

    {
        dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}

static void appendSetPaletteColor(
    Array<uint8_t>& dest,
    const PaletteColor& color
) {
    uint8_t ctrl[] = {OSC, 'P', 0, 0, 0, 0, 0, 0, 0};

    {
        sprintf(asCStr(ctrl), "%x01\n", color._n);
        sprintf(asCStr(ctrl + 1), "%x02\n", color._r);
        sprintf(asCStr(ctrl + 3), "%x02\n", color._g);
        sprintf(asCStr(ctrl + 5), "%x02\n", color._b);
    }

    {
        dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}

static void appendResetPalette(
    Array<uint8_t>& dest
) {
    constexpr uint8_t ctrl[] = {OSC, 'R'};

    {
        dest.appendMulti(ctrl, countArrayC(ctrl));
    }
}

}

}

}
