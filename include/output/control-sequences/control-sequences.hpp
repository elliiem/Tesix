#pragma once

#include "util/color.hpp"
#include "util/space.hpp"

#include <stdint.h>

namespace Tesix {

namespace Ctrl {

enum class ControlSequenceE {
    Linefeed,
    ReverseLinefeed,
    Newline,
    CursorUp,
    CursorDown,
    CursorForwards,
    CursorBackwards,
    CursorPrecedingLine,
    CursorNextLine,
    CursorLineAbsolute,
    CursorCharacterAbsolute,
    CursorPositionAbsolute,
    SaveCursor,
    RestoreCursor,
    EraseCharacters,
    EraseLineForwards,
    EraseLineBackwards,
    EraseLine,
    EraseDisplayForwards,
    EraseDisplayBackwards,
    EraseDisplay,
    DeleteCharacters,
    DeleteLines,
    InsertCharacters,
    InsertLines,
    Repeat,
    BoldOn,
    BoldOff,
    ItalicOn,
    ItalicOff,
    UnderlinedOn,
    UnderlinedOff,
    BlinkingOn,
    BlinkingOff,
    ReverseOn,
    ReverseOff,
    StrikethroughOn,
    StrikethroughOff,
    ColorForeground,
    ColorBackground,
    ColorForegroundFull,
    ColorBackgroundFull,
    ResetStyle,
    SetPaletteColor,
    ResetPalette,
};

union ControlSequenceU {
    uintmax_t CursorUp;
    uintmax_t CursorDown;
    uintmax_t CursorForwards;
    uintmax_t CursorBackwards;
    uintmax_t CursorPreceedingLine;
    uintmax_t CursorNextLine;
    uintmax_t CursorLineAbsolute;
    uintmax_t CursorCharacterAbsolute;
    Position CursorPositionAbsolute;
    uintmax_t EraseCharacters;
    uintmax_t DeleteCharacters;
    uintmax_t DeleteLines;
    uintmax_t InsertCharacters;
    uintmax_t InsertLines;
    uintmax_t Repeat;
    PaletteColor SetPaletteColor;
    uint8_t ColorForeground;
    uint8_t ColorBackground;
    Color24 ColorForegroundFull;
    Color24 ColorBackgroundFull;
};

struct ControlSequence {
    ControlSequenceE _type;
    ControlSequenceU _value;
};

}

}
