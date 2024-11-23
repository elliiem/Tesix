#pragma once

#include "../box.hpp"
#include "../buffer.hpp"
#include "../linked_list.hpp"
#include "../state.hpp"
#include "../style.hpp"
#include "../utf.hpp"
#include "control_sequences.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

namespace Tesix {

namespace Intermediary {

enum class InstrType {
    InsertChar,
    InsertString,
    Repeat,
    FillArea,
    DrawBuffer,
    MoveArea,
    Erase,
    EraseArea,
    Clear,
    ClearFw,
    ClearBw,
    ClearLine,
    ClearLineFw,
    ClearLineBw,
    ClearArea,
    RestoreArea,
};

struct InsertCharParams {
    Position _pos;
    uint32_t _ch;
    StyleContainer _style;
};

struct InsertStringParams {
    Position _pos;
    uint32_t* _str;
    size_t _len;
    StyleContainer _style;
};

struct FillAreaParams {
    FloatingBox _area;
    uint32_t _ch;
    StyleContainer _style;
};

struct MoveAreaParams {
    FloatingBox _from;
    Position _to;
};

struct ClearAreaParams {
    FloatingBox _area;
    StyleContainer _style;
};

struct RepeatParams {
    Position _pos;
    uint32_t _ch;
    size_t _n;
    StyleContainer _style;
};

struct DrawBufferParams {
    Position _pos;
    SubBuffer2D<uint32_t> _contents;
    SubBuffer2D<const uint64_t*> _styles;
};

union InstrParam {
    InsertCharParams InsertChar;
    InsertStringParams InsertString;
    FillAreaParams FillArea;
    MoveAreaParams MoveArea;
    ClearAreaParams ClearArea;
    RepeatParams Repeat;
    DrawBufferParams DrawBuffer;
};

struct Instruction {
    InstrType _type;
    InstrParam _params;

    static inline Instruction createInsertChar(const InsertCharParams& params) {
        return {._type = InstrType::InsertChar, ._params = {.InsertChar = params}};
    }

    static inline Instruction createInsertString(const InsertStringParams& params) {
        return {._type = InstrType::InsertString, ._params = {.InsertString = params}};
    }

    static inline Instruction createRepeat(const RepeatParams& params) {
        return {._type = InstrType::Repeat, ._params = {.Repeat = params}};
    }

    static inline Instruction createClearArea(const ClearAreaParams& params) {
        return {._type = InstrType::ClearArea, ._params = {.ClearArea = params}};
    }

    static inline Instruction createFillArea(const FillAreaParams& params) {
        return {._type = InstrType::FillArea, ._params = {.FillArea = params}};
    }

    static inline Instruction createDrawBuffer(const DrawBufferParams& params) {
        return {._type = InstrType::DrawBuffer, ._params = {.DrawBuffer = params}};
    }

