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
};

struct InsertCharParams {
    size_t x;
    size_t y;
    uint32_t ch;
};

struct InsertStringParams {
    size_t x;
    size_t y;
    uint32_t* str;
    size_t len;
};

struct MoveAreaParams {
    size_t x;
    size_t y;
    Area area;
};

union InstrParam {
    InsertCharParams InsertChar;
    InsertStringParams InsertString;
    Area FillArea;
    MoveAreaParams MoveArea;
    Area ClearArea;
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

            return Space::createArea({.x = params.x, .y = params.y, .width = 1, .height = 1});
        } break;
        case InstrType::InsertString: {
            const InsertStringParams& params = instr.params.InsertString;

            const struct Area area = {.x = params.x, .y = params.y, .width = params.len, .height = 1};

            return Space::createSelection(Selection::fromArea(area, term));
        } break;
        case InstrType::FillArea: {
            const Area& area = instr.params.FillArea;

            return Space::createArea(area);
        } break;
        case InstrType::MoveArea: {
            const MoveAreaParams& params = instr.params.MoveArea;

            const Area& area = params.area;

            return Space::createArea({.x = params.x, .y = params.y, .width = area.width, .height = area.height});
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
    size_t x;
    size_t y;
    uint64_t style;
};

static void submitSetCursor(size_t x, size_t y, ArrayList<ControlSeq::Instruction, Dynamic>& esc) {
    esc.append({
        .type = ControlSeq::InstrType::Cursor,
        .params =
            {
                .Cursor =
                    {
                        .type = ControlSeq::CursorInstrType::CursorTo,
                        .params =
                            {
                                .CursorTo = {.row = y, .column = x},
                            },
                    },
            },
    });
}

static bool overdraws(Node<Instruction>* node, Node<Instruction>* overdrawer) {
}

static Node<Instruction>* getOverdrawer(Node<Instruction>* node) {
}

static void truncateOverdrawnDraw(Node<Instruction>* node, Node<Instruction>* overdrawer) {
    switch(node->value.type) {
        case InstrType::InsertChar: {
        } break;
    }
}

static void removeOverdraw(LinkedList<Instruction>& instructions) {
    Node<Instruction>* cur = instructions.last();

    ArrayList<Instruction, Dynamic> draws(5);

    while(cur != nullptr) {
        Node<Instruction>* overdrawer = getOverdrawer(cur);

        while(overdrawer != nullptr) {
        }

        cur = cur->prev;
    }
}

static void createEscapeSeqence(
    LinkedList<Instruction>& instructions, ArrayList<ControlSeq::Instruction, Dynamic>& esc, State& state) {
    Node<Instruction>* cur = instructions.first();

    while(cur != nullptr) {
        const Instruction instr = cur->value;

        switch(cur->value.type) {
            case InstrType::InsertChar: {
                const InsertCharParams params = instr.params.InsertChar;

                const size_t character_str_len = UTF8::codepointLen(params.ch);

                uint8_t* character_str = static_cast<uint8_t*>(malloc(character_str_len));

                UTF8::encodeCodepoint(params.ch, character_str);

                esc.append(ControlSeq::Instruction::createCursor(ControlSeq::CursorInstruction::createCursorTo({
                    .row = params.x  + 1,
                    .column = params.y + 1,
                })));

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

} // namespace Intermediary

} // namespace Tesix
