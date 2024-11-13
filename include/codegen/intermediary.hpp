#pragma once

#include "../linked_list.hpp"
#include "../space.hpp"
#include "../utf.hpp"
#include "control_sequences.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace Tesix {

namespace Intermediary {

enum class InstrType {
    Clear,
    ChangeStyle,
    InsertChar,
    InsertString,
    FillArea,
    MoveArea,
    ClearArea,
    Repeat,
};

struct InsertCharParams {
    Position pos;
    uint32_t ch;
};

struct InsertStringParams {
    Position pos;
    uint32_t* str;
    size_t len;
};

struct MoveAreaParams {
    Position pos;
    Area area;
};

struct RepeatParams {
    Position pos;
    size_t n;
};

union InstrParam {
    InsertCharParams InsertChar;
    InsertStringParams InsertString;
    Area FillArea;
    MoveAreaParams MoveArea;
    Area ClearArea;
    RepeatParams Repeat;
};

struct Instruction {
    InstrType type;
    InstrParam params;

    static inline Instruction createInsertChar(const InsertCharParams& params) {
        return {.type = InstrType::InsertChar, .params = {.InsertChar = params}};
    }
};

static Space instrSpace(const Instruction& instr, const struct Area& term) {
    switch(instr.type) {
        case InstrType::InsertChar: {
            const InsertCharParams& params = instr.params.InsertChar;

            return Space::createArea({.x = params.pos.x, .y = params.pos.y, .width = 1, .height = 1});
        } break;
        case InstrType::InsertString: {
            const InsertStringParams& params = instr.params.InsertString;

            const struct Area area = {.x = params.pos.x, .y = params.pos.y, .width = params.len, .height = 1};

            return Space::createSelection(Selection::fromArea(area, term));
        } break;
        case InstrType::FillArea: {
            const Area& area = instr.params.FillArea;

            return Space::createArea(area);
        } break;
        case InstrType::MoveArea: {
            const MoveAreaParams& params = instr.params.MoveArea;

            const Area& area = params.area;

            return Space::createArea({.x = params.pos.x, .y = params.pos.y, .width = area.width, .height = area.height});
        } break;
        case InstrType::ClearArea: {
            const Area& area = instr.params.ClearArea;

            return Space::createArea(area);
        }
        default: {
            return Space::createArea({.x = 0, .y = 0, .width = 0, .height = 0});
        } break;
    }
}

struct State {
    Position pos;
    uint64_t style;
};

static void submitSetCursor(Position pos, ArrayList<ControlSeq::Instruction, Dynamic>& esc, State& state) {
    if(state.pos.x == pos.x && state.pos.y == pos.y) {
        return;
    }

    if(state.pos.x == pos.x) {
        if(state.pos.x > pos.x) {
            esc.append(ControlSeq::Instruction::createCursor(ControlSeq::CursorInstruction::createCursorLeft(state.pos.x - pos.x + 1)));
        } else {
            esc.append(ControlSeq::Instruction::createCursor(ControlSeq::CursorInstruction::createCursorRight(pos.x - state.pos.x + 1)));
        }

    } else if(state.pos.y == pos.y) {
        if(pos.x == 0) {
            if(state.pos.y > pos.y) {
                esc.append(ControlSeq::Instruction::createCursor(ControlSeq::CursorInstruction::createCursorUpBegin(state.pos.y - pos.y + 1)));
            } else {
                esc.append(ControlSeq::Instruction::createCursor(ControlSeq::CursorInstruction::createCursorDownBegin(pos.y - state.pos.y + 1)));
            }
        } else if(state.pos.y > pos.y) {
            esc.append(ControlSeq::Instruction::createCursor(ControlSeq::CursorInstruction::createCursorUp(state.pos.y - pos.y + 1)));
        } else {
            esc.append(ControlSeq::Instruction::createCursor(ControlSeq::CursorInstruction::createCursorDown(pos.y - state.pos.y + 1)));
        }

    } else {
        esc.append(ControlSeq::Instruction::createCursor(ControlSeq::CursorInstruction::createCursorTo({
            .row = pos.x + 1,
            .column = pos.y + 1,
        })));
    }

    state.pos = pos;
}

static void submit(LinkedList<Instruction>& instructions, ArrayList<ControlSeq::Instruction, Dynamic>& esc, State& state) {
    Node<Instruction>* cur = instructions.first();

    while(cur != nullptr) {
        const Instruction instr = cur->value;

        switch(cur->value.type) {
            case InstrType::InsertChar: {
                const InsertCharParams params = instr.params.InsertChar;

                const size_t character_str_len = UTF8::codepointLen(params.ch);

                uint8_t* character_str = static_cast<uint8_t*>(malloc(character_str_len));

                UTF8::encodeCodepoint(params.ch, character_str);

                submitSetCursor(params.pos, esc, state);

                esc.append(ControlSeq::Instruction::createInsert(ControlSeq::InsertInstrucion::createInsertString({
                    .str = character_str,
                    .lenght = character_str_len,
                })));
            } break;
            case InstrType::InsertString: {
            } break;
            case InstrType::MoveArea: {
            } break;
        }

        cur = cur->next;
    }
}

/**
 * @brief expands complex instructions to basic ones
 **/
static void expand() {

}

} // namespace Intermediary

} // namespace Tesix