    static inline Instruction createMoveArea(const MoveAreaParams& params) {
        return {._type = InstrType::MoveArea, ._params = {.MoveArea = params}};
    }
};

using InstrNode = Node<Instruction>*;
using InstrList = LinkedList<Instruction>;

static Nodes<Instruction> optGlobString(InstrList& intermediate, Node<Instruction>* instr_node) {
    const auto& instr = instr_node->_value;

    assert(instr._type == InstrType::InsertString);

    const auto& params = instr._params.InsertString;

    assert(params._len > 0);

    InstrList instrs;
    instrs.init();

    uint32_t cur_ch = params._str[0];
    uintmax_t ch_run = 1;

    uintmax_t cur_str_start = 0;
    uintmax_t cur_ch_start = 0;

    for(uintmax_t i = 1; i < params._len; i++) {
        if(i == params._len - 1) {
            if(params._str[i] == cur_ch) {
                ch_run += 1;
            }

            if(ch_run >= 5) {
                if(cur_ch_start == cur_str_start) {
                    instrs.append(Instruction::createRepeat({
                        ._pos = {._x = params._pos._x + cur_ch_start, ._y = params._pos._y},
                        ._ch = cur_ch,
                        ._n = ch_run,
                        ._style = params._style,
                    }));
                } else {
                    instrs.append(Instruction::createInsertString({
                        ._pos = {._x = params._pos._x + cur_str_start, ._y = params._pos._y},
                        ._str = params._str + cur_str_start,
                        ._len = cur_ch_start - cur_str_start,
                        ._style = params._style,
                    }));

                    instrs.append(Instruction::createRepeat({
                        ._pos = {._x = params._pos._x + cur_ch_start, ._y = params._pos._y},
                        ._ch = cur_ch,
                        ._n = ch_run,
                        ._style = params._style,
                    }));
                }
            } else {
                instrs.append(Instruction::createInsertString({
                    ._pos = {._x = params._pos._x + cur_str_start, ._y = params._pos._y},
                    ._str = params._str + cur_str_start,
                    ._len = params._len - cur_str_start,
                    ._style = params._style,
                }));
            }

            break;
        }

        if(params._str[i] == cur_ch) {
            ch_run += 1;
        } else {
            if(ch_run >= 5) {
                if(cur_ch_start == cur_str_start) {
                    instrs.append(Instruction::createRepeat({
                        ._pos = {._x = params._pos._x + cur_ch_start, ._y = params._pos._y},
                        ._ch = cur_ch,
                        ._n = ch_run,
                        ._style = params._style,
                    }));

                    cur_str_start = i;
                } else {
                    instrs.append(Instruction::createInsertString({
                        ._pos = {._x = params._pos._x + cur_str_start, ._y = params._pos._y},
                        ._str = params._str + cur_str_start,
                        ._len = cur_ch_start - cur_str_start,
                        ._style = params._style,
                    }));

                    instrs.append(Instruction::createRepeat({
                        ._pos = {._x = params._pos._x + cur_ch_start, ._y = params._pos._y},
                        ._ch = cur_ch,
                        ._n = ch_run,
                        ._style = params._style,
                    }));
                }
            }

            cur_ch = params._str[i];
            ch_run = 1;

            cur_ch_start = i;
        }
    }

    auto items = instrs.takeItems();
    intermediate.nodeReplaceMulti(items, instr_node);
    return items;
}

static void submitSetCursor(Position pos, ArrayList<ControlSeq::Instruction, Dynamic>& esc, State& state) {
    if(state._pos._x == pos._x && state._pos._y == pos._y) {
        return;
    }

    if(state._pos._y == pos._y) {
        if(state._pos._x > pos._x) {
            esc.append(ControlSeq::Instruction::createCursorLeft(state._pos._x - pos._x));
        } else {
            esc.append(ControlSeq::Instruction::createCursorRight(pos._x - state._pos._x));
        }
    } else if(state._pos._x == pos._x) {
        if(state._pos._y > pos._y) {
            esc.append(ControlSeq::Instruction::createCursorUp(state._pos._y - pos._y));
        } else {
            esc.append(ControlSeq::Instruction::createCursorDown(pos._y - state._pos._y));
        }
    } else if(pos._x == 0) {
        if(state._pos._y > pos._y) {
            esc.append(ControlSeq::Instruction::createCursorUpBegin(state._pos._y - pos._y));
        } else {
            esc.append(ControlSeq::Instruction::createCursorDownBegin(pos._y - state._pos._y));
        }
    } else {
        esc.append(ControlSeq::Instruction::createCursorTo({
            ._row = pos._x + 1,
            ._column = pos._y + 1,
        }));
    }

    state._pos = pos;
}

static void submitStyleFCFMFromDefault(const Style::Style& target, ArrayList<ControlSeq::Instruction, Dynamic>& esc) {
    switch(target._fg.FCFM._tag) {
        case Style::ColorMode::Default: {
        } break;
        case Style::ColorMode::FullColor: {
            esc.append(ControlSeq::Instruction::createForegroundFullColor({
                ._r = target._fg.FCFM._value.FC._r,
                ._g = target._fg.FCFM._value.FC._g,
                ._b = target._fg.FCFM._value.FC._b,
            }));
        } break;
        case Style::ColorMode::Palette: {
            esc.append(ControlSeq::Instruction::createForegroundColor(target._fg.FCFM._value.P));
        } break;
    }

    switch(target._bg.FCFM._tag) {
        case Style::ColorMode::Default: {
        } break;
        case Style::ColorMode::FullColor: {
            esc.append(ControlSeq::Instruction::createForegroundFullColor({
                ._r = target._bg.FCFM._value.FC._r,
                ._g = target._bg.FCFM._value.FC._g,
                ._b = target._bg.FCFM._value.FC._b,
            }));
        } break;
        case Style::ColorMode::Palette: {
            esc.append(ControlSeq::Instruction::createForegroundColor(target._bg.FCFM._value.P));
        } break;
    }

    if(target._mod.FCFM._bold) {
        esc.append(ControlSeq::Instruction::createBold(true));
    }

    if(target._mod.FCFM._italic) {
        esc.append(ControlSeq::Instruction::createItalic(true));
    }

    if(target._mod.FCFM._underlined) {
        esc.append(ControlSeq::Instruction::createUnderlined(true));
    }

    if(target._mod.FCFM._blinking) {
        esc.append(ControlSeq::Instruction::createBlinking(true));
    }

    if(target._mod.FCFM._reversed) {
        esc.append(ControlSeq::Instruction::createReversed(true));
    }

    if(target._mod.FCFM._strikethrough) {
        esc.append(ControlSeq::Instruction::createStrikethrough(true));
    }
}

static void submitStyleFCFMToFCFM(const Style::Style& cur, const Style::Style& target, ArrayList<ControlSeq::Instruction, Dynamic>& esc) {
    switch(cur._fg.FCFM._tag) {
        case Style::ColorMode::Default: {
            switch(target._fg.FCFM._tag) {
                case Style::ColorMode::Default: {
                } break;
                case Style::ColorMode::FullColor: {
                    esc.append(ControlSeq::Instruction::createForegroundFullColor({
                        ._r = target._fg.FCFM._value.FC._r,
                        ._g = target._fg.FCFM._value.FC._g,
                        ._b = target._fg.FCFM._value.FC._b,
                    }));
                } break;
                case Style::ColorMode::Palette: {
                    esc.append(ControlSeq::Instruction::createForegroundColor(target._fg.FCFM._value.P));
                } break;
            }
        } break;
        case Style::ColorMode::FullColor: {
            switch(target._fg.FCFM._tag) {
                case Style::ColorMode::Default: {
                    esc.append(ControlSeq::Instruction::createResetStyle());

                    submitStyleFCFMFromDefault(target, esc);
                } break;
                case Style::ColorMode::FullColor: {
                    if(cur._fg.FCFM._value.FC != target._fg.FCFM._value.FC) {
                        esc.append(ControlSeq::Instruction::createForegroundFullColor({
                            ._r = target._fg.FCFM._value.FC._r,
                            ._g = target._fg.FCFM._value.FC._g,
                            ._b = target._fg.FCFM._value.FC._b,
                        }));
                    }
                } break;
                case Style::ColorMode::Palette: {
                    esc.append(ControlSeq::Instruction::createForegroundColor(target._fg.FCFM._value.P));
                } break;
            }
        } break;
        case Style::ColorMode::Palette: {
            switch(target._fg.FCFM._tag) {
                case Style::ColorMode::Default: {
                    esc.append(ControlSeq::Instruction::createResetStyle());

                    submitStyleFCFMFromDefault(target, esc);
                } break;
                case Style::ColorMode::FullColor: {
                    esc.append(ControlSeq::Instruction::createForegroundFullColor({
                        ._r = target._fg.FCFM._value.FC._r,
                        ._g = target._fg.FCFM._value.FC._g,
                        ._b = target._fg.FCFM._value.FC._b,
                    }));
                } break;
                case Style::ColorMode::Palette: {
                    if(cur._fg.FCFM._value.P == target._fg.FCFM._value.P) {
                        esc.append(ControlSeq::Instruction::createForegroundColor(target._fg.FCFM._value.P));
                    }
                } break;
            }
        } break;
    }

    switch(cur._bg.FCFM._tag) {
        case Style::ColorMode::Default: {
            switch(target._bg.FCFM._tag) {
                case Style::ColorMode::Default: {
                } break;
                case Style::ColorMode::FullColor: {
                    esc.append(ControlSeq::Instruction::createBackgroundFullColor({
                        ._r = target._bg.FCFM._value.FC._r,
                        ._g = target._bg.FCFM._value.FC._g,
                        ._b = target._bg.FCFM._value.FC._b,
                    }));
                } break;
                case Style::ColorMode::Palette: {
                    esc.append(ControlSeq::Instruction::createBackgroundColor(target._bg.FCFM._value.P));
                } break;
            }
        } break;
        case Style::ColorMode::FullColor: {
            switch(target._bg.FCFM._tag) {
                case Style::ColorMode::Default: {
                    esc.append(ControlSeq::Instruction::createResetStyle());

                    submitStyleFCFMFromDefault(target, esc);
                } break;
                case Style::ColorMode::FullColor: {
                    if(cur._bg.FCFM._value.FC == target._bg.FCFM._value.FC) {
                        esc.append(ControlSeq::Instruction::createBackgroundFullColor({
                            ._r = target._bg.FCFM._value.FC._r,
                            ._g = target._bg.FCFM._value.FC._g,
                            ._b = target._bg.FCFM._value.FC._b,
                        }));
                    }
                } break;
                case Style::ColorMode::Palette: {
                    esc.append(ControlSeq::Instruction::createBackgroundColor(target._bg.FCFM._value.P));
                } break;
            }
        } break;
        case Style::ColorMode::Palette: {
            switch(target._bg.FCFM._tag) {
                case Style::ColorMode::Default: {
                    esc.append(ControlSeq::Instruction::createResetStyle());

                    submitStyleFCFMFromDefault(target, esc);
                } break;
                case Style::ColorMode::FullColor: {
                    esc.append(ControlSeq::Instruction::createBackgroundFullColor({
                        ._r = target._bg.FCFM._value.FC._r,
                        ._g = target._bg.FCFM._value.FC._g,
                        ._b = target._bg.FCFM._value.FC._b,
                    }));
                } break;
                case Style::ColorMode::Palette: {
                    if(cur._bg.FCFM._value.P == target._bg.FCFM._value.P) {
                        esc.append(ControlSeq::Instruction::createBackgroundColor(target._bg.FCFM._value.P));
                    }
                } break;
            }
        } break;
    }

    if(cur._mod.FCFM._bold != target._mod.FCFM._bold) {
        esc.append(ControlSeq::Instruction::createBold(target._mod.FCFM._bold));
    }

    if(cur._mod.FCFM._italic != target._mod.FCFM._italic) {
        esc.append(ControlSeq::Instruction::createItalic(target._mod.FCFM._italic));
    }

    if(cur._mod.FCFM._underlined != target._mod.FCFM._underlined) {
        esc.append(ControlSeq::Instruction::createUnderlined(target._mod.FCFM._underlined));
    }

    if(cur._mod.FCFM._blinking != target._mod.FCFM._blinking) {
        esc.append(ControlSeq::Instruction::createBlinking(target._mod.FCFM._blinking));
    }

    if(cur._mod.FCFM._reversed != target._mod.FCFM._reversed) {
        esc.append(ControlSeq::Instruction::createReversed(target._mod.FCFM._reversed));
    }

    if(cur._mod.FCFM._strikethrough != target._mod.FCFM._strikethrough) {
        esc.append(ControlSeq::Instruction::createStrikethrough(target._mod.FCFM._strikethrough));
    }
}

static void submitStyle(uint64_t target_enc, ArrayList<ControlSeq::Instruction, Dynamic>& esc, State& state) {
    Style::Style cur;

    switch(state._style._tag) {
        case StyleContainerTag::Ptr: {
            assert(state._style._value.P != nullptr);

            cur = Style::Style::fromEncoding(*state._style._value.P);
        } break;
        case StyleContainerTag::Value: {
            cur = Style::Style::fromEncoding(state._style._value.V);
        }
    }
    const auto target = Style::Style::fromEncoding(target_enc);

    switch(cur._tag) {
        case Style::StyleEncoding::FCFM: {
            switch(target._tag) {
                case Style::StyleEncoding::FCFM: {
                    submitStyleFCFMToFCFM(cur, target, esc);
                } break;
            }
        } break;
    }
}

static Nodes<Instruction> expand(InstrNode instr_node, InstrList& intermediate) {
    auto& instr = instr_node->_value;

    switch(instr._type) {
        case InstrType::FillArea: {
            const auto& params = instr._params.FillArea;

            assert(params._area.takesUpSpace());

            InstrList nodes;
            nodes.init();

            for(size_t y = params._area._pos._y; y <= params._area.bottom(); y++) {
                nodes.append(Instruction::createRepeat({
                    ._pos = {._x = params._area._pos._x, ._y = y},
                    ._ch = params._ch,
                    ._n = params._area._box._width,
                    ._style = params._style,
                }));
            }

            auto items = nodes.takeItems();
            intermediate.nodeReplaceMulti(items, instr_node);
            return items;
        } break;
        case InstrType::DrawBuffer: {
            auto& params = instr._params.DrawBuffer;

            assert(params._contents._area.takesUpSpace());

            InstrList instrs;
            instrs.init();

            for(uintmax_t y = 0; y < params._contents._area._box._height; y++) {
                uintmax_t cur_style_start = 0;
                const uint64_t*& cur_style = params._styles.at(0, y);

                for(uintmax_t x = 1; x < params._contents._area._box._width; x++) {
                    const uint64_t*& ch_style = params._styles.at(x, y);

                    if(ch_style != cur_style) {
                        instrs.append(Instruction::createInsertString({
                            ._pos = {params._pos._x + cur_style_start, params._pos._y + y},
                            ._str = params._contents._parent->_buffer + params._contents.index(cur_style_start, y),
                            ._len = x - cur_style_start,
                            ._style = StyleContainer::createPtr(cur_style),
                        }));

                        cur_style_start = x;
                        cur_style = ch_style;
                    }
                }

                instrs.append(Instruction::createInsertString({
                    ._pos = {params._pos._x + cur_style_start, params._pos._y + y},
                    ._str = params._contents._parent->_buffer + params._contents.index(cur_style_start, y),
                    ._len = params._contents._area._box._width - 1 - cur_style_start,
                    ._style = StyleContainer::createPtr(cur_style),
                }));
            }

            auto items = instrs.takeItems();
            intermediate.nodeReplaceMulti(items, instr_node);
            return items;
        } break;
        case InstrType::ClearArea: {
            const auto& params = instr._params.ClearArea;

            assert(params._area.takesUpSpace());

            LinkedList<Instruction> nodes;
            nodes.init();

            for(size_t line = params._area._pos._y; line < params._area.bottom(); line++) {
                nodes.append(Instruction::createRepeat({
                    ._pos = {params._area._pos._x, line},
                    ._ch = ' ',
                    ._n = params._area._box._width,
                    ._style = params._style,
                }));
            }

            auto items = nodes.takeItems();
            intermediate.nodeReplaceMulti(items, instr_node);
            return items;
        } break;
    }
}

static uint64_t applyColorModifier(uint64_t style_enc, uint64_t mod_enc) {
    const auto style = Style::Style::fromEncoding(style_enc);
    const auto mod = Style::Style::fromEncoding(mod_enc);

    switch(style._tag) {
        case Style::StyleEncoding::FCFM: {
            switch(mod._tag) {
                case Style::StyleEncoding::FCFM: {
                    assert(style._bg.FCFM._tag == Style::ColorMode::FullColor);
                    assert(mod._bg.FCFM._tag == Style::ColorMode::FullColor);

                    auto modified = style;

                    const float alpha = static_cast<double_t>(style._bg.FCFM._value.FC._a) / 15.0;

                    const uint8_t style_bg_part_r = static_cast<uint8_t>(style._bg.FCFM._value.FC._r * alpha);
                    const uint8_t style_bg_part_g = static_cast<uint8_t>(style._bg.FCFM._value.FC._g * alpha);
                    const uint8_t style_bg_part_b = static_cast<uint8_t>(style._bg.FCFM._value.FC._b * alpha);

                    const uint8_t mod_bg_part_r = static_cast<uint8_t>(mod._bg.FCFM._value.FC._r * (1 - alpha));
                    const uint8_t mod_bg_part_g = static_cast<uint8_t>(mod._bg.FCFM._value.FC._g * (1 - alpha));
                    const uint8_t mod_bg_part_b = static_cast<uint8_t>(mod._bg.FCFM._value.FC._b * (1 - alpha));

                    modified.bgFullColor({
                        ._r = static_cast<uint8_t>(style_bg_part_r + mod_bg_part_r),
                        ._g = static_cast<uint8_t>(style_bg_part_g + mod_bg_part_g),
                        ._b = static_cast<uint8_t>(style_bg_part_b + mod_bg_part_b),
                        ._a = 15,
                    });

                    return modified.toEncoding();
                }
            }
        }
    }
}

static FloatingBox insertStringArea(const InsertStringParams& params) {
    return {._pos = params._pos, ._box = {._width = params._len, ._height = 1}};
}

static void resolveStringOnStringOverdraw(InstrNode top_node, InstrNode bottom_node, InstrList& instrs) {
    const auto& top_instr = top_node->_value;
    assert(top_instr._type == InstrType::InsertString);
    const auto& top_params = top_instr._params.InsertString;

    const auto& bottom_instr = bottom_node->_value;
    assert(bottom_instr._type == InstrType::InsertString);
    const auto& bottom_params = bottom_instr._params.InsertString;

    assert(overlap(insertStringArea(top_params), insertStringArea(bottom_params)));

    InstrList top_instrs;
    top_instrs.init();

    InstrList bottom_instrs;
    bottom_instrs.init();

    const auto top_style = Style::Style::fromEncoding(top_params._style.value());

    const uintmax_t top_end = top_params._pos._x + top_params._len;
    const uintmax_t bottom_end = bottom_params._pos._x + bottom_params._len;

    switch(top_style._tag) {
        case Style::StyleEncoding::FCFM: {
            if(top_style._bg.FCFM._tag == Style::ColorMode::FullColor && top_style._bg.FCFM._value.FC._a < 15) {
                if(top_params._pos._x < bottom_params._pos._x) {
                    if(top_end > bottom_end) {
                        top_instrs.append(Instruction::createInsertString({
                            ._pos = top_params._pos,
                            ._str = top_params._str,
                            ._len = bottom_params._pos._x - top_params._pos._x,
                            ._style = top_params._style,
                        }));

                        top_instrs.append(Instruction::createInsertString({
                            ._pos = bottom_params._pos,
                            ._str = top_params._str + bottom_params._pos._x - top_params._pos._x,
                            ._len = bottom_params._len,
                            ._style = StyleContainer::createValue(applyColorModifier(top_params._style.value(), bottom_params._style.value())),
                        }));

                        top_instrs.append(Instruction::createInsertString({
                            ._pos = {bottom_end, top_params._pos._y},
                            ._str = top_params._str + top_end - bottom_end,
                            ._len = top_end - bottom_end,
                            ._style = top_params._style,
                        }));

                        instrs.nodeErase(bottom_node);
                    } else {
                        top_instrs.append(Instruction::createInsertString({
                            ._pos = top_params._pos,
                            ._str = top_params._str,
                            ._len = bottom_params._pos._x - top_params._pos._x,
                            ._style = top_params._style,
                        }));

                        top_instrs.append(Instruction::createInsertString({
                            ._pos = bottom_params._pos,
                            ._str = top_params._str + bottom_params._pos._x - top_params._pos._x,
                            ._len = top_end - bottom_params._pos._x,
                            ._style = StyleContainer::createValue(applyColorModifier(top_params._style.value(), bottom_params._style.value())),
                        }));

                        bottom_instrs.append(Instruction::createInsertString({
                            ._pos = {top_end, bottom_params._pos._y},
                            ._str = bottom_params._str + top_end - bottom_params._pos._x,
                            ._len = bottom_end - top_end,
                            ._style = bottom_params._style,
                        }));
                    }
                } else {
                    if(top_end < bottom_end) {
                        bottom_instrs.append(Instruction::createInsertString({
                            ._pos = bottom_params._pos,
                            ._str = bottom_params._str,
                            ._len = top_params._pos._x - bottom_params._pos._x,
                            ._style = bottom_params._style,
                        }));

                        top_instrs.append(Instruction::createInsertString({
                            ._pos = top_params._pos,
                            ._str = top_params._str,
                            ._len = top_params._len,
                            ._style = StyleContainer::createValue(applyColorModifier(top_params._style.value(), bottom_params._style.value())),
                        }));

                        bottom_instrs.append(Instruction::createInsertString({
                            ._pos = {top_end, bottom_params._pos._y},
                            ._str = bottom_params._str + top_end - bottom_params._pos._x,
                            ._len = bottom_end - top_end,
                            ._style = bottom_params._style,
                        }));
                    } else {
                        bottom_instrs.append(Instruction::createInsertString({
                            ._pos = bottom_params._pos,
                            ._str = bottom_params._str,
                            ._len = top_params._pos._x - bottom_params._pos._x,
                            ._style = bottom_params._style,
                        }));

                        top_instrs.append(Instruction::createInsertString({
                            ._pos = top_params._pos,
                            ._str = top_params._str,
                            ._len = bottom_end - top_params._pos._x,
                            ._style = StyleContainer::createValue(applyColorModifier(top_params._style.value(), bottom_params._style.value())),
                        }));

                        top_instrs.append(Instruction::createInsertString({
                            ._pos = {bottom_end, top_params._pos._y},
                            ._str = top_params._str + bottom_end - top_params._pos._x,
                            ._len = top_end - bottom_end,
                            ._style = top_params._style,
                        }));
                    }
                }
            } else {
                if(top_params._pos._x < bottom_params._pos._x) {
                    if(top_end > bottom_end) {
                        instrs.nodeErase(bottom_node);
                        return;
                    }
                } else {
                    if(top_end < bottom_end) {

                    }
                }
            }
        } break;
    }

    if(top_instrs._len > 0) {
        auto top_items = top_instrs.takeItems();
        instrs.nodeReplaceMulti(top_items, top_node);
    }

    if(bottom_instrs._len > 0) {
        auto bottom_items = bottom_instrs.takeItems();
        instrs.nodeReplaceMulti(bottom_items, bottom_node);
    }
    // front
    // middle
    // back
}

static void submit(LinkedList<Instruction>& intermediate, ArrayList<ControlSeq::Instruction, Dynamic>& esc, State& state) {
    //  TODO: update Screen buffer

    Node<Instruction>* cur = intermediate.first();

    while(cur != nullptr) {
        const Instruction instr = cur->_value;

        switch(cur->_value._type) {
            case InstrType::InsertChar: {
                const InsertCharParams params = instr._params.InsertChar;

                const size_t utf8_str_len = UTF8::codepointLen(params._ch);

                uint8_t* utf8 = static_cast<uint8_t*>(malloc(utf8_str_len));

                UTF8::encodeCodepoint(params._ch, utf8);

                submitSetCursor(params._pos, esc, state);
                submitStyle(params._style.value(), esc, state);

                esc.append(ControlSeq::Instruction::createInsertString({
                    ._str = utf8,
                    ._len = utf8_str_len,
                }));

                state._last_ch = params._ch;
                state._pos._x += 1;
            } break;
            case InstrType::InsertString: {
                const InsertStringParams params = instr._params.InsertString;

                const size_t utf8_len = UTF8::codepointStringLen(params._str, params._len);
                uint8_t* utf8 = static_cast<uint8_t*>(malloc(utf8_len));

                UTF8::encodeCodepointStr(params._str, params._len, utf8);

                submitSetCursor(params._pos, esc, state);
                submitStyle(params._style.value(), esc, state);

                esc.append(ControlSeq::Instruction::createInsertString({
                    ._str = utf8,
                    ._len = utf8_len,
                }));

                state._last_ch = params._str[params._len - 1];
                state._pos._x += params._len;
            } break;
            case InstrType::FillArea: {
                cur = expand(cur, intermediate)._front;
                cur = cur->_prev;
            } break;
            case InstrType::DrawBuffer: {
                cur = expand(cur, intermediate)._front;
                cur = cur->_prev;
            } break;
            case InstrType::MoveArea: {
                cur = expand(cur, intermediate)._front;
                cur = cur->_prev;
            } break;
            case InstrType::Repeat: {
                const RepeatParams params = instr._params.Repeat;

                submitSetCursor(params._pos, esc, state);
                submitStyle(params._style.value(), esc, state);

                if(state._last_ch != params._ch) {
                    const size_t utf8_len = UTF8::codepointLen(params._ch);
                    uint8_t* utf8 = static_cast<uint8_t*>(malloc(utf8_len));

                    UTF8::encodeCodepoint(params._ch, utf8);

                    esc.append(ControlSeq::Instruction::createInsertString({
                        ._str = utf8,
                        ._len = utf8_len,
                    }));

                    state._last_ch = params._ch;

                    esc.append(ControlSeq::Instruction::createRepeatChar(params._n - 1));
                } else {
                    esc.append(ControlSeq::Instruction::createRepeatChar(params._n));
                }

                state._pos._x += params._n;
            }
        }

        cur = cur->_next;
    }
}

} // namespace Intermediary

} // namespace Tesix
