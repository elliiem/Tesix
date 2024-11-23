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

#ifdef _DEBUG
    #define callEnsureSpace(size, buffer, out_fd, debug_out_fd) ensureSpace(size, buffer, out_fd, debug_out_fd)
    #define callFlushBuffer(buffer, out_fd, debug_out_fd) flushBuffer(buffer, out_fd, debug_out_fd)
    #define callSubmitCursor(instr, buffer, out_fd, debug_out_fd) submitCursor(instr, buffer, out_fd, debug_out_fd)
    #define callSubmitErase(instr, buffer, out_fd, debug_out_fd) submitErase(instr, buffer, out_fd, debug_out_fd)
    #define callSubmitPalette(instr, buffer, out_fd, debug_out_fd) submitPalette(instr, buffer, out_fd, debug_out_fd)
    #define callSubmitInsert(instr, buffer, out_fd, debug_out_fd) submitInsert(instr, buffer, out_fd, debug_out_fd)
    #define callSubmitStyle(instr, buffer, out_fd, debug_out_fd) submitStyle(instr, buffer, out_fd, debug_out_fd)
#else
    #define callEnsureSpace(size, buffer, out_fd, debug_out_fd) ensureSpace(size, buffer, out_fd)
    #define callFlushBuffer(buffer, out_fd, debug_out_fd) flushBuffer(buffer, out_fd)
    #define callSubmitCursor(instr, buffer, out_fd, debug_out_fd) submitCursor(instr, buffer, out_fd)
    #define callSubmitErase(instr, buffer, out_fd, debug_out_fd) submitErase(instr, buffer, out_fd)
    #define callSubmitPalette(instr, buffer, out_fd, debug_out_fd) submitPalette(instr, buffer, out_fd)
    #define callSubmitInsert(instr, buffer, out_fd, debug_out_fd) submitInsert(instr, buffer, out_fd)
    #define callSubmitStyle(instr, buffer, out_fd, debug_out_fd) submitStyle(instr, buffer, out_fd)
#endif

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
    if(value == 0) {
        return 1;
    }

    return ceil(log10(value)) + 1;
}

template<TypeArrayList<uint8_t> T>
static inline void writeParameter(const size_t value, T& buffer) {
    buffer.len += sprintf(reinterpret_cast<char*>(buffer.ptr + buffer.len), "%lu", value);
}

