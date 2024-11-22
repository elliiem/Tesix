#pragma once

#include "../box.hpp"
#include "../buffer.hpp"
#include "../linked_list.hpp"
#include "../state.hpp"
#include "../style.hpp"
#include "../utf.hpp"
#include "control_sequences.hpp"

#include <cassert>
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
    ColorModifier,
};

struct InsertCharParams {
    Position _pos;
    uint32_t _ch;
    const uint64_t* _style;
};

struct InsertStringParams {
    Position _pos;
    uint32_t* _str;
    size_t _len;
    const uint64_t* _style;
};

struct FillAreaParams {
    FloatingBox _area;
    uint32_t _ch;
    const uint64_t* _style;
};

struct MoveAreaParams {
    FloatingBox _from;
    Position _to;
};

struct ClearAreaParams {
    FloatingBox _area;
    const uint64_t* _style;
};

struct RepeatParams {
    Position _pos;
    uint32_t _ch;
    size_t _n;
    const uint64_t* _style;
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
            esc.append(ControlSeq::Instruction::createCursor(ControlSeq::CursorInstruction::createCursorLeft(state._pos._x - pos._x)));
        } else {
            esc.append(ControlSeq::Instruction::createCursor(ControlSeq::CursorInstruction::createCursorRight(pos._x - state._pos._x)));
        }
    } else if(state._pos._x == pos._x) {
        if(state._pos._y > pos._y) {
            esc.append(ControlSeq::Instruction::createCursor(ControlSeq::CursorInstruction::createCursorUp(state._pos._y - pos._y)));
        } else {
            esc.append(ControlSeq::Instruction::createCursor(ControlSeq::CursorInstruction::createCursorDown(pos._y - state._pos._y)));
        }
    } else if(pos._x == 0) {
        if(state._pos._y > pos._y) {
            esc.append(ControlSeq::Instruction::createCursor(ControlSeq::CursorInstruction::createCursorUpBegin(state._pos._y - pos._y)));
        } else {
            esc.append(ControlSeq::Instruction::createCursor(ControlSeq::CursorInstruction::createCursorDownBegin(pos._y - state._pos._y)));
        }
    } else {
        esc.append(ControlSeq::Instruction::createCursor(ControlSeq::CursorInstruction::createCursorTo({
            ._row = pos._x + 1,
            ._column = pos._y + 1,
        })));
    }

    state._pos = pos;
}

static void submitStyleFCFMFromDefault(const Style::Style& target, ArrayList<ControlSeq::Instruction, Dynamic>& esc) {
    switch(target._fg.FCFM._tag) {
        case Style::ColorMode::Default: {
        } break;
        case Style::ColorMode::FullColor: {
            esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createForegroundFullColor({
                ._r = target._fg.FCFM._value.FC._r,
                ._g = target._fg.FCFM._value.FC._g,
                ._b = target._fg.FCFM._value.FC._b,
            })));
        } break;
        case Style::ColorMode::Palette: {
            esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createForegroundColor(target._fg.FCFM._value.P)));
        } break;
    }

    switch(target._bg.FCFM._tag) {
        case Style::ColorMode::Default: {
        } break;
        case Style::ColorMode::FullColor: {
            esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createForegroundFullColor({
                ._r = target._bg.FCFM._value.FC._r,
                ._g = target._bg.FCFM._value.FC._g,
                ._b = target._bg.FCFM._value.FC._b,
            })));
        } break;
        case Style::ColorMode::Palette: {
            esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createForegroundColor(target._bg.FCFM._value.P)));
        } break;
    }

    if(target._mod.FCFM._bold) {
        esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createBold(true)));
    }

    if(target._mod.FCFM._italic) {
        esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createItalic(true)));
    }

    if(target._mod.FCFM._underlined) {
        esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createUnderlined(true)));
    }

    if(target._mod.FCFM._blinking) {
        esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createBlinking(true)));
    }

    if(target._mod.FCFM._reverse) {
        esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createReversed(true)));
    }

    if(target._mod.FCFM._strikethrough) {
        esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createStrikethrough(true)));
    }
}

