#pragma once

#include "util/buffer/styled-buffer.hpp"
#include "util/style.hpp"
#include "util/space.hpp"
#include "util/utf.hpp"
#include "util/string.hpp"

#include <stdint.h>

namespace Tesix {

namespace Draw {

static void drawCharacter(
    StyledBuffer& buf,
    const uint32_t ch,
    const Style::StyleContainer& style,
    const Position& pos
) {
    buf._ch.at(pos) = ch;
    buf._style.at(pos) = style;
}

static void drawCharacter(
    StyledBufferArea&& buf,
    const uint32_t ch,
    const Style::StyleContainer& style,
    const Position& pos
) {
    buf._ch.at(pos) = ch;
    buf._style.at(pos) = style;
}

static void drawCharacter(
    StyledBufferArea& buf,
    const uint32_t ch,
    const Style::StyleContainer& style,
    const Position& pos
) {
    buf._ch.at(pos) = ch;
    buf._style.at(pos) = style;
}

static void drawString(
    StyledBuffer& buf,
    const uint8_t* const utf8,
    const uintmax_t utf8_c,
    const Style::StyleContainer& style,
    const Position& pos
) {
    auto utf32 = UTF8::toUTF32(utf8, utf8_c);

    for(uintmax_t i = 0; i < utf32._n; i++) {
        drawCharacter(buf, utf32._ptr[i], style, pos + Position::create(i, 0));
    }

    utf32.free();
}

static void drawString(
    StyledBufferArea& buf,
    const uint8_t* const utf8,
    const uintmax_t utf8_c,
    const Style::StyleContainer& style,
    const Position& pos
) {
    auto utf32 = UTF8::toUTF32(utf8, utf8_c);

    for(uintmax_t i = 0; i < utf32._n; i++) {
        drawCharacter(buf, utf32._ptr[i], style, pos + Position::create(i, 0));
    }

    utf32.free();
}

static void drawString(
    StyledBufferArea&& buf,
    const char* const utf8,
    const Style::StyleContainer& style,
    const Position& pos
) {
    auto utf32 = UTF8::toUTF32(asByteStr(utf8), strlen(utf8));

    for(uintmax_t i = 0; i < utf32._n; i++) {
        drawCharacter(buf, utf32._ptr[i], style, pos + Position::create(i, 0));
    }

    utf32.free();
}

static void fill(
    StyledBuffer& buf,
    const uint32_t ch,
    const Style::StyleContainer& style
) {
    for(uintmax_t y = 0; y < buf._ch._box._height; y++) {
        for(uintmax_t x = 0; x < buf._ch._box._width; x++) {
            drawCharacter(buf, ch, style, Position::create(x, y));
        }
    }
}

static void fill(
    StyledBufferArea&& buf,
    const uint32_t ch,
    const Style::StyleContainer& style
) {
    for(uintmax_t y = 0; y < buf._ch._area._box._height; y++) {
        for(uintmax_t x = 0; x < buf._ch._area._box._width; x++) {
            drawCharacter(buf, ch, style, Position::create(x, y));
        }
    }
}

static void fill(
    StyledBufferArea& buf,
    const uint32_t ch,
    const Style::StyleContainer& style
) {
    for(uintmax_t y = 0; y < buf._ch._area._box._height; y++) {
        for(uintmax_t x = 0; x < buf._ch._area._box._width; x++) {
            drawCharacter(buf, ch, style, Position::create(x, y));
        }
    }
}


}

}
