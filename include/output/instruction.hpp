#pragma once

#include "output/control-sequences/stream.hpp"
#include "output/control-sequences/write.hpp"

#include "util/array.hpp"
#include "util/space.hpp"
#include "util/color.hpp"
#include "util/utf.hpp"

#include <cstdint>
#include <stdint.h>

namespace Tesix {

namespace Out {

enum class InstructionE {
    Character,
    String,
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

union InstructionU {
    uint32_t Character;
    Array<uint32_t> String;
    uintmax_t CursorUp;
    uintmax_t CursorDown;
    uintmax_t CursorForwards;
    uintmax_t CursorBackwards;
    uintmax_t CursorPrecedingLine;
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
    uint8_t ColorForeground;
    uint8_t ColorBackground;
    Color24 ColorForegroundFull;
    Color24 ColorBackgroundFull;
    PaletteColor SetPaletteColor;
};

struct Instruction {
    InstructionE _type;
    InstructionU _value;

    void operator=(
        const Instruction& other
    ) {
        _type = other._type;
        _value = other._value;
    }

    static inline Instruction createCharacter(
        const uint32_t ch
    ) {
        return {._type = InstructionE::Character, ._value = {.Character = ch}};
    }

    static inline Instruction createString(
        const Array<uint32_t>& str
    ) {
        return {._type = InstructionE::String, ._value = {.String = str}};
    }

    static consteval Instruction createLinefeed() {
        return {._type = InstructionE::Linefeed};
    }

    static consteval Instruction createReverseLinefeed() {
        return {._type = InstructionE::ReverseLinefeed};
    }

    static consteval Instruction createNewline() {
        return {._type = InstructionE::Newline};
    }

    static inline Instruction createCursorUp(
        const uintmax_t n
    ) {
        return {._type = InstructionE::CursorUp, ._value {.CursorUp = n}};
    }

    static inline Instruction createCursorDown(
        const uintmax_t n
    ) {
        return {._type = InstructionE::CursorDown, ._value {.CursorDown = n}};
    }

    static inline Instruction createCursorForwards(
        const uintmax_t n
    ) {
        return {._type = InstructionE::CursorForwards, ._value {.CursorForwards = n}};
    }

    static inline Instruction createCursorBackwards(
        const uintmax_t n
    ) {
        return {._type = InstructionE::CursorBackwards, ._value {.CursorBackwards = n}};
    }

    static inline Instruction createCursorPrecedingLine(
        const uintmax_t n
    ) {
        return {._type = InstructionE::CursorPrecedingLine, ._value {.CursorPrecedingLine = n}};
    }

    static inline Instruction createCursorNextLine(
        const uintmax_t n
    ) {
        return {._type = InstructionE::CursorNextLine, ._value {.CursorNextLine = n}};
    }

    static inline Instruction createCursorLineAbsolute(
        const uintmax_t y
    ) {
        return {._type = InstructionE::CursorLineAbsolute, ._value = {.CursorLineAbsolute = y}};
    }

    static inline Instruction createCursorCharacterAbsolute(
        const uintmax_t x
    ) {
        return {._type = InstructionE::CursorCharacterAbsolute, ._value = {.CursorCharacterAbsolute = x}};
    }

    static inline Instruction createCursorPositionAbsolute(
        const Position pos
    ) {
        return {._type = InstructionE::CursorPositionAbsolute, ._value = {.CursorPositionAbsolute = pos}};
    }

    static consteval Instruction createSaveCursor() {
        return {._type = InstructionE::SaveCursor};
    }

    static consteval Instruction createRestoreCursor() {
        return {._type = InstructionE::RestoreCursor};
    }

    static inline Instruction createEraseCharacters(
        const uintmax_t n
    ) {
        return {._type = InstructionE::EraseCharacters, ._value {.EraseCharacters = n}};
    }

    static consteval Instruction createEraseLineForwards() {
        return {._type = InstructionE::EraseLineForwards};
    }

    static consteval Instruction createEraseLineBackwards() {
        return {._type = InstructionE::EraseLineBackwards};
    }

    static consteval Instruction createEraseLine() {
        return {._type = InstructionE::EraseLine};
    }

    static consteval Instruction createEraseDisplayForwards() {
        return {._type = InstructionE::EraseDisplayForwards};
    }

    static consteval Instruction createEraseDisplayBackwards() {
        return {._type = InstructionE::EraseDisplayBackwards};
    }

    static consteval Instruction createEraseDisplay() {
        return {._type = InstructionE::EraseDisplay};
    }

