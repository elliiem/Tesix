#pragma once

#include "../buffer.hpp"
#include "../linked_list.hpp"
#include "../space.hpp"
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
    Erase,
    Clear,
    ClearForwards,
    ClearBackwards,

    ChangeStyle,

    FillArea,
    ClearArea,
    MoveArea,
    DrawBuffer,
};

struct InsertCharParams {
    Position _pos;
    uint32_t _ch;
};

struct InsertStringParams {
    Position _pos;
    uint32_t* _str;
    size_t _len;
};

struct FillAreaParams {
    Box _area;
    uint32_t _ch;
};

struct MoveAreaParams {
    Position _pos;
    Box _area;
};

struct RepeatParams {
    Position _pos;
    uint32_t _ch;
    size_t _n;
};

union InstrParam {
    InsertCharParams InsertChar;
    InsertStringParams InsertString;
    FillAreaParams FillArea;
    MoveAreaParams MoveArea;
    Box ClearArea;
    RepeatParams Repeat;
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

    static inline Instruction createClearArea(const Box& area) {
        return {._type = InstrType::ClearArea, ._params = {.ClearArea = area}};
    }

    static inline Instruction createFillArea(const FillAreaParams& params) {
        return {._type = InstrType::FillArea, ._params = {.FillArea = params}};
    }
};

using InstrNode = Node<Instruction>*;
using InstrList = LinkedList<Instruction>;

struct State {
    Position _pos;
    uint64_t _style;
    uint32_t _last_ch;
    Buffer2D<uint32_t>& _screen_buffer;
    Box _term_area;
};

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
            .row = pos._x + 1,
            .column = pos._y + 1,
        })));
    }

    state._pos = pos;
}

static Node<Instruction>* expand(Node<Instruction>* instr, LinkedList<Instruction>& intermediate) {
    switch(instr->value._type) {
        case InstrType::ClearArea: {
            const auto& params = instr->value._params.ClearArea;

            LinkedList<Instruction> nodes;
            nodes.init();

            for(size_t line = params._pos._y; line < params.bottom(); line++) {
                nodes.append(Instruction::createRepeat({
                    ._pos = {params._pos._x, line},
                    ._ch = ' ',
                    ._n = params._width,
                }));
            }

            auto items = nodes.take();

            intermediate.emplaceNodesAtNode(items, instr);

            return items;
        } break;
        case InstrType::FillArea: {
            const auto& params = instr->value._params.FillArea;

            InstrList nodes;
            nodes.init();

            for(size_t line = params._area._pos._y; line < params._area.bottom(); line++) {
                nodes.append(Instruction::createRepeat({
                    ._pos = {params._area._pos._x, line},
                    ._ch = params._ch,
                    ._n = params._area._width,
                }));
            }

            auto items = nodes.take();

            intermediate.emplaceNodesAtNode(items, instr);

            return items;
        } break;
        case InstrType::MoveArea: {
        } break;
    }
}

