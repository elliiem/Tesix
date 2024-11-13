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
#include <iterator>
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
static inline void writeCSI(T& buffer) {
    buffer.appendAssume(ESC);
    buffer.appendAssume('[');
}

template<TypeArrayList<uint8_t> T>
static inline void writeSeparator(T& buffer) {
    buffer.appendAssume(';');
}

static inline size_t countDigits(const size_t value) {
    return static_cast<size_t>(ceil(log10(value)) + 1);
}

template<TypeArrayList<uint8_t> T>
static inline void writeParameter(const size_t value, T& buffer) {
    buffer.len += sprintf(reinterpret_cast<char*>(buffer.ptr + buffer.len), "%lu", value);
}

template<TypeArrayList<uint8_t> T>
static inline void flushBuffer(T& buffer, size_t out_fd) {
    write(out_fd, buffer.ptr, buffer.len);

    buffer.clear();
}

template<TypeArrayList<uint8_t> T>
static inline void ensureSpace(const size_t space, T& buffer, size_t out_fd);

template<>
inline void ensureSpace<ArrayList<uint8_t, Dynamic>>(const size_t space, ArrayList<uint8_t, Dynamic>& buffer, size_t out_fd) {
    if(buffer.freeSpace() < space) {
        if(buffer.capacity < space) {
            flushBuffer(buffer, out_fd);

            buffer.expandCapacityNoCopy(space);

            return;
        }

        flushBuffer(buffer, out_fd);
    }
}