static void submitStyleFCFMToFCFM(const Style::Style& cur, const Style::Style& target, ArrayList<ControlSeq::Instruction, Dynamic>& esc) {
    switch(cur._fg.FCFM._tag) {
        case Style::ColorMode::Default: {
            switch(target._fg.FCFM._tag) {
                case Style::ColorMode::Default: {
                } break;
                case Style::ColorMode::FullColor: {
                    esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createForegroundFullColor({
                        ._r = target._fg.FCFM._value.FC._r,
                        ._g = target._fg.FCFM._value.FC._g,
                        ._b = target._fg.FCFM._value.FC._b,
                    })));
                }
                case Style::ColorMode::Palette: {
                    esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createForegroundColor(target._fg.FCFM._value.P)));
                }
            }
        } break;
        case Style::ColorMode::FullColor: {
            switch(target._fg.FCFM._tag) {
                case Style::ColorMode::Default: {
                    esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createResetStyle()));

                    submitStyleFCFMFromDefault(target, esc);
                } break;
                case Style::ColorMode::FullColor: {
                    if(cur._fg.FCFM._value.FC != target._fg.FCFM._value.FC) {
                        esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createForegroundFullColor({
                            ._r = target._fg.FCFM._value.FC._r,
                            ._g = target._fg.FCFM._value.FC._g,
                            ._b = target._fg.FCFM._value.FC._b,
                        })));
                    }
                } break;
                case Style::ColorMode::Palette: {
                    esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createForegroundColor(target._fg.FCFM._value.P)));
                } break;
            }
        } break;
        case Style::ColorMode::Palette: {
            switch(target._fg.FCFM._tag) {
                case Style::ColorMode::Default: {
                    esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createResetStyle()));

                    submitStyleFCFMFromDefault(target, esc);
                } break;
                case Style::ColorMode::FullColor: {
                    esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createForegroundFullColor({
                        ._r = target._fg.FCFM._value.FC._r,
                        ._g = target._fg.FCFM._value.FC._g,
                        ._b = target._fg.FCFM._value.FC._b,
                    })));
                } break;
                case Style::ColorMode::Palette: {
                    if(cur._fg.FCFM._value.P == target._fg.FCFM._value.P) {
                        esc.append(
                            ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createForegroundColor(target._fg.FCFM._value.P)));
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
                    esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createBackgroundFullColor({
                        ._r = target._bg.FCFM._value.FC._r,
                        ._g = target._bg.FCFM._value.FC._g,
                        ._b = target._bg.FCFM._value.FC._b,
                    })));
                } break;
                case Style::ColorMode::Palette: {
                    esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createBackgroundColor(target._bg.FCFM._value.P)));
                } break;
            }
        } break;
        case Style::ColorMode::FullColor: {
            switch(target._bg.FCFM._tag) {
                case Style::ColorMode::Default: {
                    esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createResetStyle()));

                    submitStyleFCFMFromDefault(target, esc);
                } break;
                case Style::ColorMode::FullColor: {
                    if(cur._bg.FCFM._value.FC == target._bg.FCFM._value.FC) {
                        esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createBackgroundFullColor({
                            ._r = target._bg.FCFM._value.FC._r,
                            ._g = target._bg.FCFM._value.FC._g,
                            ._b = target._bg.FCFM._value.FC._b,
                        })));
                    }
                } break;
                case Style::ColorMode::Palette: {
                    esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createBackgroundColor(target._bg.FCFM._value.P)));
                } break;
            }
        } break;
        case Style::ColorMode::Palette: {
            switch(target._bg.FCFM._tag) {
                case Style::ColorMode::Default: {
                    esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createResetStyle()));

                    submitStyleFCFMFromDefault(target, esc);
                } break;
                case Style::ColorMode::FullColor: {
                    esc.append(ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createBackgroundFullColor({
                        ._r = target._bg.FCFM._value.FC._r,
                        ._g = target._bg.FCFM._value.FC._g,
                        ._b = target._bg.FCFM._value.FC._b,
                    })));
                } break;
                case Style::ColorMode::Palette: {
                    if(cur._bg.FCFM._value.P == target._bg.FCFM._value.P) {
                        esc.append(
                            ControlSeq::Instruction::createStyle(ControlSeq::StyleInstruction::createBackgroundColor(target._bg.FCFM._value.P)));
                    }
                } break;
            }
        } break;
    }
}

