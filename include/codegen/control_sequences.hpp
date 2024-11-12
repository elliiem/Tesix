#pragma once

#include "../array_list.hpp"
#include "../bits.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>

namespace Tesix {

namespace ControlSeq {

consteval uint8_t decodeRowColumn(const size_t row, const size_t column) {
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
constexpr uint8_t CPL = decodeRowColumn(4, 5);
constexpr uint8_t CNL = decodeRowColumn(4, 6);
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

template<TypeArrayList<uint8_t> T>
static inline void writeCSI(T& out) {
    out.appendAssume(ESC);
    out.appendAssume('[');
}

template<TypeArrayList<uint8_t> T>
static inline void writeSeparator(T& out) {
    out.appendAssume(';');
}

static inline size_t countDigits(const size_t value) {
    return static_cast<size_t>(ceil(log10(value)) + 1);
}

template<TypeArrayList<uint8_t> T>
static inline void writeParameter(const size_t value, T& out) {
    out.len += sprintf(reinterpret_cast<char*>(out.buffer + out.len), "%lu", value);
}

template<TypeArrayList<uint8_t> T>
static inline void flushBuffer(T& out) {
    write(STDOUT_FILENO, out.buffer, out.len);

    out.clear();
}

template<TypeArrayList<uint8_t> T>
static inline void ensureSpace(const size_t space, T& out);

template<>
inline void ensureSpace<ArrayList<uint8_t, Dynamic>>(const size_t space, ArrayList<uint8_t, Dynamic>& out) {
    if(out.freeSpace() < space) {
        if(out.capacity < space) {
            flushBuffer(out);

            out.expandCapacityNoCopy(space);

            return;
        }

        flushBuffer(out);
    }
}

template<>
inline void ensureSpace<ArrayList<uint8_t, Static>>(const size_t space, ArrayList<uint8_t, Static>& out) {
    if(out.freeSpace() < space) {
        if(out.capacity < space) {
            throw std::runtime_error("Not enough space in buffer");
        }

        flushBuffer(out);
    }
}

enum class CursorInstrType {
    Linefeed,
    ReverseLinefeed,
    Newline,
    CursorUp,
    CursorDown,
    CursorRight,
    CursorLeft,
    CursorUpBegin,
    CursorDownBegin,
    CursorRow,
    CursorColumn,
    CursorTo,
    SaveCursor,
    RestoreCursor,
};

struct CursorTo {
    size_t row;
    size_t column;
};

union CursorInstrParam {
    size_t CursorUp;
    size_t CursorDown;
    size_t CursorLeft;
    size_t CursorRight;
    size_t CursorUpBegin;
    size_t CursorDownBegin;
    size_t CursorRow;
    size_t CursorColumn;
    struct CursorTo CursorTo;
};

struct CursorInstruction {
    CursorInstrType type;
    CursorInstrParam params;

    [[clang::always_inline]] static inline CursorInstruction createCursorUp(const size_t n) {
        return {.type = CursorInstrType::CursorUp, .params = {.CursorUp = n}};
    }

    [[clang::always_inline]] static inline CursorInstruction createCursorDown(const size_t n) {
        return {.type = CursorInstrType::CursorDown, .params = {.CursorDown = n}};
    }

    [[clang::always_inline]] static inline CursorInstruction createCursorLeft(const size_t n) {
        return {.type = CursorInstrType::CursorLeft, .params = {.CursorLeft = n}};
    }

    [[clang::always_inline]] static inline CursorInstruction createCursorRight(const size_t n) {
        return {.type = CursorInstrType::CursorRight, .params = {.CursorRight = n}};
    }

    [[clang::always_inline]] static inline CursorInstruction createCursorUpBegin(const size_t n) {
        return {.type = CursorInstrType::CursorUpBegin, .params = {.CursorUpBegin = n}};
    }

    [[clang::always_inline]] static inline CursorInstruction createCursorDownBegin(const size_t n) {
        return {.type = CursorInstrType::CursorDownBegin, .params = {.CursorDownBegin = n}};
    }

    [[clang::always_inline]] static inline CursorInstruction createCursorRow(const size_t row) {
        return {.type = CursorInstrType::CursorRow, .params = {.CursorRow = row}};
    }

    [[clang::always_inline]] static inline CursorInstruction createCursorColumn(const size_t column) {
        return {.type = CursorInstrType::CursorColumn, .params = {.CursorColumn = column}};
    }

    [[clang::always_inline]] static inline CursorInstruction createCursorTo(const CursorTo& params) {
        return {.type = CursorInstrType::CursorTo, .params = {.CursorTo = params}};
    }
};

template<TypeArrayList<uint8_t> T>
static void submitCursor(const CursorInstruction& instr, T& out) {
    switch(instr.type) {
        case CursorInstrType::Linefeed: {
            ensureSpace(2, out);

            out.appendAssume(ESC);
            out.appendAssume(LF);
        } break;
        case CursorInstrType::ReverseLinefeed: {
            ensureSpace(2, out);

            out.appendAssume(ESC);
            out.appendAssume(RI);
        } break;
        case CursorInstrType::Newline: {
            ensureSpace(2, out);

            out.appendAssume(ESC);
            out.appendAssume(NEL);

        } break;
        case CursorInstrType::CursorUp: {
            ensureSpace(3 + countDigits(instr.params.CursorUp), out);

            writeCSI(out);
            writeParameter(instr.params.CursorUp, out);
            out.appendAssume(CUU);
        } break;
        case CursorInstrType::CursorDown: {
            ensureSpace(3 + countDigits(instr.params.CursorDown), out);

            writeCSI(out);
            writeParameter(instr.params.CursorDown, out);
            out.appendAssume(CUD);
        } break;
        case CursorInstrType::CursorRight: {
            ensureSpace(3 + countDigits(instr.params.CursorRight), out);

            writeCSI(out);
            writeParameter(instr.params.CursorRight, out);
            out.appendAssume(CUF);
        } break;
        case CursorInstrType::CursorLeft: {
            ensureSpace(3 + countDigits(instr.params.CursorLeft), out);

            writeCSI(out);
            writeParameter(instr.params.CursorLeft, out);
            out.appendAssume(CUB);
        } break;
        case CursorInstrType::CursorUpBegin: {
            ensureSpace(3 + countDigits(instr.params.CursorUpBegin), out);

            writeCSI(out);
            writeParameter(instr.params.CursorUpBegin, out);
            out.appendAssume(CPL);
        } break;
        case CursorInstrType::CursorDownBegin: {
            ensureSpace(3 + countDigits(instr.params.CursorDownBegin), out);

            writeCSI(out);
            writeParameter(instr.params.CursorDownBegin, out);
            out.appendAssume(CNL);
        } break;
        case CursorInstrType::CursorRow: {
            ensureSpace(3 + countDigits(instr.params.CursorRow), out);

            writeCSI(out);
            writeParameter(instr.params.CursorRow, out);
            out.appendAssume(VPA);
        } break;
        case CursorInstrType::CursorColumn: {
            ensureSpace(3 + countDigits(instr.params.CursorColumn), out);

            writeCSI(out);
            writeParameter(instr.params.CursorColumn, out);
            out.appendAssume(CHA);
        } break;
        case CursorInstrType::CursorTo: {
            ensureSpace(4 + countDigits(instr.params.CursorTo.column) + countDigits(instr.params.CursorTo.row), out);

            writeCSI(out);
            writeParameter(instr.params.CursorTo.column, out);
            writeSeparator(out);
            writeParameter(instr.params.CursorTo.row, out);
            out.appendAssume(CUP);
        } break;
        case CursorInstrType::SaveCursor: {
            ensureSpace(2, out);

            writeCSI(out);
            out.appendAssume('s');

        } break;
        case CursorInstrType::RestoreCursor: {
            ensureSpace(2, out);

            writeCSI(out);
            out.appendAssume('u');
        } break;
    }
}

enum class EraseInstrType {
    EraseCharacters,
    EraseLine,
    EraseDisplay,
    DeleteCharacters,
    DeleteLines,
};

enum class EraseLine {
    Forwards,
    Backwards,
    All,
};

enum class EraseDisplay {
    Forwards,
    Backwards,
    All,
};

union EraseInstrParam {
    size_t EraseCharacters;
    enum EraseLine EraseLine;
    enum EraseDisplay EraseDisplay;
    size_t DeleteCharacters;
    size_t DeleteLines;
};

struct EraseInstruction {
    EraseInstrType type;
    EraseInstrParam params;
};

template<TypeArrayList<uint8_t> T>
static void submitErase(const EraseInstruction instr, T& out) {
    switch(instr.type) {
        case EraseInstrType::EraseCharacters: {
            ensureSpace(3 + countDigits(instr.params.EraseCharacters), out);

            writeCSI(out);
            writeParameter(instr.params.EraseCharacters, out);
            out.appendAssume(ECH);
        } break;
        case EraseInstrType::EraseLine: {
            switch(instr.params.EraseLine) {
                case EraseLine::Forwards: {
                    ensureSpace(3, out);

                    writeCSI(out);
                    out.appendAssume(EL);
                } break;
                case EraseLine::Backwards: {
                    ensureSpace(4, out);

                    writeCSI(out);
                    out.appendAssume('1');
                    out.appendAssume(EL);
                } break;
                case EraseLine::All: {
                    ensureSpace(4, out);

                    writeCSI(out);
                    out.appendAssume('2');
                    out.appendAssume(EL);
                } break;
            }
        } break;
        case EraseInstrType::EraseDisplay: {
            switch(instr.params.EraseDisplay) {
                case EraseDisplay::Forwards: {
                    ensureSpace(3, out);

                    writeCSI(out);
                    out.appendAssume(ED);
                } break;
                case EraseDisplay::Backwards: {
                    ensureSpace(4, out);

                    writeCSI(out);
                    out.appendAssume('1');
                    out.appendAssume(ED);
                } break;
                case EraseDisplay::All: {
                    ensureSpace(4, out);

                    writeCSI(out);
                    out.appendAssume('2');
                    out.appendAssume(ED);
                } break;
            }
        } break;
        case EraseInstrType::DeleteCharacters: {
            ensureSpace(3 + countDigits(instr.params.DeleteCharacters), out);

            writeCSI(out);
            writeParameter(instr.params.DeleteCharacters, out);
            out.appendAssume(DCH);
        } break;
        case EraseInstrType::DeleteLines: {
            ensureSpace(3 + countDigits(instr.params.DeleteLines), out);

            writeCSI(out);
            writeParameter(instr.params.DeleteLines, out);
            out.appendAssume(DL);
        } break;
    }
}

enum class PaletteInstrType {
    SetPaletteColor,
    ResetPalette,
};

union PaletteInstrParam {
    struct {
        uint8_t n;
        uint8_t r;
        uint8_t g;
        uint8_t b;
    } SetPaletteColor;
};

struct PaletteInstruction {
    PaletteInstrType type;
    PaletteInstrParam params;
};

template<TypeArrayList<uint8_t> T>
static void submitPalette(const PaletteInstruction instr, T& out) {
    switch(instr.type) {
        case PaletteInstrType::SetPaletteColor: {
            ensureSpace(9, out);

            out.appendAssume(OSC);
            out.appendAssume('P');

            assert(instr.params.SetPaletteColor.n <= 15);

            out.len += sprintf(reinterpret_cast<char*>(out.buffer + out.len), "%x01\n", instr.params.SetPaletteColor.n);
            out.len += sprintf(reinterpret_cast<char*>(out.buffer + out.len), "%x02\n", instr.params.SetPaletteColor.r);
            out.len += sprintf(reinterpret_cast<char*>(out.buffer + out.len), "%x02\n", instr.params.SetPaletteColor.g);
            out.len += sprintf(reinterpret_cast<char*>(out.buffer + out.len), "%x02\n", instr.params.SetPaletteColor.b);
        } break;
        case PaletteInstrType::ResetPalette: {
            ensureSpace(2, out);

            out.appendAssume(OSC);
            out.appendAssume('R');
        } break;
    }
}

enum class InsertInstrType {
    InsertCharacter,
    InsertString,
    BlankCharacters,
    BlankLines,
    RepeatCharacter,
};

struct InsertStringParams {
    uint8_t* str;
    size_t lenght;
};

union InsertInstrParam {
    uint8_t InsertCharacter;
    InsertStringParams InsertString;
    size_t BlankCharacters;
    size_t BlankLines;
    size_t RepeatCharacter;
};

struct InsertInstrucion {
    InsertInstrType type;
    InsertInstrParam params;

    [[clang::always_inline]] static inline InsertInstrucion createInsertChar(const uint8_t ch) {
        return {.type = InsertInstrType::InsertCharacter, .params = {.InsertCharacter = ch}};
    }

    [[clang::always_inline]] static inline InsertInstrucion createInsertString(const InsertStringParams& params) {
        return {.type = InsertInstrType::InsertString, .params = {.InsertString = params}};
    }
};

template<TypeArrayList<uint8_t> T>
static void submitInsert(const InsertInstrucion instr, T& out) {
    switch(instr.type) {
        case InsertInstrType::InsertCharacter: {
            ensureSpace(1, out);

            out.appendAssume(instr.params.InsertCharacter);
        } break;
        case InsertInstrType::InsertString: {
            ensureSpace(instr.params.InsertString.lenght, out);

            out.appendArrayAssume(instr.params.InsertString.str, instr.params.InsertString.lenght);

            free(instr.params.InsertString.str);
        } break;
        case InsertInstrType::BlankCharacters: {
            ensureSpace(3 + countDigits(instr.params.BlankCharacters), out);

            writeCSI(out);
            writeParameter(instr.params.BlankCharacters, out);
            out.appendAssume(ICH);
        } break;
        case InsertInstrType::BlankLines: {
            ensureSpace(3 + countDigits(instr.params.BlankCharacters), out);

            writeCSI(out);
            writeParameter(instr.params.BlankCharacters, out);
            out.appendAssume(IL);
        } break;
        case InsertInstrType::RepeatCharacter: {
            ensureSpace(3 + countDigits(instr.params.BlankCharacters), out);

            writeCSI(out);
            writeParameter(instr.params.BlankCharacters, out);
            out.appendAssume(REP);
        } break;
    }
}

enum class StyleInstrType {
    ForegroundColor,
    ForegroundFullColor,
    BackgroundColor,
    BackgroundFullColor,
    ModifierBold,
    ModifierItalic,
    ModifierUnderlined,
    SaveStyle,
    RestoreStyle,
};

union StyleInstrParm {
    uint8_t ForegroundColor;
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    } ForegroundFullColor;
    uint8_t BackgroundColor;
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    } BackgroundFullColor;
    bool ModifierBold;
    bool ModifierItalic;
    bool ModifierUnderlined;
};

struct StyleInstruction {
    StyleInstrType type;
    StyleInstrParm params;
};

template<TypeArrayList<uint8_t> T>
static inline void submitStyle(const StyleInstruction instr, T& out) {
    switch(instr.type) {
        case StyleInstrType::ForegroundColor: {
            ensureSpace(4, out);

            assert(instr.params.ForegroundColor <= 15);

            if(instr.params.ForegroundColor < 7) {
                writeCSI(out);
                out.appendAssume('3');
                writeParameter(instr.params.ForegroundColor, out);
                out.appendAssume(SGR);
            } else {
                writeCSI(out);
                out.appendAssume('9');
                writeParameter(instr.params.ForegroundColor - 8, out);
                out.appendAssume(SGR);
            }
        } break;
        case StyleInstrType::ForegroundFullColor: {
            const size_t r_digits = countDigits(instr.params.ForegroundFullColor.r);
            const size_t g_digits = countDigits(instr.params.ForegroundFullColor.g);
            const size_t b_digits = countDigits(instr.params.ForegroundFullColor.b);

            ensureSpace(7 + r_digits + g_digits + b_digits, out);

            writeCSI(out);
            out.appendAssume('3');
            out.appendAssume('8');
            writeSeparator(out);
            out.appendAssume('2');
            writeSeparator(out);
            writeParameter(instr.params.ForegroundFullColor.r, out);
            writeSeparator(out);
            writeParameter(instr.params.ForegroundFullColor.g, out);
            writeSeparator(out);
            writeParameter(instr.params.ForegroundFullColor.b, out);
            out.appendAssume(SGR);
        } break;
        case StyleInstrType::BackgroundColor: {
            assert(instr.params.ForegroundColor <= 15);

            if(instr.params.ForegroundColor < 7) {
                ensureSpace(4, out);

                writeCSI(out);
                out.appendAssume('4');
                writeParameter(instr.params.ForegroundColor, out);
                out.appendAssume(SGR);
            } else {
                ensureSpace(5, out);

                writeCSI(out);
                out.appendAssume('1');
                out.appendAssume('0');
                writeParameter(instr.params.ForegroundColor - 8, out);
                out.appendAssume(SGR);
            }
        } break;
        case StyleInstrType::BackgroundFullColor: {
            const size_t r_digits = countDigits(instr.params.BackgroundFullColor.r);
            const size_t g_digits = countDigits(instr.params.BackgroundFullColor.g);
            const size_t b_digits = countDigits(instr.params.BackgroundFullColor.b);

            ensureSpace(7 + r_digits + g_digits + b_digits, out);

            writeCSI(out);
            out.appendAssume('4');
            out.appendAssume('8');
            writeSeparator(out);
            out.appendAssume('2');
            writeSeparator(out);
            writeParameter(instr.params.BackgroundFullColor.r, out);
            writeSeparator(out);
            writeParameter(instr.params.BackgroundFullColor.g, out);
            writeSeparator(out);
            writeParameter(instr.params.BackgroundFullColor.b, out);
            out.appendAssume(SGR);
        } break;
        case StyleInstrType::ModifierBold: {
            if(instr.params.ModifierBold) {
                ensureSpace(4, out);

                writeCSI(out);
                out.appendAssume('1');
                out.appendAssume(SGR);
            } else {
                ensureSpace(5, out);

                writeCSI(out);
                out.appendAssume('2');
                out.appendAssume('2');
                out.appendAssume(SGR);
            }
        } break;
        case StyleInstrType::ModifierItalic: {
            if(instr.params.ModifierItalic) {
                ensureSpace(4, out);

                writeCSI(out);
                out.appendAssume('3');
                out.appendAssume(SGR);
            } else {
                ensureSpace(5, out);

                writeCSI(out);
                out.appendAssume('2');
                out.appendAssume('3');
                out.appendAssume(SGR);
            }
        } break;
        case StyleInstrType::ModifierUnderlined: {
            if(instr.params.ModifierUnderlined) {
                ensureSpace(4, out);

                writeCSI(out);
                out.appendAssume('4');
                out.appendAssume(SGR);
            } else {
                ensureSpace(5, out);

                writeCSI(out);
                out.appendAssume('2');
                out.appendAssume('4');
                out.appendAssume(SGR);
            }
        } break;
        case StyleInstrType::SaveStyle: {
            ensureSpace(2, out);
            out.appendAssume(ESC);
            out.appendAssume(DECSC);
        } break;
        case StyleInstrType::RestoreStyle: {
            ensureSpace(2, out);
            out.appendAssume(ESC);
            out.appendAssume(DECRC);
        } break;
    }
}

enum class InstrType {
    Reset,
    Tabstop,
    SetScrollingRegion,
    ISOCharacterSet,
    UTF8CharacterSet,