template<>
inline void ensureSpace<ArrayList<uint8_t, Static>>(const size_t space, ArrayList<uint8_t, Static>& buffer, size_t out_fd) {
    if(buffer.freeSpace() < space) {
        if(buffer.capacity < space) {
            throw std::runtime_error("Not enough space in buffer");
        }

        flushBuffer(buffer, out_fd);
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
static void submitCursor(const CursorInstruction& instr, T& buffer, size_t out_fd) {
    switch(instr.type) {
        case CursorInstrType::Linefeed: {
            ensureSpace(2, buffer, out_fd);

            buffer.appendAssume(ESC);
            buffer.appendAssume(LF);
        } break;
        case CursorInstrType::ReverseLinefeed: {
            ensureSpace(2, buffer, out_fd);

            buffer.appendAssume(ESC);
            buffer.appendAssume(RI);
        } break;
        case CursorInstrType::Newline: {
            ensureSpace(2, buffer, out_fd);

            buffer.appendAssume(ESC);
            buffer.appendAssume(NEL);

        } break;
        case CursorInstrType::CursorUp: {
            ensureSpace(3 + countDigits(instr.params.CursorUp), buffer, out_fd);

            writeCSI(buffer);
            writeParameter(instr.params.CursorUp, buffer);
            buffer.appendAssume(CUU);
        } break;
        case CursorInstrType::CursorDown: {
            ensureSpace(3 + countDigits(instr.params.CursorDown), buffer, out_fd);

            writeCSI(buffer);
            writeParameter(instr.params.CursorDown, buffer);
            buffer.appendAssume(CUD);
        } break;
        case CursorInstrType::CursorRight: {
            ensureSpace(3 + countDigits(instr.params.CursorRight), buffer, out_fd);

            writeCSI(buffer);
            writeParameter(instr.params.CursorRight, buffer);
            buffer.appendAssume(CUF);
        } break;
        case CursorInstrType::CursorLeft: {
            ensureSpace(3 + countDigits(instr.params.CursorLeft), buffer, out_fd);

            writeCSI(buffer);
            writeParameter(instr.params.CursorLeft, buffer);
            buffer.appendAssume(CUB);
        } break;
        case CursorInstrType::CursorUpBegin: {
            ensureSpace(3 + countDigits(instr.params.CursorUpBegin), buffer, out_fd);

            writeCSI(buffer);
            writeParameter(instr.params.CursorUpBegin, buffer);
            buffer.appendAssume(CPL);
        } break;
        case CursorInstrType::CursorDownBegin: {
            ensureSpace(3 + countDigits(instr.params.CursorDownBegin), buffer, out_fd);

            writeCSI(buffer);
            writeParameter(instr.params.CursorDownBegin, buffer);
            buffer.appendAssume(CNL);
        } break;
        case CursorInstrType::CursorRow: {
            ensureSpace(3 + countDigits(instr.params.CursorRow), buffer, out_fd);

            writeCSI(buffer);
            writeParameter(instr.params.CursorRow, buffer);
            buffer.appendAssume(VPA);
        } break;
        case CursorInstrType::CursorColumn: {
            ensureSpace(3 + countDigits(instr.params.CursorColumn), buffer, out_fd);

            writeCSI(buffer);
            writeParameter(instr.params.CursorColumn, buffer);
            buffer.appendAssume(CHA);
        } break;
        case CursorInstrType::CursorTo: {
            ensureSpace(4 + countDigits(instr.params.CursorTo.column) + countDigits(instr.params.CursorTo.row), buffer, out_fd);

            writeCSI(buffer);
            writeParameter(instr.params.CursorTo.column, buffer);
            writeSeparator(buffer);
            writeParameter(instr.params.CursorTo.row, buffer);
            buffer.appendAssume(CUP);
        } break;
        case CursorInstrType::SaveCursor: {
            ensureSpace(2, buffer, out_fd);

            writeCSI(buffer);
            buffer.appendAssume('s');

        } break;
        case CursorInstrType::RestoreCursor: {
            ensureSpace(2, buffer, out_fd);

            writeCSI(buffer);
            buffer.appendAssume('u');
        } break;
    }
}

enum class EraseInstrType {
    EraseChars,
    EraseLine,
    EraseDisplay,
    DeleteChars,
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
    size_t EraseChars;
    enum EraseLine EraseLine;
    enum EraseDisplay EraseDisplay;
    size_t DeleteChars;
    size_t DeleteLines;
};

struct EraseInstruction {
    EraseInstrType type;
    EraseInstrParam params;

    [[clang::always_inline]] static inline EraseInstruction createEraseChars(const size_t n) {
        return {.type = EraseInstrType::EraseChars, .params = {.EraseChars = n}};
    }

    [[clang::always_inline]] static inline EraseInstruction createEraseLine(const EraseLine mode) {
        return {.type = EraseInstrType::EraseLine, .params = {.EraseLine = mode}};
    }

    [[clang::always_inline]] static inline EraseInstruction createEraseDisplay(const EraseDisplay mode) {
        return {.type = EraseInstrType::EraseDisplay, .params = {.EraseDisplay = mode}};
    }

    [[clang::always_inline]] static inline EraseInstruction createDeleteChars(const size_t n) {
        return {.type = EraseInstrType::DeleteChars, .params = {.DeleteChars = n}};
    }

    [[clang::always_inline]] static inline EraseInstruction createDeleteLines(const size_t n) {
        return {.type = EraseInstrType::DeleteLines, .params = {.DeleteLines = n}};
    }
};

template<TypeArrayList<uint8_t> T>
static void submitErase(const EraseInstruction instr, T& buffer, size_t out_fd) {
    switch(instr.type) {
        case EraseInstrType::EraseChars: {
            ensureSpace(3 + countDigits(instr.params.EraseChars), buffer, out_fd);

            writeCSI(buffer);
            writeParameter(instr.params.EraseChars, buffer);
            buffer.appendAssume(ECH);
        } break;
        case EraseInstrType::EraseLine: {
            switch(instr.params.EraseLine) {
                case EraseLine::Forwards: {
                    ensureSpace(3, buffer, out_fd);

                    writeCSI(buffer);
                    buffer.appendAssume(EL);
                } break;
                case EraseLine::Backwards: {
                    ensureSpace(4, buffer, out_fd);

                    writeCSI(buffer);
                    buffer.appendAssume('1');
                    buffer.appendAssume(EL);
                } break;
                case EraseLine::All: {
                    ensureSpace(4, buffer, out_fd);

                    writeCSI(buffer);
                    buffer.appendAssume('2');
                    buffer.appendAssume(EL);
                } break;
            }
        } break;
        case EraseInstrType::EraseDisplay: {
            switch(instr.params.EraseDisplay) {
                case EraseDisplay::Forwards: {
                    ensureSpace(3, buffer, out_fd);

                    writeCSI(buffer);
                    buffer.appendAssume(ED);
                } break;
                case EraseDisplay::Backwards: {
                    ensureSpace(4, buffer, out_fd);

                    writeCSI(buffer);
                    buffer.appendAssume('1');
                    buffer.appendAssume(ED);
                } break;
                case EraseDisplay::All: {
                    ensureSpace(4, buffer, out_fd);

                    writeCSI(buffer);
                    buffer.appendAssume('2');
                    buffer.appendAssume(ED);
                } break;
            }
        } break;
        case EraseInstrType::DeleteChars: {
            ensureSpace(3 + countDigits(instr.params.DeleteChars), buffer, out_fd);

            writeCSI(buffer);
            writeParameter(instr.params.DeleteChars, buffer);
            buffer.appendAssume(DCH);
        } break;
        case EraseInstrType::DeleteLines: {
            ensureSpace(3 + countDigits(instr.params.DeleteLines), buffer, out_fd);

            writeCSI(buffer);
            writeParameter(instr.params.DeleteLines, buffer);
            buffer.appendAssume(DL);
        } break;
    }
}

enum class PaletteInstrType {
    SetPaletteColor,
    ResetPalette,
};

struct SetPaletteColorParams {
    uint8_t n;
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

union PaletteInstrParam {
    SetPaletteColorParams SetPaletteColor;
};

struct PaletteInstruction {
    PaletteInstrType type;
    PaletteInstrParam params;

    [[clang::always_inline]] static inline PaletteInstruction createSetPaletteColor(const SetPaletteColorParams& params) {
        return {.type = PaletteInstrType::SetPaletteColor, .params = {.SetPaletteColor = params}};
    }

    [[clang::always_inline]] static inline PaletteInstruction createResetPalette() {
        return {.type = PaletteInstrType::ResetPalette};
    }
};

template<TypeArrayList<uint8_t> T>
static void submitPalette(const PaletteInstruction instr, T& buffer, size_t out_fd) {
    switch(instr.type) {
        case PaletteInstrType::SetPaletteColor: {
            ensureSpace(9, buffer, out_fd);

            buffer.appendAssume(OSC);
            buffer.appendAssume('P');

            assert(instr.params.SetPaletteColor.n <= 15);

            buffer.len += sprintf(reinterpret_cast<char*>(buffer.ptr + buffer.len), "%x01\n", instr.params.SetPaletteColor.n);
            buffer.len += sprintf(reinterpret_cast<char*>(buffer.ptr + buffer.len), "%x02\n", instr.params.SetPaletteColor.r);
            buffer.len += sprintf(reinterpret_cast<char*>(buffer.ptr + buffer.len), "%x02\n", instr.params.SetPaletteColor.g);
            buffer.len += sprintf(reinterpret_cast<char*>(buffer.ptr + buffer.len), "%x02\n", instr.params.SetPaletteColor.b);
        } break;
        case PaletteInstrType::ResetPalette: {
            ensureSpace(2, buffer, out_fd);

            buffer.appendAssume(OSC);
            buffer.appendAssume('R');
        } break;
    }
}

enum class InsertInstrType {
    InsertChars,
    InsertString,
    BlankChars,
    BlankLines,
    RepeatChar,
};

struct InsertStringParams {
    uint8_t* str;
    size_t lenght;
};

union InsertInstrParam {
    uint8_t InsertChars;
    InsertStringParams InsertString;
    size_t BlankChars;
    size_t BlankLines;
    size_t RepeatChar;
};

struct InsertInstrucion {
    InsertInstrType type;
    InsertInstrParam params;

    [[clang::always_inline]] static inline InsertInstrucion createInsertChar(const uint8_t ch) {
        return {.type = InsertInstrType::InsertChars, .params = {.InsertChars = ch}};
    }

    [[clang::always_inline]] static inline InsertInstrucion createInsertString(const InsertStringParams& params) {
        return {.type = InsertInstrType::InsertString, .params = {.InsertString = params}};
    }

    [[clang::always_inline]] static inline InsertInstrucion createBlankChars(const size_t n) {
        return {.type = InsertInstrType::BlankChars, .params = {.BlankChars = n}};
    }

    [[clang::always_inline]] static inline InsertInstrucion createBlankLines(const size_t n) {
        return {.type = InsertInstrType::BlankLines, .params = {.BlankLines = n}};
    }

    [[clang::always_inline]] static inline InsertInstrucion createRepeatChar(const size_t n) {
        return {.type = InsertInstrType::RepeatChar, .params = {.RepeatChar = n}};
    }
};

template<TypeArrayList<uint8_t> T>
static void submitInsert(const InsertInstrucion instr, T& buffer, size_t out_fd) {
    switch(instr.type) {
        case InsertInstrType::InsertChars: {
            ensureSpace(1, buffer, out_fd);

            buffer.appendAssume(instr.params.InsertChars);
        } break;
        case InsertInstrType::InsertString: {
            ensureSpace(instr.params.InsertString.lenght, buffer, out_fd);

            buffer.appendArrayAssume(instr.params.InsertString.str, instr.params.InsertString.lenght);

            free(instr.params.InsertString.str);
        } break;
        case InsertInstrType::BlankChars: {
            ensureSpace(3 + countDigits(instr.params.BlankChars), buffer, out_fd);

            writeCSI(buffer);
            writeParameter(instr.params.BlankChars, buffer);
            buffer.appendAssume(ICH);
        } break;
        case InsertInstrType::BlankLines: {
            ensureSpace(3 + countDigits(instr.params.BlankChars), buffer, out_fd);

            writeCSI(buffer);
            writeParameter(instr.params.BlankChars, buffer);
            buffer.appendAssume(IL);
        } break;
        case InsertInstrType::RepeatChar: {
            ensureSpace(3 + countDigits(instr.params.BlankChars), buffer, out_fd);

            writeCSI(buffer);
            writeParameter(instr.params.BlankChars, buffer);
            buffer.appendAssume(REP);
        } break;
    }
}

enum class StyleInstrType {
    ForegroundColor,
    ForegroundFullColor,
    BackgroundColor,
    BackgroundFullColor,
    Bold,
    Italic,
    Underlined,
    SaveStyle,
    RestoreStyle,
};

struct FullColorParams {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

union StyleInstrParm {
    uint8_t ForegroundColor;
    FullColorParams ForegroundFullColor;
    uint8_t BackgroundColor;
    FullColorParams BackgroundFullColor;
    bool Bold;
    bool Italic;
    bool Underlined;
};

struct StyleInstruction {
    StyleInstrType type;
    StyleInstrParm params;

    [[clang::always_inline]] static inline StyleInstruction createForegroundColor(const uint8_t col) {
        assert(col <= 15);

        return {.type = StyleInstrType::ForegroundColor, .params = {.ForegroundColor = col}};
    }

    [[clang::always_inline]] static inline StyleInstruction createForegroundFullColor(const FullColorParams& params) {
        return {.type = StyleInstrType::ForegroundFullColor, .params = {.ForegroundFullColor = params}};
    }

    [[clang::always_inline]] static inline StyleInstruction createBackgroundColor(const uint8_t col) {
        assert(col <= 15);

        return {.type = StyleInstrType::BackgroundColor, .params = {.BackgroundColor = col}};
    }

    [[clang::always_inline]] static inline StyleInstruction createBackgroundFullColor(const FullColorParams& params) {
        return {.type = StyleInstrType::BackgroundFullColor, .params = {.BackgroundFullColor = params}};
    }

    [[clang::always_inline]] static inline StyleInstruction createBold(bool value) {
        return {.type = StyleInstrType::Bold, .params = {.Bold = value}};
    }

    [[clang::always_inline]] static inline StyleInstruction createItalic(bool value) {
        return {.type = StyleInstrType::Italic, .params = {.Italic = value}};
    }

    [[clang::always_inline]] static inline StyleInstruction createUnderlined(bool value) {
        return {.type = StyleInstrType::Underlined, .params = {.Underlined = value}};
    }

    [[clang::always_inline]] static inline StyleInstruction createSaveStyle() {
        return {.type = StyleInstrType::SaveStyle};
    }

    [[clang::always_inline]] static inline StyleInstruction createRestoreStyle() {
        return {.type = StyleInstrType::RestoreStyle};
    }
};

template<TypeArrayList<uint8_t> T>
static inline void submitStyle(const StyleInstruction instr, T& buffer, size_t out_fd) {
    switch(instr.type) {
        case StyleInstrType::ForegroundColor: {
            ensureSpace(4, buffer, out_fd);

            assert(instr.params.ForegroundColor <= 15);

            if(instr.params.ForegroundColor < 7) {
                writeCSI(buffer);
                buffer.appendAssume('3');
                writeParameter(instr.params.ForegroundColor, buffer);
                buffer.appendAssume(SGR);
            } else {
                writeCSI(buffer);
                buffer.appendAssume('9');
                writeParameter(instr.params.ForegroundColor - 8, buffer);
                buffer.appendAssume(SGR);
            }
        } break;
        case StyleInstrType::ForegroundFullColor: {
            const size_t r_digits = countDigits(instr.params.ForegroundFullColor.r);
            const size_t g_digits = countDigits(instr.params.ForegroundFullColor.g);
            const size_t b_digits = countDigits(instr.params.ForegroundFullColor.b);

            ensureSpace(7 + r_digits + g_digits + b_digits, buffer, out_fd);

            writeCSI(buffer);
            buffer.appendAssume('3');
            buffer.appendAssume('8');
            writeSeparator(buffer);
            buffer.appendAssume('2');
            writeSeparator(buffer);
            writeParameter(instr.params.ForegroundFullColor.r, buffer);
            writeSeparator(buffer);
            writeParameter(instr.params.ForegroundFullColor.g, buffer);
            writeSeparator(buffer);
            writeParameter(instr.params.ForegroundFullColor.b, buffer);
            buffer.appendAssume(SGR);
        } break;
        case StyleInstrType::BackgroundColor: {
            assert(instr.params.ForegroundColor <= 15);

            if(instr.params.ForegroundColor < 7) {
                ensureSpace(4, buffer, out_fd);

                writeCSI(buffer);
                buffer.appendAssume('4');
                writeParameter(instr.params.ForegroundColor, buffer);
                buffer.appendAssume(SGR);
            } else {
                ensureSpace(5, buffer, out_fd);

                writeCSI(buffer);
                buffer.appendAssume('1');
                buffer.appendAssume('0');
                writeParameter(instr.params.ForegroundColor - 8, buffer);
                buffer.appendAssume(SGR);
            }
        } break;
        case StyleInstrType::BackgroundFullColor: {
            const size_t r_digits = countDigits(instr.params.BackgroundFullColor.r);
            const size_t g_digits = countDigits(instr.params.BackgroundFullColor.g);
            const size_t b_digits = countDigits(instr.params.BackgroundFullColor.b);

            ensureSpace(7 + r_digits + g_digits + b_digits, buffer, out_fd);

            writeCSI(buffer);
            buffer.appendAssume('4');
            buffer.appendAssume('8');
            writeSeparator(buffer);
            buffer.appendAssume('2');
            writeSeparator(buffer);
            writeParameter(instr.params.BackgroundFullColor.r, buffer);
            writeSeparator(buffer);
            writeParameter(instr.params.BackgroundFullColor.g, buffer);
            writeSeparator(buffer);
            writeParameter(instr.params.BackgroundFullColor.b, buffer);
            buffer.appendAssume(SGR);
        } break;
        case StyleInstrType::Bold: {
            if(instr.params.Bold) {
                ensureSpace(4, buffer, out_fd);

                writeCSI(buffer);
                buffer.appendAssume('1');
                buffer.appendAssume(SGR);
            } else {
                ensureSpace(5, buffer, out_fd);

                writeCSI(buffer);
                buffer.appendAssume('2');
                buffer.appendAssume('2');
                buffer.appendAssume(SGR);
            }
        } break;
        case StyleInstrType::Italic: {
            if(instr.params.Italic) {
                ensureSpace(4, buffer, out_fd);

                writeCSI(buffer);
                buffer.appendAssume('3');
                buffer.appendAssume(SGR);
            } else {
                ensureSpace(5, buffer, out_fd);

                writeCSI(buffer);
                buffer.appendAssume('2');
                buffer.appendAssume('3');
                buffer.appendAssume(SGR);
            }
        } break;
        case StyleInstrType::Underlined: {
            if(instr.params.Underlined) {
                ensureSpace(4, buffer, out_fd);

                writeCSI(buffer);
                buffer.appendAssume('4');
                buffer.appendAssume(SGR);
            } else {
                ensureSpace(5, buffer, out_fd);

                writeCSI(buffer);
                buffer.appendAssume('2');
                buffer.appendAssume('4');
                buffer.appendAssume(SGR);
            }
        } break;
        case StyleInstrType::SaveStyle: {
            ensureSpace(2, buffer, out_fd);
            buffer.appendAssume(ESC);
            buffer.appendAssume(DECSC);
        } break;
        case StyleInstrType::RestoreStyle: {
            ensureSpace(2, buffer, out_fd);
            buffer.appendAssume(ESC);
            buffer.appendAssume(DECRC);
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
 * @brief generates escape sequence and writes it to stdout_fd
 **/
template<TypeArrayList<Instruction> I, TypeArrayList<uint8_t> B>
static void submit(const I& instructions, B& buffer, size_t out_fd = STDOUT_FILENO) {
    for(size_t i = 0; i < instructions.len; i++) {
        const Instruction instr = instructions.ptr[i];

        switch(instr.type) {
            case InstrType::Reset: {
                ensureSpace(1, buffer, out_fd);

                buffer.appendAssume(RIS);
            } break;
            case InstrType::Tabstop: {
                ensureSpace(1, buffer, out_fd);

                buffer.appendAssume(HTS);
            } break;
            case InstrType::SetScrollingRegion: {
                const size_t top_digits = countDigits(instr.params.SetScrollingRegion.top);
                const size_t bottom_digits = countDigits(instr.params.SetScrollingRegion.bottom);

                ensureSpace(4 + top_digits + bottom_digits, buffer, out_fd);

                writeCSI(buffer);
                writeParameter(instr.params.SetScrollingRegion.top, buffer);
                writeSeparator(buffer);
                writeParameter(instr.params.SetScrollingRegion.bottom, buffer);
                buffer.appendAssume(DECSTBM);
            } break;
            case InstrType::ISOCharacterSet: {
                ensureSpace(3, buffer, out_fd);

                buffer.appendAssume(ESC);
                buffer.appendAssume('%');
                buffer.appendAssume('@');
            } break;
            case InstrType::UTF8CharacterSet: {
                ensureSpace(3, buffer, out_fd);

                buffer.appendAssume(ESC);
                buffer.appendAssume('%');
                buffer.appendAssume('G');
            } break;
            case InstrType::Cursor: {
                submitCursor(instr.params.Cursor, buffer, out_fd);
            } break;
            case InstrType::Insert: {
                submitInsert(instr.params.Insert, buffer, out_fd);
            } break;
            case InstrType::Erase: {
                submitErase(instr.params.Erase, buffer, out_fd);
            } break;
            case InstrType::Palette: {
                submitPalette(instr.params.Palette, buffer, out_fd);
            } break;
            case InstrType::Style: {
                submitStyle(instr.params.Style, buffer, out_fd);
            } break;
        }
    }

    flushBuffer(buffer, out_fd);
}

} // namespace ControlSeq

} // namespace Tesix