static void submitStyle(uint64_t target_enc, ArrayList<ControlSeq::Instruction, Dynamic>& esc, State& state) {
    assert(state._style != nullptr);

    const auto cur = Style::Style::fromEncoding(*state._style);
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
                            ._style = cur_style,
                        }));

                        cur_style_start = x;
                        cur_style = ch_style;
                    }
                }

                instrs.append(Instruction::createInsertString({
                    ._pos = {params._pos._x + cur_style_start, params._pos._y + y},
                    ._str = params._contents._parent->_buffer + params._contents.index(cur_style_start, y),
                    ._len = params._contents._area._box._width - 1 - cur_style_start,
                    ._style = cur_style,
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
                submitStyle(*params._style, esc, state);

                esc.append(ControlSeq::Instruction::createInsert(ControlSeq::InsertInstrucion::createInsertString({
                    ._str = utf8,
                    ._len = utf8_str_len,
                })));

                state._last_ch = params._ch;
                state._pos._x += 1;
            } break;
            case InstrType::InsertString: {
                const InsertStringParams params = instr._params.InsertString;

                const size_t utf8_len = UTF8::codepointStringLen(params._str, params._len);
                uint8_t* utf8 = static_cast<uint8_t*>(malloc(utf8_len));

                UTF8::encodeCodepointStr(params._str, params._len, utf8);

                submitSetCursor(params._pos, esc, state);
                submitStyle(*params._style, esc, state);

                esc.append(ControlSeq::Instruction::createInsert(ControlSeq::InsertInstrucion::createInsertString({
                    ._str = utf8,
                    ._len = utf8_len,
                })));

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
                submitStyle(*params._style, esc, state);

                if(state._last_ch != params._ch) {
                    const size_t utf8_len = UTF8::codepointLen(params._ch);
                    uint8_t* utf8 = static_cast<uint8_t*>(malloc(utf8_len));

                    UTF8::encodeCodepoint(params._ch, utf8);

                    esc.append(ControlSeq::Instruction::createInsert(ControlSeq::InsertInstrucion::createInsertString({
                        ._str = utf8,
                        ._len = utf8_len,
                    })));

                    state._last_ch = params._ch;

                    esc.append(ControlSeq::Instruction::createInsert(ControlSeq::InsertInstrucion::createRepeatChar(params._n - 1)));
                } else {
                    esc.append(ControlSeq::Instruction::createInsert(ControlSeq::InsertInstrucion::createRepeatChar(params._n)));
                }

                state._pos._x += params._n;
            }
        }

        cur = cur->_next;
    }
}

static void resolveCharOnStringOverdraw(InstrNode drawer, InstrNode drawee, InstrList& intermediate) {
    const auto& drawer_instr = drawer->_value;
    assert(drawer_instr._type == InstrType::InsertChar);
    const auto& drawer_params = drawer_instr._params.InsertChar;

    auto& drawee_instr = drawee->_value;
    assert(drawee_instr._type == InstrType::InsertString);
    auto& drawee_params = drawee_instr._params.InsertString;

    assert(drawee_params._len > 0);

    if(drawer_params._pos._x == drawee_params._pos._x) {
        drawee_params._str += 1;
    } else if(drawer_params._pos._x == drawee_params._pos._x + drawee_params._len) {
        drawee_params._len -= 1;
    } else {
        const size_t index = drawer_params._pos._x - drawee_params._pos._x;

        const auto new_instr = Instruction::createInsertString({
            ._pos = {._x = drawee_params._pos._x + index + 1, ._y = drawee_params._pos._y},
            ._str = drawee_params._str + index + 1,
            ._len = drawee_params._len - index - 1,
        });

        drawee_params._len = index;

        intermediate.nodeAppend(new_instr, drawee);
    }
}