static void submit(LinkedList<Instruction>& intermediate, ArrayList<ControlSeq::Instruction, Dynamic>& esc, State& state) {
    //  TODO: update Screen buffer

    Node<Instruction>* cur = intermediate.first();

    while(cur != nullptr) {
        const Instruction instr = cur->value;

        switch(cur->value._type) {
            case InstrType::InsertChar: {
                const InsertCharParams params = instr._params.InsertChar;

                const size_t utf8_str_len = UTF8::codepointLen(params._ch);

                uint8_t* utf8 = static_cast<uint8_t*>(malloc(utf8_str_len));

                UTF8::encodeCodepoint(params._ch, utf8);

                submitSetCursor(params._pos, esc, state);

                esc.append(ControlSeq::Instruction::createInsert(ControlSeq::InsertInstrucion::createInsertString({
                    .str = utf8,
                    .len = utf8_str_len,
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

                esc.append(ControlSeq::Instruction::createInsert(ControlSeq::InsertInstrucion::createInsertString({
                    .str = utf8,
                    .len = utf8_len,
                })));

                state._last_ch = params._str[params._len - 1];
                state._pos._x += params._len;
            } break;
            case InstrType::FillArea: {
                cur = expand(cur, intermediate);
                cur = cur->prev;
            } break;
            case InstrType::Repeat: {
                const RepeatParams params = instr._params.Repeat;

                submitSetCursor(params._pos, esc, state);

                if(state._last_ch != params._ch) {
                    const size_t utf8_len = UTF8::codepointLen(params._ch);
                    uint8_t* utf8 = static_cast<uint8_t*>(malloc(utf8_len));

                    UTF8::encodeCodepoint(params._ch, utf8);

                    esc.append(ControlSeq::Instruction::createInsert(ControlSeq::InsertInstrucion::createInsertString({
                        .str = utf8,
                        .len = utf8_len,
                    })));

                    state._last_ch = params._ch;

                    esc.append(ControlSeq::Instruction::createInsert(ControlSeq::InsertInstrucion::createRepeatChar(params._n - 1)));
                } else {
                    esc.append(ControlSeq::Instruction::createInsert(ControlSeq::InsertInstrucion::createRepeatChar(params._n)));
                }

                state._pos._x += params._n;
            }
        }

        cur = cur->next;
    }
}

static void resolveCharOnStringOverdraw(InstrNode drawer, InstrNode drawee, InstrList& intermediate) {
    const auto& drawer_instr = drawer->value;
    assert(drawer_instr._type == InstrType::InsertChar);
    const auto& drawer_params = drawer_instr._params.InsertChar;

    auto& drawee_instr = drawee->value;
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

        intermediate.insertAtNode(new_instr, drawee);
    }
}

static void resolveStringOnStringOverdraw(InstrNode drawer, InstrNode drawee, InstrList& intermediate) {
    const auto& drawer_instr = drawer->value;
    assert(drawer_instr._type == InstrType::InsertString);
    const auto& drawer_params = drawer_instr._params.InsertString;

    auto& drawee_instr = drawee->value;
    assert(drawer_instr._type == InstrType::InsertString);
    auto& drawee_params = drawee_instr._params.InsertString;

    const size_t drawer_end_i = (drawer_params._pos._x + drawer_params._len);
    const size_t drawee_end_i = (drawee_params._pos._x + drawee_params._len);

    if(drawer_params._pos._x <= drawee_params._pos._x) {
        if(drawer_end_i > drawee_end_i) {
            intermediate.eraseNode(drawee);
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

        intermediate.insertAtNode(new_instr, drawee);
    }
}

static void resolveCharOnAnyOverdraw(InstrNode drawer, InstrNode drawee, InstrList& intermediate) {
    const auto& drawee_instr = drawee->value;

    switch(drawee_instr._type) {
        case InstrType::InsertChar: {
            intermediate.eraseNode(drawee);
        } break;
        case InstrType::InsertString: {
            resolveCharOnStringOverdraw(drawer, drawee, intermediate);
        } break;
    }
}

static void resolveStringOnAnyOverdraw(InstrNode drawer, InstrNode drawee, InstrList& intermediate) {
    const auto& drawer_instr = drawer->value;
    assert(drawer_instr._type == InstrType::InsertString);
    const auto& drawer_params = drawer_instr._params.InsertString;

    const auto& drawee_instr = drawee->value;

    switch(drawee_instr._type) {
        case InstrType::InsertChar: {
            intermediate.eraseNode(drawee);
        } break;
        case InstrType::InsertString: {
            resolveStringOnStringOverdraw(drawer, drawee, intermediate);
        } break;
        case InstrType::ClearArea: {
            const auto& drawee_params = drawee_instr._params.ClearArea;

            InstrNode lines_front = expand(drawee, intermediate);

            InstrNode overdraw_line = getNodeForwards(lines_front, drawer_params._pos._y - drawer_params._pos._y);

            resolveStringOnStringOverdraw(drawer, overdraw_line, intermediate);
        } break;
    }
}

static void resolveOverdraw(InstrNode drawer, InstrNode drawee, InstrList& intermediate) {
    auto& drawer_instr = drawer->value;

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
