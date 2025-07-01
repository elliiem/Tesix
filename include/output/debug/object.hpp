#pragma once

#include "output/debug/symbols.hpp"
#include "output/common.hpp"

#include "util/box.hpp"
#include "util/string.hpp"

#include <inttypes.h>

namespace Tesix {

namespace Debug {

consteval static uintmax_t countObjBegin() {
    return cstrlen(OBJ_BEGIN);
}

constexpr static inline uintmax_t countObjEnd(
    const uintmax_t indent,
    const uintmax_t indent_width
) {
    return (indent_width * indent) + cstrlen(OBJ_END);
}

constexpr static inline uintmax_t countDeclaration(
    const uintmax_t name_c,
    const uintmax_t indent,
    const uintmax_t indent_width
) {
    return (indent_width * indent) + name_c + cstrlen(DECLARATION_EQL);
}

template <typename T>
constexpr static inline uintmax_t countField(
    const T& val,
    const uintmax_t name_c,
    const uintmax_t indent,
    const uintmax_t indent_width
);

template <>
constexpr inline uintmax_t countField(
    const uintmax_t& val,
    const uintmax_t name_c,
    const uintmax_t indent,
    const uintmax_t indent_width
) {
    return countDeclaration(name_c, indent, indent_width) + countDigits(val) + cstrlen(FIELD_TERMINATOR);
}

template<>
constexpr inline uintmax_t countField(
    const Area::Position& val,
    const uintmax_t name_c,
    const uintmax_t indent,
    const uintmax_t indent_width
) {
    constexpr const char* const field_x_name = "_x";
    constexpr const char* const field_y_name = "_y";

    const uintmax_t field_dec_c = countDeclaration(name_c, indent, indent_width);
    const uintmax_t field_x_c = countField(val._x, cstrlen(field_x_name), indent, indent_width);
    const uintmax_t field_y_c = countField(val._x, cstrlen(field_y_name), indent, indent_width);

    return field_dec_c + countObjBegin() + field_x_c + field_y_c + countObjEnd(indent, indent_width);
}

template<typename T>
static inline uintmax_t countObj(
    const T& val,
    const uintmax_t indent,
    const uintmax_t indent_width
);

template<>
inline uintmax_t countObj(
    const uintmax_t& val,
    const uintmax_t indent,
    const uintmax_t indent_width
) {
    return countDigits(val) + cstrlen(FIELD_TERMINATOR);
}

template<>
inline uintmax_t countObj(
    const Area::Position& val,
    const uintmax_t indent,
    const uintmax_t indent_width
) {
    static constexpr const char* const field_x_name = "_x";
    static constexpr const char* const field_y_name = "_y";

    const uintmax_t field_x_c = countField(val._x, cstrlen(field_x_name), indent + 1,  indent_width);
    const uintmax_t field_y_c = countField(val._y, cstrlen(field_y_name), indent + 1, indent_width);

    return countObjBegin() + field_x_c + field_y_c + countObjEnd(indent, indent_width);
}

static uint8_t* const writeIndent(
    uint8_t* const buf,
    const uintmax_t indent,
    const uintmax_t indent_width
) {
    return writeByteMulti(buf, ' ', (indent_width * indent));
}

static uint8_t* const writeObjBegin(
    uint8_t* const buf
) {
    return writeBytes(buf, asByteStr(OBJ_BEGIN), cstrlen(OBJ_BEGIN));
}

static uint8_t* const writeObjEnd(
    uint8_t* const buf,
    const uintmax_t indent,
    const uintmax_t indent_width
) {
    auto buf_rem = buf;

    buf_rem = writeIndent(buf_rem, indent, indent_width);
    buf_rem = writeBytes(buf_rem, asByteStr(OBJ_END), cstrlen(OBJ_END));

    return buf_rem;
}

static uint8_t* const writeFieldDeclarationEql(
    uint8_t* const buf
) {
    return writeBytes(buf, asByteStr(DECLARATION_EQL), cstrlen(DECLARATION_EQL));
}


// writes the contents of a member declaration to buf and returns the offset
// buffer
static uint8_t* const writeFieldDeclaration(
    uint8_t* const buf,
    const uint8_t* const name,
    const uintmax_t name_c,
    const uintmax_t indent,
    const uintmax_t indent_width
) {
    auto buf_rem = buf;

    buf_rem = writeIndent(buf_rem, indent, indent_width);
    buf_rem = writeBytes(buf_rem, name, name_c);
    buf_rem = writeFieldDeclarationEql(buf_rem);

    return buf_rem;
}


// writes the contents of a member to buf and returns the offset buffer
template <typename T>
static uint8_t* const writeObj(
    uint8_t* const buf,
    const T& val,
    const uintmax_t indent,
    const uintmax_t indent_width
);

// writes the contents of a member to buf and returns the offset buffer
template <std::unsigned_integral T>
uint8_t* const writeObj(
    uint8_t* const buf,
    const T& val,
    const uintmax_t indent,
    const uintmax_t indent_width
) {
    auto buf_rem = buf;

    buf_rem += sprintf(asCStr(buf_rem), "%lu", val);
    buf_rem = writeBytes(buf_rem, asByteStr(FIELD_TERMINATOR), cstrlen(FIELD_TERMINATOR));

    return buf_rem;
}

template <>
uint8_t* const writeObj(
    uint8_t* const buf,
    const Area::Position& val,
    const uintmax_t indent,
    const uintmax_t indent_width
) {
    static constexpr const char* const field_x_name = "_x";
    static constexpr const char* const field_y_name = "_y";

    uint8_t* buf_rem = buf;

    buf_rem = writeObjBegin(buf);
    buf_rem = writeFieldDeclaration(buf_rem, asByteStr(field_x_name), cstrlen(field_x_name), indent + 1, indent_width);
    buf_rem = writeObj(buf_rem, val._x, indent, indent_width);
    buf_rem = writeFieldDeclaration(buf_rem, asByteStr(field_y_name), cstrlen(field_y_name), indent + 1, indent_width);
    buf_rem = writeObj(buf_rem, val._y, indent, indent_width);

    buf_rem = writeObjEnd(buf_rem, indent, indent_width);

    return buf_rem;
}

}

}