static void resolveStringOnStringOverdraw(InstrNode drawer, InstrNode drawee, InstrList& intermediate) {
    const auto& drawer_instr = drawer->_value;
    assert(drawer_instr._type == InstrType::InsertString);
    const auto& drawer_params = drawer_instr._params.InsertString;

    auto& drawee_instr = drawee->_value;
    assert(drawer_instr._type == InstrType::InsertString);
    auto& drawee_params = drawee_instr._params.InsertString;

    const size_t drawer_end_i = (drawer_params._pos._x + drawer_params._len);
    const size_t drawee_end_i = (drawee_params._pos._x + drawee_params._len);

    if(drawer_params._pos._x <= drawee_params._pos._x) {
        if(drawer_end_i > drawee_end_i) {
            intermediate.nodeErase(drawee);
        } else {
            const size_t n = drawer_end_i - drawee_params._pos._x;

            drawee_params._pos._x += n;
            drawee_params._str += n;
            drawee_params._len -= n;
        }
    } else if(drawer_end_i >= drawee_end_i) {
        drawee_params._len -= drawee_end_i - drawer_params._pos._x;
    } else {
        const size_t index = drawer_params._pos._x - drawee_params._pos._x;

        const auto new_instr = Instruction::createInsertString({
            ._pos = {._x = drawee_params._pos._x + index + 1, ._y = drawee_params._pos._y},
            ._str = drawee_params._str + index + drawer_params._len,
            ._len = drawee_params._len - (index + drawer_params._len),
        });

        drawee_params._len = index;

        intermediate.nodeAppend(new_instr, drawee);
    }
}

static void resolveCharOnAnyOverdraw(InstrNode drawer, InstrNode drawee, InstrList& intermediate) {
    const auto& drawee_instr = drawee->_value;

    switch(drawee_instr._type) {
        case InstrType::InsertChar: {
            intermediate.nodeErase(drawee);
        } break;
        case InstrType::InsertString: {
            resolveCharOnStringOverdraw(drawer, drawee, intermediate);
        } break;
    }
}

static void resolveStringOnAnyOverdraw(InstrNode drawer, InstrNode drawee, InstrList& intermediate) {
    const auto& drawer_instr = drawer->_value;
    assert(drawer_instr._type == InstrType::InsertString);
    const auto& drawer_params = drawer_instr._params.InsertString;

    const auto& drawee_instr = drawee->_value;

    switch(drawee_instr._type) {
        case InstrType::InsertChar: {
            intermediate.nodeErase(drawee);
        } break;
        case InstrType::InsertString: {
            resolveStringOnStringOverdraw(drawer, drawee, intermediate);
        } break;
        case InstrType::ClearArea: {
            const auto& drawee_params = drawee_instr._params.ClearArea;

            InstrNode lines_front = expand(drawee, intermediate)._front;

            InstrNode overdraw_line = getNodeForwards(lines_front, drawer_params._pos._y - drawer_params._pos._y);

            resolveStringOnStringOverdraw(drawer, overdraw_line, intermediate);
        } break;
    }
}

static void resolveOverdraw(InstrNode drawer, InstrNode drawee, InstrList& intermediate) {
    auto& drawer_instr = drawer->_value;

    switch(drawer_instr._type) {
        case InstrType::InsertChar: {
            resolveCharOnAnyOverdraw(drawer, drawee, intermediate);
        } break;
    }
}

static void optOverdraw(LinkedList<Instruction>& intermediate) {
}

} // namespace Intermediary

} // namespace Tesix