    static inline Instruction createDeleteCharacters(
        const uintmax_t n
    ) {
        return {._type = InstructionE::DeleteCharacters, ._value {.DeleteCharacters = n}};
    }

    static inline Instruction createDeleteLines(
        const uintmax_t n
    ) {
        return {._type = InstructionE::DeleteLines, ._value {.DeleteLines = n}};
    }

    static inline Instruction createInsertCharacters(
        const uintmax_t n
    ) {
        return {._type = InstructionE::InsertCharacters, ._value {.InsertCharacters = n}};
    }

    static inline Instruction createInsertLines(
        const uintmax_t n
    ) {
        return {._type = InstructionE::InsertLines, ._value {.InsertLines = n}};
    }

    static inline Instruction createRepeat(
        const uintmax_t n
    ) {
        return {._type = InstructionE::Repeat, ._value {.Repeat = n}};
    }

    static consteval Instruction createBoldOn() {
        return {._type = InstructionE::BoldOn};
    }

    static consteval Instruction createBoldOff() {
        return {._type = InstructionE::BoldOff};
    }

    static consteval Instruction createItalicOn() {
        return {._type = InstructionE::ItalicOn};
    }

    static consteval Instruction createItalicOff() {
        return {._type = InstructionE::ItalicOff};
    }

    static consteval Instruction createUnderlinedOn() {
        return {._type = InstructionE::UnderlinedOn};
    }

    static consteval Instruction createUnderlinedOff() {
        return {._type = InstructionE::UnderlinedOff};
    }

    static consteval Instruction createBlinkingOn() {
        return {._type = InstructionE::BlinkingOn};
    }

    static consteval Instruction createBlinkingOff() {
        return {._type = InstructionE::BlinkingOff};
    }

    static consteval Instruction createReverseOn() {
        return {._type = InstructionE::ReverseOn};
    }

    static consteval Instruction createReverseOff() {
        return {._type = InstructionE::ReverseOff};
    }

    static consteval Instruction createStrikethroughOn() {
        return {._type = InstructionE::StrikethroughOn};
    }

    static consteval Instruction createStrikethroughOff() {
        return {._type = InstructionE::StrikethroughOff};
    }

    static inline Instruction createColorForeground(
        const uint8_t n
    ) {
        return {._type = InstructionE::ColorForeground, ._value = {.ColorForeground = n}};
    }

    static inline Instruction createColorBackground(
        const uint8_t n
    ) {
        return {._type = InstructionE::ColorBackground, ._value = {.ColorBackground = n}};
    }

    static inline Instruction createColorForegroundFull(
        const Color24& color
    ) {
        return {._type = InstructionE::ColorForegroundFull, ._value = {.ColorForegroundFull = color}};
    }

    static inline Instruction createColorBackgroundFull(
        const Color24& color
    ) {
        return {._type = InstructionE::ColorBackgroundFull, ._value = {.ColorBackgroundFull = color}};
    }

    static consteval Instruction createResetStyle() {
        return {._type = InstructionE::ResetStyle};
    }

    static inline Instruction createSetPaletteColor(
        const PaletteColor& color
    ) {
        return {._type = InstructionE::SetPaletteColor, ._value = {.SetPaletteColor = color}};
    }

