#pragma once

#include "codegen/state.hpp"

#include "output/stream.hpp"
#include "output/instruction.hpp"

#include "util/array.hpp"

#include <stdint.h>

#include "codegen/submit/style.hpp"

namespace Tesix {

namespace Codegen {

static void submitCursorPosition(
    Array<uint8_t>& out_buf,
    Array<Out::Instruction>& instr_buf,
    State& state,
    const Position& target,
    const uintmax_t fd
) {
    if(target == state._cursor_pos) {
        return;
    }

    if(target._x == 0) {
        if(target._y < state._cursor_pos._y) {
            Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createCursorPrecedingLine(state._cursor_pos._y - target._y), fd);
        } else {
            Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createCursorNextLine(target._y - state._cursor_pos._y), fd);
        }

        goto ret;
    }

    if(target._x == state._cursor_pos._x) {
        Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createCursorLineAbsolute(target._y), fd);

        goto ret;
    }

    if(target._y == state._cursor_pos._y) {
        Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createCursorCharacterAbsolute(target._x), fd);

        goto ret;
    }

    Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createCursorPositionAbsolute(target), fd);

ret:
    state._cursor_pos = target;
}

}

}