#ifdef _DEBUG
template<TypeArrayList<uint8_t> T>
static inline void flushBuffer(T& buffer, const size_t out_fd, const size_t debug_out_fd) {
#else
template<TypeArrayList<uint8_t> T>
static inline void flushBuffer(T& buffer, const size_t out_fd) {
#endif
    write(out_fd, buffer.ptr, buffer.len);

#ifdef _DEBUG
    write(debug_out_fd, buffer.ptr, buffer.len);
#endif

    buffer.clear();
}

#ifdef _DEBUG
template<TypeArrayList<uint8_t> T>
static inline void ensureSpace(const size_t space, T& buffer, const size_t out_fd, const size_t debug_out_fd);
#else
template<TypeArrayList<uint8_t> T>
static inline void ensureSpace(const size_t space, T& buffer, const size_t out_fd);
#endif

#ifdef _DEBUG
template<>
inline void ensureSpace<ArrayList<uint8_t, Dynamic>>(
    const size_t space, ArrayList<uint8_t, Dynamic>& buffer, size_t out_fd, const size_t debug_out_fd) {
#else
template<>
inline void ensureSpace<ArrayList<uint8_t, Dynamic>>(const size_t space, ArrayList<uint8_t, Dynamic>& buffer, size_t out_fd) {
#endif
    if(buffer.freeSpace() < space) {
        if(buffer.capacity < space) {
            callFlushBuffer(buffer, out_fd, debug_out_fd);

            buffer.expandCapacityNoCopy(space);

            return;
        }

        callFlushBuffer(buffer, out_fd, debug_out_fd);
    }
}

#ifdef _DEBUG
template<>
inline void ensureSpace<ArrayList<uint8_t, Static>>(
    const size_t space, ArrayList<uint8_t, Static>& buffer, size_t out_fd, const size_t debug_out_fd) {
#else
template<>
inline void ensureSpace<ArrayList<uint8_t, Static>>(const size_t space, ArrayList<uint8_t, Static>& buffer, size_t out_fd) {
#endif
    if(buffer.freeSpace() < space) {
        if(buffer.capacity < space) {
            throw std::runtime_error("Not enough space in buffer");
        }

        callFlushBuffer(buffer, out_fd, debug_out_fd);
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

struct CursorToParams {
    size_t _row;
    size_t _column;
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
    struct CursorToParams CursorTo;
};

struct CursorInstruction {
    CursorInstrType _type;
    CursorInstrParam _params;
};

#ifdef _DEBUG
template<TypeArrayList<uint8_t> T>
static void submitCursor(const CursorInstruction& instr, T& buffer, const size_t out_fd, const size_t debug_out_fd) {
#else
template<TypeArrayList<uint8_t> T>
static void submitCursor(const CursorInstruction& instr, T& buffer, const size_t out_fd) {
#endif
    switch(instr._type) {
        case CursorInstrType::Linefeed: {
            callEnsureSpace(2, buffer, out_fd, debug_out_fd);

            buffer.appendAssume(ESC);
            buffer.appendAssume(LF);
        } break;
        case CursorInstrType::ReverseLinefeed: {
            callEnsureSpace(2, buffer, out_fd, debug_out_fd);

            buffer.appendAssume(ESC);
            buffer.appendAssume(RI);
        } break;
        case CursorInstrType::Newline: {
            callEnsureSpace(2, buffer, out_fd, debug_out_fd);

            buffer.appendAssume(ESC);
            buffer.appendAssume(NEL);

        } break;
        case CursorInstrType::CursorUp: {
            callEnsureSpace(3 + countDigits(instr._params.CursorUp), buffer, out_fd, debug_out_fd);

            writeCSI(buffer);
            writeParameter(instr._params.CursorUp, buffer);
            buffer.appendAssume(CUU);
        } break;
        case CursorInstrType::CursorDown: {
            callEnsureSpace(3 + countDigits(instr._params.CursorDown), buffer, out_fd, debug_out_fd);

            writeCSI(buffer);
            writeParameter(instr._params.CursorDown, buffer);
            buffer.appendAssume(CUD);
        } break;
        case CursorInstrType::CursorRight: {
            callEnsureSpace(3 + countDigits(instr._params.CursorRight), buffer, out_fd, debug_out_fd);

            writeCSI(buffer);
            writeParameter(instr._params.CursorRight, buffer);
            buffer.appendAssume(CUF);
        } break;
        case CursorInstrType::CursorLeft: {
            callEnsureSpace(3 + countDigits(instr._params.CursorLeft), buffer, out_fd, debug_out_fd);

            writeCSI(buffer);
            writeParameter(instr._params.CursorLeft, buffer);
            buffer.appendAssume(CUB);
        } break;
        case CursorInstrType::CursorUpBegin: {
            callEnsureSpace(3 + countDigits(instr._params.CursorUpBegin), buffer, out_fd, debug_out_fd);

            writeCSI(buffer);
            writeParameter(instr._params.CursorUpBegin, buffer);
            buffer.appendAssume(CPL);
        } break;
        case CursorInstrType::CursorDownBegin: {
            callEnsureSpace(3 + countDigits(instr._params.CursorDownBegin), buffer, out_fd, debug_out_fd);

            writeCSI(buffer);
            writeParameter(instr._params.CursorDownBegin, buffer);
            buffer.appendAssume(CNL);
        } break;
        case CursorInstrType::CursorRow: {
            callEnsureSpace(3 + countDigits(instr._params.CursorRow), buffer, out_fd, debug_out_fd);

            writeCSI(buffer);
            writeParameter(instr._params.CursorRow, buffer);
            buffer.appendAssume(VPA);
        } break;
        case CursorInstrType::CursorColumn: {
            callEnsureSpace(3 + countDigits(instr._params.CursorColumn), buffer, out_fd, debug_out_fd);

            writeCSI(buffer);
            writeParameter(instr._params.CursorColumn, buffer);
            buffer.appendAssume(CHA);
        } break;
        case CursorInstrType::CursorTo: {
            callEnsureSpace(4 + countDigits(instr._params.CursorTo._column) + countDigits(instr._params.CursorTo._row), buffer, out_fd, debug_out_fd);

            writeCSI(buffer);
            writeParameter(instr._params.CursorTo._column, buffer);
            writeSeparator(buffer);
            writeParameter(instr._params.CursorTo._row, buffer);
            buffer.appendAssume(CUP);
        } break;
        case CursorInstrType::SaveCursor: {
            callEnsureSpace(2, buffer, out_fd, debug_out_fd);

            writeCSI(buffer);
            buffer.appendAssume('s');

        } break;
        case CursorInstrType::RestoreCursor: {
            callEnsureSpace(2, buffer, out_fd, debug_out_fd);

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
    EraseInstrType _type;
    EraseInstrParam _params;
};

#ifdef _DEBUG
template<TypeArrayList<uint8_t> T>
static void submitErase(const EraseInstruction& instr, T& buffer, const size_t out_fd, const size_t debug_out_fd) {
#else
template<TypeArrayList<uint8_t> T>
static void submitErase(const EraseInstruction& instr, T& buffer, const size_t out_fd) {
#endif
    switch(instr._type) {
        case EraseInstrType::EraseChars: {
            callEnsureSpace(3 + countDigits(instr._params.EraseChars), buffer, out_fd, debug_out_fd);

            writeCSI(buffer);
            writeParameter(instr._params.EraseChars, buffer);
            buffer.appendAssume(ECH);
        } break;
        case EraseInstrType::EraseLine: {
            switch(instr._params.EraseLine) {
                case EraseLine::Forwards: {
                    callEnsureSpace(3, buffer, out_fd, debug_out_fd);

                    writeCSI(buffer);
                    buffer.appendAssume(EL);
                } break;
                case EraseLine::Backwards: {
                    callEnsureSpace(4, buffer, out_fd, debug_out_fd);

                    writeCSI(buffer);
                    buffer.appendAssume('1');
                    buffer.appendAssume(EL);
                } break;
                case EraseLine::All: {
                    callEnsureSpace(4, buffer, out_fd, debug_out_fd);

                    writeCSI(buffer);
                    buffer.appendAssume('2');
                    buffer.appendAssume(EL);
                } break;
            }
        } break;
        case EraseInstrType::EraseDisplay: {
            switch(instr._params.EraseDisplay) {
                case EraseDisplay::Forwards: {
                    callEnsureSpace(3, buffer, out_fd, debug_out_fd);

                    writeCSI(buffer);
                    buffer.appendAssume(ED);
                } break;
                case EraseDisplay::Backwards: {
                    callEnsureSpace(4, buffer, out_fd, debug_out_fd);

                    writeCSI(buffer);
                    buffer.appendAssume('1');
                    buffer.appendAssume(ED);
                } break;
                case EraseDisplay::All: {
                    callEnsureSpace(4, buffer, out_fd, debug_out_fd);

                    writeCSI(buffer);
                    buffer.appendAssume('2');
                    buffer.appendAssume(ED);
                } break;
            }
        } break;
        case EraseInstrType::DeleteChars: {
            callEnsureSpace(3 + countDigits(instr._params.DeleteChars), buffer, out_fd, debug_out_fd);

            writeCSI(buffer);
            writeParameter(instr._params.DeleteChars, buffer);
            buffer.appendAssume(DCH);
        } break;
        case EraseInstrType::DeleteLines: {
            callEnsureSpace(3 + countDigits(instr._params.DeleteLines), buffer, out_fd, debug_out_fd);

            writeCSI(buffer);
            writeParameter(instr._params.DeleteLines, buffer);
            buffer.appendAssume(DL);
        } break;
    }
}

enum class PaletteInstrType {
    SetPaletteColor,
    ResetPalette,
};

struct SetPaletteColorParams {
    uint8_t _n;
    uint8_t _r;
    uint8_t _g;
    uint8_t _b;
};

union PaletteInstrParam {
    SetPaletteColorParams SetPaletteColor;
};

struct PaletteInstruction {
    PaletteInstrType _type;
    PaletteInstrParam _params;
};

#ifdef _DEBUG
template<TypeArrayList<uint8_t> T>
static void submitPalette(const PaletteInstruction instr, T& buffer, const size_t out_fd, const size_t debug_out_fd) {
#else
template<TypeArrayList<uint8_t> T>
static void submitPalette(const PaletteInstruction instr, T& buffer, const size_t out_fd) {
#endif
    switch(instr._type) {
        case PaletteInstrType::SetPaletteColor: {
            callEnsureSpace(9, buffer, out_fd, debug_out_fd);

            buffer.appendAssume(OSC);
            buffer.appendAssume('P');

            assert(instr._params.SetPaletteColor._n <= 15);

            buffer.len += sprintf(reinterpret_cast<char*>(buffer.ptr + buffer.len), "%x01\n", instr._params.SetPaletteColor._n);
            buffer.len += sprintf(reinterpret_cast<char*>(buffer.ptr + buffer.len), "%x02\n", instr._params.SetPaletteColor._r);
            buffer.len += sprintf(reinterpret_cast<char*>(buffer.ptr + buffer.len), "%x02\n", instr._params.SetPaletteColor._g);
            buffer.len += sprintf(reinterpret_cast<char*>(buffer.ptr + buffer.len), "%x02\n", instr._params.SetPaletteColor._b);
        } break;
        case PaletteInstrType::ResetPalette: {
            callEnsureSpace(2, buffer, out_fd, debug_out_fd);

            buffer.appendAssume(OSC);
            buffer.appendAssume('R');
        } break;
    }
}

enum class InsertInstrType {
    InsertChar,
    InsertString,
    BlankChars,
    BlankLines,
    RepeatChar,
};

struct InsertStringParams {
    uint8_t* _str;
    size_t _len;
};

union InsertInstrParam {
    uint8_t InsertChar;
    InsertStringParams InsertString;
    size_t BlankChars;
    size_t BlankLines;
    size_t RepeatChar;
};

struct InsertInstrucion {
    InsertInstrType _type;
    InsertInstrParam _params;
};

#ifdef _DEBUG
template<TypeArrayList<uint8_t> T>
static void submitInsert(const InsertInstrucion instr, T& buffer, const size_t out_fd, const size_t debug_out_fd) {
#else
template<TypeArrayList<uint8_t> T>
static void submitInsert(const InsertInstrucion instr, T& buffer, const size_t out_fd) {
#endif
    switch(instr._type) {
        case InsertInstrType::InsertChar: {
            callEnsureSpace(1, buffer, out_fd, debug_out_fd);

            buffer.appendAssume(instr._params.InsertChar);
        } break;
        case InsertInstrType::InsertString: {
            callEnsureSpace(instr._params.InsertString._len, buffer, out_fd, debug_out_fd);

            buffer.appendArrayAssume(instr._params.InsertString._str, instr._params.InsertString._len);

            free(instr._params.InsertString._str);
        } break;
        case InsertInstrType::BlankChars: {
            callEnsureSpace(3 + countDigits(instr._params.BlankChars), buffer, out_fd, debug_out_fd);

            writeCSI(buffer);
            writeParameter(instr._params.BlankChars, buffer);
            buffer.appendAssume(ICH);
        } break;
        case InsertInstrType::BlankLines: {
            callEnsureSpace(3 + countDigits(instr._params.BlankChars), buffer, out_fd, debug_out_fd);

            writeCSI(buffer);
            writeParameter(instr._params.BlankChars, buffer);
            buffer.appendAssume(IL);
        } break;
        case InsertInstrType::RepeatChar: {
            callEnsureSpace(3 + countDigits(instr._params.BlankChars), buffer, out_fd, debug_out_fd);

            writeCSI(buffer);
            writeParameter(instr._params.BlankChars, buffer);
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
    Blinking,
    Reversed,
    Strikethrough,
    ResetStyle,
    SaveStyle,
    RestoreStyle,
};

struct FullColorParams {
    uint8_t _r;
    uint8_t _g;
    uint8_t _b;
};

union StyleInstrParam {
    uint8_t ForegroundColor;
    FullColorParams ForegroundFullColor;
    uint8_t BackgroundColor;
    FullColorParams BackgroundFullColor;
    bool Bold;
    bool Italic;
    bool Underlined;
    bool Blinking;
    bool Reversed;
    bool Strikethrough;
};

struct StyleInstruction {
    StyleInstrType _type;
    StyleInstrParam _params;
};

#ifdef _DEBUG
template<TypeArrayList<uint8_t> T>
static inline void submitStyle(const StyleInstruction instr, T& buffer, const size_t out_fd, const size_t debug_out_fd) {
#else
template<TypeArrayList<uint8_t> T>
static inline void submitStyle(const StyleInstruction instr, T& buffer, const size_t out_fd) {
#endif
    switch(instr._type) {
        case StyleInstrType::ForegroundColor: {
            const auto& color = instr._params.ForegroundColor;

            callEnsureSpace(4, buffer, out_fd, debug_out_fd);

            if(color <= 8) {
                writeCSI(buffer);
                buffer.appendAssume('3');
                writeParameter(color, buffer);
                buffer.appendAssume(SGR);
            } else {
                writeCSI(buffer);
                buffer.appendAssume('9');
                writeParameter(color - 8, buffer);
                buffer.appendAssume(SGR);
            }
        } break;
        case StyleInstrType::ForegroundFullColor: {
            const size_t r_digits = countDigits(instr._params.ForegroundFullColor._r);
            const size_t g_digits = countDigits(instr._params.ForegroundFullColor._g);
            const size_t b_digits = countDigits(instr._params.ForegroundFullColor._b);

            callEnsureSpace(7 + r_digits + g_digits + b_digits, buffer, out_fd, debug_out_fd);

            writeCSI(buffer);
            buffer.appendAssume('3');
            buffer.appendAssume('8');
            writeSeparator(buffer);
            buffer.appendAssume('2');
            writeSeparator(buffer);
            writeParameter(instr._params.ForegroundFullColor._r, buffer);
            writeSeparator(buffer);
            writeParameter(instr._params.ForegroundFullColor._g, buffer);
            writeSeparator(buffer);
            writeParameter(instr._params.ForegroundFullColor._b, buffer);
            buffer.appendAssume(SGR);
        } break;
        case StyleInstrType::BackgroundColor: {
            const auto& color = instr._params.ForegroundColor;

            if(color <= 8) {
                callEnsureSpace(4, buffer, out_fd, debug_out_fd);

                writeCSI(buffer);
                buffer.appendAssume('4');
                writeParameter(color, buffer);
                buffer.appendAssume(SGR);
            } else {
                callEnsureSpace(5, buffer, out_fd, debug_out_fd);

                writeCSI(buffer);
                buffer.appendAssume('1');
                buffer.appendAssume('0');
                writeParameter(color - 8, buffer);
                buffer.appendAssume(SGR);
            }
        } break;
        case StyleInstrType::BackgroundFullColor: {
            const size_t r_digits = countDigits(instr._params.BackgroundFullColor._r);
            const size_t g_digits = countDigits(instr._params.BackgroundFullColor._g);
            const size_t b_digits = countDigits(instr._params.BackgroundFullColor._b);

            callEnsureSpace(7 + r_digits + g_digits + b_digits, buffer, out_fd, debug_out_fd);

            writeCSI(buffer);
            buffer.appendAssume('4');
            buffer.appendAssume('8');
            writeSeparator(buffer);
            buffer.appendAssume('2');
            writeSeparator(buffer);
            writeParameter(instr._params.BackgroundFullColor._r, buffer);
            writeSeparator(buffer);
            writeParameter(instr._params.BackgroundFullColor._g, buffer);
            writeSeparator(buffer);
            writeParameter(instr._params.BackgroundFullColor._b, buffer);
            buffer.appendAssume(SGR);
        } break;
        case StyleInstrType::Bold: {
            if(instr._params.Bold) {
                callEnsureSpace(4, buffer, out_fd, debug_out_fd);

                writeCSI(buffer);
                buffer.appendAssume('1');
                buffer.appendAssume(SGR);
            } else {
                callEnsureSpace(5, buffer, out_fd, debug_out_fd);

                writeCSI(buffer);
                buffer.appendAssume('2');
                buffer.appendAssume('2');
                buffer.appendAssume(SGR);
            }
        } break;
        case StyleInstrType::Italic: {
            if(instr._params.Italic) {
                callEnsureSpace(4, buffer, out_fd, debug_out_fd);

                writeCSI(buffer);
                buffer.appendAssume('3');
                buffer.appendAssume(SGR);
            } else {
                callEnsureSpace(5, buffer, out_fd, debug_out_fd);

                writeCSI(buffer);
                buffer.appendAssume('2');
                buffer.appendAssume('3');
                buffer.appendAssume(SGR);
            }
        } break;
        case StyleInstrType::Underlined: {
            if(instr._params.Underlined) {
                callEnsureSpace(4, buffer, out_fd, debug_out_fd);

                writeCSI(buffer);
                buffer.appendAssume('4');
                buffer.appendAssume(SGR);
            } else {
                callEnsureSpace(5, buffer, out_fd, debug_out_fd);

                writeCSI(buffer);
                buffer.appendAssume('2');
                buffer.appendAssume('4');
                buffer.appendAssume(SGR);
            }
        } break;
        case StyleInstrType::Blinking: {
            if(instr._params.Blinking) {
                callEnsureSpace(4, buffer, out_fd, debug_out_fd);

                writeCSI(buffer);
                buffer.appendAssume('5');
                buffer.appendAssume(SGR);
            } else {
                callEnsureSpace(5, buffer, out_fd, debug_out_fd);

                writeCSI(buffer);
                buffer.appendAssume('2');
                buffer.appendAssume('5');
                buffer.appendAssume(SGR);
            }
        } break;
        case StyleInstrType::Reversed: {
            if(instr._params.Reversed) {
                callEnsureSpace(4, buffer, out_fd, debug_out_fd);

                writeCSI(buffer);
                buffer.appendAssume('7');
                buffer.appendAssume(SGR);
            } else {
                callEnsureSpace(5, buffer, out_fd, debug_out_fd);

                writeCSI(buffer);
                buffer.appendAssume('2');
                buffer.appendAssume('7');
                buffer.appendAssume(SGR);
            }
        } break;
        case StyleInstrType::Strikethrough: {
            if(instr._params.Strikethrough) {
                callEnsureSpace(4, buffer, out_fd, debug_out_fd);

                writeCSI(buffer);
                buffer.appendAssume('9');
                buffer.appendAssume(SGR);
            } else {
                callEnsureSpace(5, buffer, out_fd, debug_out_fd);

                writeCSI(buffer);
                buffer.appendAssume('2');
                buffer.appendAssume('9');
                buffer.appendAssume(SGR);
            }
        } break;
        case StyleInstrType::ResetStyle: {
            callEnsureSpace(4, buffer, out_fd, debug_out_fd);

            writeCSI(buffer);
            buffer.appendAssume('0');
            buffer.appendAssume(SGR);
        } break;
        case StyleInstrType::SaveStyle: {
            callEnsureSpace(2, buffer, out_fd, debug_out_fd);
            buffer.appendAssume(ESC);
            buffer.appendAssume(DECSC);
        } break;
        case StyleInstrType::RestoreStyle: {
            callEnsureSpace(2, buffer, out_fd, debug_out_fd);
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
    size_t _top;
    size_t _bottom;
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
    InstrType _type;
    InstrParameter _params;

    static inline Instruction createReset() {
        return {._type = InstrType::Reset};
    }

    static inline Instruction createTabstop() {
        return {._type = InstrType::Tabstop};
    }

    static inline Instruction createScrollingRegion(const SetScrollingRegionParams& params) {
        return {._type = InstrType::SetScrollingRegion, ._params = {.SetScrollingRegion = params}};
    }

    static inline Instruction createISOCharacterSet() {
        return {._type = InstrType::ISOCharacterSet};
    }

    static inline Instruction createUTF8CharacterSet() {
        return {._type = InstrType::UTF8CharacterSet};
    }

    static inline Instruction createLinefeed() {
        return {._type = InstrType::Cursor, ._params = {.Cursor = {._type = CursorInstrType::Linefeed}}};
    }

    static inline Instruction createReverseLinefeed() {
        return {._type = InstrType::Cursor, ._params = {.Cursor = {._type = CursorInstrType::ReverseLinefeed}}};
    }

    static inline Instruction createNewline() {
        return {._type = InstrType::Cursor, ._params = {.Cursor = {._type = CursorInstrType::Newline}}};
    }

    static inline Instruction createCursorUp(const size_t n) {
        return {._type = InstrType::Cursor, ._params = {.Cursor = {._type = CursorInstrType::CursorUp, ._params = {.CursorUp = n}}}};
    }

    static inline Instruction createCursorDown(const size_t n) {
        return {._type = InstrType::Cursor, ._params = {.Cursor = {._type = CursorInstrType::CursorDown, ._params = {.CursorDown = n}}}};
    }

    static inline Instruction createCursorLeft(const size_t n) {
        return {._type = InstrType::Cursor, ._params = {.Cursor = {._type = CursorInstrType::CursorLeft, ._params = {.CursorLeft = n}}}};
    }

    static inline Instruction createCursorRight(const size_t n) {
        return {._type = InstrType::Cursor, ._params = {.Cursor = {._type = CursorInstrType::CursorRight, ._params = {.CursorRight = n}}}};
    }

    static inline Instruction createCursorUpBegin(const size_t n) {
        return {._type = InstrType::Cursor, ._params = {.Cursor = {._type = CursorInstrType::CursorUpBegin, ._params = {.CursorUpBegin = n}}}};
    }

    static inline Instruction createCursorDownBegin(const size_t n) {
        return {._type = InstrType::Cursor, ._params = {.Cursor = {._type = CursorInstrType::CursorDownBegin, ._params = {.CursorDownBegin = n}}}};
    }

    static inline Instruction createCursorRow(const size_t row) {
        return {._type = InstrType::Cursor, ._params = {.Cursor = {._type = CursorInstrType::CursorRow, ._params = {.CursorRow = row}}}};
    }

    static inline Instruction createCursorColumn(const size_t column) {
        return {._type = InstrType::Cursor, ._params = {.Cursor = {._type = CursorInstrType::CursorColumn, ._params = {.CursorColumn = column}}}};
    }

    static inline Instruction createCursorTo(const CursorToParams& params) {
        return {._type = InstrType::Cursor, ._params = {.Cursor = {._type = CursorInstrType::CursorTo, ._params = {.CursorTo = params}}}};
    }

    static inline Instruction createSaveCursor() {
        return {._type = InstrType::Cursor, ._params = {.Cursor = {._type = CursorInstrType::SaveCursor}}};
    }

    static inline Instruction createRestoreCursor() {
        return {._type = InstrType::Cursor, ._params = {.Cursor = {._type = CursorInstrType::RestoreCursor}}};
    }

    static inline Instruction createEraseChars(const size_t n) {
        return {._type = InstrType::Erase, ._params = {.Erase = {._type = EraseInstrType::EraseChars, ._params = {.EraseChars = n}}}};
    }

    static inline Instruction createEraseLine(const EraseLine type) {
        return {._type = InstrType::Erase, ._params = {.Erase = {._type = EraseInstrType::EraseLine, ._params = {.EraseLine = type}}}};
    }

    static inline Instruction createEraseDisplay(const EraseDisplay type) {
        return {._type = InstrType::Erase, ._params = {.Erase = {._type = EraseInstrType::EraseDisplay, ._params = {.EraseDisplay = type}}}};
    }

    static inline Instruction createDeleteChars(const size_t n) {
        return {._type = InstrType::Erase, ._params = {.Erase = {._type = EraseInstrType::DeleteChars, ._params = {.DeleteChars = n}}}};
    }

    static inline Instruction createDeleteLines(const size_t n) {
        return {._type = InstrType::Erase, ._params = {.Erase = {._type = EraseInstrType::DeleteLines, ._params = {.DeleteLines = n}}}};
    }

    static inline Instruction createSetPaletteColor(const SetPaletteColorParams& params) {
        return {._type = InstrType::Palette,
            ._params = {.Palette = {._type = PaletteInstrType::SetPaletteColor, ._params = {.SetPaletteColor = params}}}};
    }

    static inline Instruction createResetPalette() {
        return {._type = InstrType::Palette, ._params = {.Palette = {._type = PaletteInstrType::ResetPalette}}};
    }

    static inline Instruction createInsertChar(const uint8_t ch) {
        return {._type = InstrType::Insert, ._params = {.Insert = {._type = InsertInstrType::InsertChar, ._params = {.InsertChar = ch}}}};
    }

    static inline Instruction createInsertString(const InsertStringParams& params) {
        return {._type = InstrType::Insert, ._params = {.Insert = {._type = InsertInstrType::InsertString, ._params = {.InsertString = params}}}};
    }

    static inline Instruction createBlankChars(const uintmax_t n) {
        return {._type = InstrType::Insert, ._params = {.Insert = {._type = InsertInstrType::BlankChars, ._params = {.BlankChars = n}}}};
    }

    static inline Instruction createBlankLines(const uintmax_t n) {
        return {._type = InstrType::Insert, ._params = {.Insert = {._type = InsertInstrType::BlankLines, ._params = {.BlankLines = n}}}};
    }

    static inline Instruction createRepeatChar(const uintmax_t n) {
        return {._type = InstrType::Insert, ._params = {.Insert = {._type = InsertInstrType::RepeatChar, ._params = {.RepeatChar = n}}}};
    }

    static inline Instruction createForegroundColor(const uint8_t color) {
        return {._type = InstrType::Style, ._params = {.Style = {._type = StyleInstrType::ForegroundColor, ._params = {.ForegroundColor = color}}}};
    }

    static inline Instruction createForegroundFullColor(const FullColorParams& params) {
        return {._type = InstrType::Style, ._params = {.Style = {._type = StyleInstrType::ForegroundFullColor, ._params = {.ForegroundFullColor = params}}}};
    }

    static inline Instruction createBackgroundColor(const uint8_t color) {
        return {._type = InstrType::Style, ._params = {.Style = {._type = StyleInstrType::BackgroundColor, ._params = {.BackgroundColor = color}}}};
    }

    static inline Instruction createBackgroundFullColor(const FullColorParams& params) {
        return {._type = InstrType::Style, ._params = {.Style = {._type = StyleInstrType::BackgroundFullColor, ._params = {.BackgroundFullColor = params}}}};
    }

    static inline Instruction createBold(const bool boolean) {
        return {._type = InstrType::Style, ._params = {.Style = {._type = StyleInstrType::Bold, ._params = {.Bold = boolean}}}};
    }

    static inline Instruction createItalic(const bool boolean) {
        return {._type = InstrType::Style, ._params = {.Style = {._type = StyleInstrType::Italic, ._params = {.Italic = boolean}}}};
    }

    static inline Instruction createUnderlined(const bool boolean) {
        return {._type = InstrType::Style, ._params = {.Style = {._type = StyleInstrType::Underlined, ._params = {.Underlined = boolean}}}};
    }

    static inline Instruction createBlinking(const bool boolean) {
        return {._type = InstrType::Style, ._params = {.Style = {._type = StyleInstrType::Blinking, ._params = {.Blinking = boolean}}}};
    }

    static inline Instruction createReversed(const bool boolean) {
        return {._type = InstrType::Style, ._params = {.Style = {._type = StyleInstrType::Reversed, ._params = {.Reversed = boolean}}}};
    }

    static inline Instruction createStrikethrough(const bool boolean) {
        return {._type = InstrType::Style, ._params = {.Style = {._type = StyleInstrType::Strikethrough, ._params = {.Strikethrough = boolean}}}};
    }

    static inline Instruction createResetStyle() {
        return {._type = InstrType::Style, ._params = {.Style = {._type = StyleInstrType::ResetStyle}}};
    }

    static inline Instruction createSaveStyle() {
        return {._type = InstrType::Style, ._params = {.Style = {._type = StyleInstrType::SaveStyle}}};
    }

    static inline Instruction createRestoreStyle() {
        return {._type = InstrType::Style, ._params = {.Style = {._type = StyleInstrType::RestoreStyle}}};
    }
};

/**
 * @brief generates escape sequence and writes it to stdout_fd
 **/
#ifdef _DEBUG
template<TypeArrayList<Instruction> I, TypeArrayList<uint8_t> B>
static void submit(const I& instructions, B& buffer, const size_t out_fd, const size_t debug_out_fd) {
#else
template<TypeArrayList<Instruction> I, TypeArrayList<uint8_t> B>
static void submit(const I& instructions, B& buffer, const size_t out_fd) {
#endif
    for(size_t i = 0; i < instructions.len; i++) {
        const Instruction instr = instructions.ptr[i];

        switch(instr._type) {
            case InstrType::Reset: {
                callEnsureSpace(1, buffer, out_fd, debug_out_fd);

                buffer.appendAssume(RIS);
            } break;
            case InstrType::Tabstop: {
                callEnsureSpace(1, buffer, out_fd, debug_out_fd);

                buffer.appendAssume(HTS);
            } break;
            case InstrType::SetScrollingRegion: {
                const size_t top_digits = countDigits(instr._params.SetScrollingRegion._top);
                const size_t bottom_digits = countDigits(instr._params.SetScrollingRegion._bottom);

                callEnsureSpace(4 + top_digits + bottom_digits, buffer, out_fd, debug_out_fd);

                writeCSI(buffer);
                writeParameter(instr._params.SetScrollingRegion._top, buffer);
                writeSeparator(buffer);
                writeParameter(instr._params.SetScrollingRegion._bottom, buffer);
                buffer.appendAssume(DECSTBM);
            } break;
            case InstrType::ISOCharacterSet: {
                callEnsureSpace(3, buffer, out_fd, debug_out_fd);

                buffer.appendAssume(ESC);
                buffer.appendAssume('%');
                buffer.appendAssume('@');
            } break;
            case InstrType::UTF8CharacterSet: {
                callEnsureSpace(3, buffer, out_fd, debug_out_fd);

                buffer.appendAssume(ESC);
                buffer.appendAssume('%');
                buffer.appendAssume('G');
            } break;
            case InstrType::Cursor: {
                callSubmitCursor(instr._params.Cursor, buffer, out_fd, debug_out_fd);
            } break;
            case InstrType::Insert: {
                callSubmitInsert(instr._params.Insert, buffer, out_fd, debug_out_fd);
            } break;
            case InstrType::Erase: {
                callSubmitErase(instr._params.Erase, buffer, out_fd, debug_out_fd);
            } break;
            case InstrType::Palette: {
                callSubmitPalette(instr._params.Palette, buffer, out_fd, debug_out_fd);
            } break;
            case InstrType::Style: {
                callSubmitStyle(instr._params.Style, buffer, out_fd, debug_out_fd);
            } break;
        }
    }

    callFlushBuffer(buffer, out_fd, debug_out_fd);
}

} // namespace ControlSeq

} // namespace Tesix