    static consteval Instruction createResetPalette() {
        return {._type = InstructionE::ResetPalette};
    }
};

static void streamControlSequence(
    Array<uint8_t>& out_buf,
    const Instruction& instr,
    const uintmax_t fd
) {
    switch(instr._type) {
        case InstructionE::Character: {
            uint8_t utf8[4];

            const uintmax_t octet_c = UTF32::toUTF8Single(utf8, instr._value.Character);

            streamBytes(out_buf, utf8, octet_c, fd);
        } break;
        case InstructionE::String: {
            for(uintmax_t i = 0; i < instr._value.String._n; i++) {
                uint8_t utf8[4];

                const uintmax_t octet_c = UTF32::toUTF8Single(utf8, instr._value.String._ptr[i]);

                streamBytes(out_buf, utf8, octet_c, fd);
            }
        } break;
        case InstructionE::Linefeed: {
            Ctrl::streamLinefeed(out_buf, fd);
        } break;
        case InstructionE::ReverseLinefeed: {
            Ctrl::streamReverseLinefeed(out_buf, fd);
        } break;
        case InstructionE::Newline: {
            Ctrl::streamNewline(out_buf, fd);
        } break;
        case InstructionE::CursorUp: {
            Ctrl::streamCursorUp(out_buf, instr._value.CursorUp, fd);
        } break;
        case InstructionE::CursorDown: {
            Ctrl::streamCursorDown(out_buf, instr._value.CursorDown, fd);
        } break;
        case InstructionE::CursorForwards: {
            Ctrl::streamCursorForwards(out_buf, instr._value.CursorForwards, fd);
        } break;
        case InstructionE::CursorBackwards: {
            Ctrl::streamCursorBackwards(out_buf, instr._value.CursorBackwards, fd);
        } break;
        case InstructionE::CursorPrecedingLine: {
            Ctrl::streamCursorPrecedingLine(out_buf, instr._value.CursorPrecedingLine, fd);
        } break;
        case InstructionE::CursorNextLine: {
            Ctrl::streamCursorNextLine(out_buf, instr._value.CursorNextLine, fd);
        } break;
        case InstructionE::CursorLineAbsolute: {
            Ctrl::streamCursorLineAbsolute(out_buf, instr._value.CursorLineAbsolute, fd);
        } break;
        case InstructionE::CursorCharacterAbsolute: {
            Ctrl::streamCursorCharacterAbsolute(out_buf, instr._value.CursorCharacterAbsolute, fd);
        } break;
        case InstructionE::CursorPositionAbsolute: {
            Ctrl::streamCursorPositionAbsolute(out_buf, instr._value.CursorPositionAbsolute, fd);
        } break;
        case InstructionE::SaveCursor: {
            Ctrl::streamSaveCursor(out_buf, fd);
        } break;
        case InstructionE::RestoreCursor: {
            Ctrl::streamRestoreCursor(out_buf, fd);
        } break;
        case InstructionE::EraseCharacters: {
            Ctrl::streamEraseCharacters(out_buf, instr._value.EraseCharacters, fd);
        } break;
        case InstructionE::EraseLineForwards: {
            Ctrl::streamEraseLineForwards(out_buf, fd);
        } break;
        case InstructionE::EraseLineBackwards: {
            Ctrl::streamEraseLineBackwards(out_buf, fd);
        } break;
        case InstructionE::EraseLine: {
            Ctrl::streamEraseLine(out_buf, fd);
        } break;
        case InstructionE::EraseDisplayForwards: {
            Ctrl::streamEraseDisplayForwards(out_buf, fd);
        } break;
        case InstructionE::EraseDisplayBackwards: {
            Ctrl::streamEraseDisplayBackwards(out_buf, fd);
        } break;
        case InstructionE::EraseDisplay: {
            Ctrl::streamEraseDisplay(out_buf, fd);
        } break;
        case InstructionE::DeleteCharacters: {
            Ctrl::streamDeleteCharacters(out_buf, instr._value.DeleteCharacters, fd);
        } break;
        case InstructionE::DeleteLines: {
            Ctrl::streamDeleteLines(out_buf, instr._value.DeleteLines, fd);
        } break;
        case InstructionE::InsertCharacters: {
            Ctrl::streamInsertCharacters(out_buf, instr._value.InsertCharacters, fd);
        } break;
        case InstructionE::InsertLines: {
            Ctrl::streamInsertLines(out_buf, instr._value.InsertLines, fd);
        } break;
        case InstructionE::Repeat: {
            Ctrl::streamRepeat(out_buf, instr._value.Repeat, fd);
        } break;
        case InstructionE::BoldOn: {
            Ctrl::streamBoldOn(out_buf, fd);
        } break;
        case InstructionE::BoldOff: {
            Ctrl::streamBoldOff(out_buf, fd);
        } break;
        case InstructionE::ItalicOn: {
            Ctrl::streamItalicOn(out_buf, fd);
        } break;
        case InstructionE::ItalicOff: {
            Ctrl::streamItalicOff(out_buf, fd);
        } break;
        case InstructionE::UnderlinedOn: {
            Ctrl::streamUnderlinedOn(out_buf, fd);
        } break;
        case InstructionE::UnderlinedOff: {
            Ctrl::streamUnderlinedOff(out_buf, fd);
        } break;
        case InstructionE::BlinkingOn: {
            Ctrl::streamBlinkingOn(out_buf, fd);
        } break;
        case InstructionE::BlinkingOff: {
            Ctrl::streamBlinkingOff(out_buf, fd);
        } break;
        case InstructionE::ReverseOn: {
            Ctrl::streamReverseOn(out_buf, fd);
        } break;
        case InstructionE::ReverseOff: {
            Ctrl::streamReverseOff(out_buf, fd);
        } break;
        case InstructionE::StrikethroughOn: {
            Ctrl::streamStrikethroughOn(out_buf, fd);
        } break;
        case InstructionE::StrikethroughOff: {
            Ctrl::streamStrikethroughOff(out_buf, fd);
        } break;
        case InstructionE::ColorForeground: {
            Ctrl::streamColorForeground(out_buf, instr._value.ColorForeground, fd);
        } break;
        case InstructionE::ColorBackground: {
            Ctrl::streamColorBackground(out_buf, instr._value.ColorBackground, fd);
        } break;
        case InstructionE::ColorForegroundFull: {
            Ctrl::streamColorForegroundFull(out_buf, instr._value.ColorForegroundFull, fd);
        } break;
        case InstructionE::ColorBackgroundFull: {
            Ctrl::streamColorBackgroundFull(out_buf, instr._value.ColorBackgroundFull, fd);
        } break;
        case InstructionE::ResetStyle: {
            Ctrl::streamResetStyle(out_buf, fd);
        } break;
        case InstructionE::SetPaletteColor: {
            Ctrl::streamSetPaletteColor(out_buf, instr._value.SetPaletteColor, fd);
        } break;
        case InstructionE::ResetPalette: {
            Ctrl::streamResetPalette(out_buf, fd);
        } break;
    }
}

static void appendControlSequence(
    Array<uint8_t>& dest,
    const Instruction& instr
) {
    switch(instr._type) {
        case InstructionE::Character: {
            uint8_t utf8[4];

            const uintmax_t octet_c = UTF32::toUTF8Single(utf8, instr._value.Character);

            dest.appendMulti(utf8, octet_c);
        } break;
        case InstructionE::String: {
            for(uintmax_t i = 0; i < instr._value.String._n; i++) {
                uint8_t utf8[4];

                const uintmax_t octet_c = UTF32::toUTF8Single(utf8, instr._value.String._ptr[i]);

                dest.appendMulti(utf8, octet_c);
            }
        } break;
        case InstructionE::Linefeed: {
            Ctrl::appendLinefeed(dest);
        } break;
        case InstructionE::ReverseLinefeed: {
            Ctrl::appendReverseLinefeed(dest);
        } break;
        case InstructionE::Newline: {
            Ctrl::appendNewline(dest);
        } break;
        case InstructionE::CursorUp: {
            Ctrl::appendCursorUp(dest, instr._value.CursorUp);
        } break;
        case InstructionE::CursorDown: {
            Ctrl::appendCursorDown(dest, instr._value.CursorDown);
        } break;
        case InstructionE::CursorForwards: {
            Ctrl::appendCursorForwards(dest, instr._value.CursorForwards);
        } break;
        case InstructionE::CursorBackwards: {
            Ctrl::appendCursorBackwards(dest, instr._value.CursorBackwards);
        } break;
        case InstructionE::CursorPrecedingLine: {
            Ctrl::appendCursorPrecedingLine(dest, instr._value.CursorPrecedingLine);
        } break;
        case InstructionE::CursorNextLine: {
            Ctrl::appendCursorNextLine(dest, instr._value.CursorNextLine);
        } break;
        case InstructionE::CursorLineAbsolute: {
            Ctrl::appendCursorLineAbsolute(dest, instr._value.CursorLineAbsolute);
        } break;
        case InstructionE::CursorCharacterAbsolute: {
            Ctrl::appendCursorCharacterAbsolute(dest, instr._value.CursorCharacterAbsolute);
        } break;
        case InstructionE::CursorPositionAbsolute: {
            Ctrl::appendCursorPositionAbsolute(dest, instr._value.CursorPositionAbsolute);
        } break;
        case InstructionE::SaveCursor: {
            Ctrl::appendSaveCursor(dest);
        } break;
        case InstructionE::RestoreCursor: {
            Ctrl::appendRestoreCursor(dest);
        } break;
        case InstructionE::EraseCharacters: {
            Ctrl::appendEraseCharacters(dest, instr._value.EraseCharacters);
        } break;
        case InstructionE::EraseLineForwards: {
            Ctrl::appendEraseLineForwards(dest);
        } break;
        case InstructionE::EraseLineBackwards: {
            Ctrl::appendEraseLineBackwards(dest);
        } break;
        case InstructionE::EraseLine: {
            Ctrl::appendEraseLine(dest);
        } break;
        case InstructionE::EraseDisplayForwards: {
            Ctrl::appendEraseDisplayForwards(dest);
        } break;
        case InstructionE::EraseDisplayBackwards: {
            Ctrl::appendEraseDisplayBackwards(dest);
        } break;
        case InstructionE::EraseDisplay: {
            Ctrl::appendEraseDisplay(dest);
        } break;
        case InstructionE::DeleteCharacters: {
            Ctrl::appendDeleteCharacters(dest, instr._value.DeleteCharacters);
        } break;
        case InstructionE::DeleteLines: {
            Ctrl::appendDeleteLines(dest, instr._value.DeleteLines);
        } break;
        case InstructionE::InsertCharacters: {
            Ctrl::appendInsertCharacters(dest, instr._value.InsertCharacters);
        } break;
        case InstructionE::InsertLines: {
            Ctrl::appendInsertLines(dest, instr._value.InsertLines);
        } break;
        case InstructionE::Repeat: {
            Ctrl::appendRepeat(dest, instr._value.Repeat);
        } break;
        case InstructionE::BoldOn: {
            Ctrl::appendBoldOn(dest);
        } break;
        case InstructionE::BoldOff: {
            Ctrl::appendBoldOff(dest);
        } break;
        case InstructionE::ItalicOn: {
            Ctrl::appendItalicOn(dest);
        } break;
        case InstructionE::ItalicOff: {
            Ctrl::appendItalicOff(dest);
        } break;
        case InstructionE::UnderlinedOn: {
            Ctrl::appendUnderlinedOn(dest);
        } break;
        case InstructionE::UnderlinedOff: {
            Ctrl::appendUnderlinedOff(dest);
        } break;
        case InstructionE::BlinkingOn: {
            Ctrl::appendBlinkingOn(dest);
        } break;
        case InstructionE::BlinkingOff: {
            Ctrl::appendBlinkingOff(dest);
        } break;
        case InstructionE::ReverseOn: {
            Ctrl::appendReverseOn(dest);
        } break;
        case InstructionE::ReverseOff: {
            Ctrl::appendReverseOff(dest);
        } break;
        case InstructionE::StrikethroughOn: {
            Ctrl::appendStrikethroughOn(dest);
        } break;
        case InstructionE::StrikethroughOff: {
            Ctrl::appendStrikethroughOff(dest);
        } break;
        case InstructionE::ColorForeground: {
            Ctrl::appendColorForeground(dest, instr._value.ColorForeground);
        } break;
        case InstructionE::ColorBackground: {
            Ctrl::appendColorBackground(dest, instr._value.ColorBackground);
        } break;
        case InstructionE::ColorForegroundFull: {
            Ctrl::appendColorForegroundFull(dest, instr._value.ColorForegroundFull);
        } break;
        case InstructionE::ColorBackgroundFull: {
            Ctrl::appendColorBackgroundFull(dest, instr._value.ColorBackgroundFull);
        } break;
        case InstructionE::ResetStyle: {
            Ctrl::appendResetStyle(dest);
        } break;
        case InstructionE::SetPaletteColor: {
            Ctrl::appendSetPaletteColor(dest, instr._value.SetPaletteColor);
        } break;
        case InstructionE::ResetPalette: {
            Ctrl::appendResetPalette(dest);
        } break;
    }
}

static void emptyInstructionBuffer(
    Array<uint8_t>& out,
    Array<Out::Instruction>& instr_buf,
    const uintmax_t fd
) {
    for(uintmax_t i = 0; i < instr_buf._n; i++) {
        streamControlSequence(out, instr_buf._ptr[i], fd);
    }

    instr_buf._n = 0;
}

static void streamInstruction(
    Array<uint8_t>& out,
    Array<Out::Instruction>& instrs,
    const Out::Instruction& instr,
    const uintmax_t fd
) {
    if(instrs.remaining() >= 1) {
        instrs.append(instr);
        return;
    }

    emptyInstructionBuffer(out, instrs, fd);

    instrs.append(instr);
}

static void streamInstructions(
    Array<uint8_t>& out,
    Array<Out::Instruction>& instr_buf,
    const Out::Instruction* const src,
    const uintmax_t src_c,
    const uintmax_t fd
) {
    const uintmax_t instr_buf_rem = instr_buf.remaining();

    if(instr_buf_rem >= src_c) {
        instr_buf.appendMulti(src, src_c);
        return;
    }

    const Instruction* src_cur = src;

    instr_buf.appendMulti(src_cur, instr_buf_rem);
    src_cur += instr_buf_rem;

    emptyInstructionBuffer(out, instr_buf, fd);

    for(uintmax_t i = 0; i < (src_c - instr_buf_rem) / instr_buf._cap; i++) {
        instr_buf.appendMulti(src_cur, instr_buf._cap);
        src_cur += instr_buf._cap;

        emptyInstructionBuffer(out, instr_buf, fd);
    }

    const uintmax_t src_rem = (src_c - instr_buf_rem) % instr_buf._cap;

    instr_buf.appendMulti(src_cur, src_rem);
}

}

}