    Cursor,
    Insert,
    Erase,
    Palette,
    Style,
};

struct SetScrollingRegionParams {
    size_t top;
    size_t bottom;
};

union InstrParameter {
    SetScrollingRegionParams SetScrollingRegion;
    CursorInstruction Cursor;
    InsertInstrucion Insert;
    EraseInstruction Erase;
    PaletteInstruction Palette;
    StyleInstruction Style;
};

struct Instruction {
    InstrType type;
    InstrParameter params;

    [[clang::always_inline]] static inline Instruction createScrollingRegion(const SetScrollingRegionParams& params) {
        return {.type = InstrType::SetScrollingRegion, .params = {.SetScrollingRegion = params}};
    }

    [[clang::always_inline]] static inline Instruction createCursor(const CursorInstruction& instr) {
        return {.type = InstrType::Cursor, .params = {.Cursor = instr}};
    }

    [[clang::always_inline]] static inline Instruction createInsert(const InsertInstrucion& instr) {
        return {.type = InstrType::Insert, .params = {.Insert = instr}};
    }

    [[clang::always_inline]] static inline Instruction createErase(const EraseInstruction& instr) {
        return {.type = InstrType::Erase, .params = {.Erase = instr}};
    }

    [[clang::always_inline]] static inline Instruction createPalette(const PaletteInstruction& instr) {
        return {.type = InstrType::Palette, .params = {.Palette = instr}};
    }

    [[clang::always_inline]] static inline Instruction createStyle(const StyleInstruction& instr) {
        return {.type = InstrType::Style, .params = {.Style = instr}};
    }
};

/**
 * @brief generates escape sequence and writes it to stdout
 **/
template<TypeArrayList<Instruction> I, TypeArrayList<uint8_t> O>
static void submit(const I& instructions, O& out) {
    for(size_t i = 0; i < instructions.len; i++) {
        const Instruction instr = instructions.buffer[i];

        switch(instr.type) {
            case InstrType::Reset: {
                ensureSpace(1, out);

                out.appendAssume(RIS);
            } break;
            case InstrType::Tabstop: {
                ensureSpace(1, out);

                out.appendAssume(HTS);
            } break;
            case InstrType::SetScrollingRegion: {
                const size_t top_digits = countDigits(instr.params.SetScrollingRegion.top);
                const size_t bottom_digits = countDigits(instr.params.SetScrollingRegion.bottom);

                ensureSpace(4 + top_digits + bottom_digits, out);

                writeCSI(out);
                writeParameter(instr.params.SetScrollingRegion.top, out);
                writeSeparator(out);
                writeParameter(instr.params.SetScrollingRegion.bottom, out);
                out.appendAssume(DECSTBM);
            } break;
            case InstrType::ISOCharacterSet: {
                ensureSpace(3, out);

                out.appendAssume(ESC);
                out.appendAssume('%');
                out.appendAssume('@');
            } break;
            case InstrType::UTF8CharacterSet: {
                ensureSpace(3, out);

                out.appendAssume(ESC);
                out.appendAssume('%');
                out.appendAssume('G');
            } break;
            case InstrType::Cursor: {
                submitCursor(instr.params.Cursor, out);
            } break;
            case InstrType::Insert: {
                submitInsert(instr.params.Insert, out);
            } break;
            case InstrType::Erase: {
                submitErase(instr.params.Erase, out);
            } break;
            case InstrType::Palette: {
                submitPalette(instr.params.Palette, out);
            } break;
            case InstrType::Style: {
                submitStyle(instr.params.Style, out);
            } break;
        }
    }

    flushBuffer(out);
}

} // namespace ControlSeq

} // namespace Tesix
