#pragma once

#include "codegen/state.hpp"
#include "codegen/instruction.hpp"

#include "codegen/submit/common.hpp"

#include "codegen/expand/draw-buffer.hpp"
#include "codegen/expand/fill-area.hpp"

#include "output/instruction.hpp"

#include "util/linked-list.hpp"
#include "util/array.hpp"

#include <stdint.h>

namespace Tesix {

namespace Codegen {

static void submitInstruction(
    Array<uint8_t>& out_buf,
    Array<Out::Instruction>& instr_buf,
    State& state,
    const Instruction instr,
    const uintmax_t fd
);

static void submitInstructions(
    Array<uint8_t>& out_buf,
    Array<Out::Instruction>& instr_buf,
    State& state,
    const LinkedList<Instruction>& instrs,
    const uintmax_t fd
);

static void submitString(
    Array<uint8_t>& out_buf,
    Array<Out::Instruction>& instr_buf,
    State& state,
    const StringParams& params,
    const uintmax_t fd
) {
    if(params._area._len == 0) {
        return;
    }

    submitStyle(out_buf, instr_buf, state, params._style.value(), fd);
    submitCursorPosition(out_buf, instr_buf, state, params._area._pos, fd);

    const Out::Instruction out_instr = Out::Instruction::createString(Array<uint32_t>::fromRawFull(params._str, params._area._len));

    Out::streamInstruction(out_buf, instr_buf, out_instr, fd);

    state._cursor_pos._x += params._area._len;
    state._last_ch = params._str[params._area._len - 1];
}

static void submitRepeat(
    Array<uint8_t>& out_buf,
    Array<Out::Instruction>& instr_buf,
    State& state,
    const RepeatParams& params,
    const uintmax_t fd
) {
    if(params._area._len == 0) {
        return;
    }

    submitCursorPosition(out_buf, instr_buf, state, params._area._pos, fd);
    submitStyle(out_buf, instr_buf, state, params._style.value(), fd);

    if(params._ch == state._last_ch) {
        Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createRepeat(params._area._len), fd);

    } else {
        Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createCharacter(params._ch), fd);

        if(params._area._len > 1) {
            Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createRepeat(params._area._len - 1), fd);
        }
    }

    state._cursor_pos._x += params._area._len;
    state._last_ch = params._ch;
}

static void submitErase(
    Array<uint8_t>& out_buf,
    Array<Out::Instruction>& instr_buf,
    State& state,
    const EraseParams& params,
    const uintmax_t fd
) {
    submitStyle(out_buf, instr_buf, state, params._style.value(), fd);
    submitCursorPosition(out_buf, instr_buf, state, params._pos, fd);

    Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createEraseCharacters(params._n), fd);
}

static void submitEraseDisplay(
    Array<uint8_t>& out_buf,
    Array<Out::Instruction>& instr_buf,
    State& state,
    const EraseDisplayParams& params,
    const uintmax_t fd
) {
    submitStyle(out_buf, instr_buf, state, params._style.value(), fd);

    Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createEraseDisplay(), fd);
}

static void submitEraseDisplayForwards(
    Array<uint8_t>& out_buf,
    Array<Out::Instruction>& instr_buf,
    State& state,
    const EraseDisplayForwardsParams& params,
    const uintmax_t fd
) {
    submitStyle(out_buf, instr_buf, state, params._style.value(), fd);
    submitCursorPosition(out_buf, instr_buf, state, params._pos, fd);

    Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createEraseDisplayForwards(), fd);
}

static void submitEraseDisplayBackwards(
    Array<uint8_t>& out_buf,
    Array<Out::Instruction>& instr_buf,
    State& state,
    const EraseDisplayBackwardsParams& params,
    const uintmax_t fd
) {
    submitStyle(out_buf, instr_buf, state, params._style.value(), fd);
    submitCursorPosition(out_buf, instr_buf, state, params._pos, fd);

    Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createEraseDisplayBackwards(), fd);
}
static void submitEraseLine(
    Array<uint8_t>& out_buf,
    Array<Out::Instruction>& instr_buf,
    State& state,
    const EraseLineParams& params,
    const uintmax_t fd
) {
    submitStyle(out_buf, instr_buf, state, params._style.value(), fd);
    submitCursorPosition(out_buf, instr_buf, state, {._x = state._cursor_pos._x, ._y = params._y}, fd);

    Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createEraseLine(), fd);
}

static void submitEraseLineForwards(
    Array<uint8_t>& out_buf,
    Array<Out::Instruction>& instr_buf,
    State& state,
    const EraseLineForwardsParams& params,
    const uintmax_t fd
) {
    submitStyle(out_buf, instr_buf, state, params._style.value(), fd);
    submitCursorPosition(out_buf, instr_buf, state, params._pos, fd);

    Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createEraseLineForwards(), fd);
}

static void submitEraseLineBackwards(
    Array<uint8_t>& out_buf,
    Array<Out::Instruction>& instr_buf,
    State& state,
    const EraseLineBackwardsParams& params,
    const uintmax_t fd
) {
    submitStyle(out_buf, instr_buf, state, params._style.value(), fd);
    submitCursorPosition(out_buf, instr_buf, state, params._pos, fd);

    Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createEraseLineBackwards(), fd);
}

static void submitFillArea(
    Array<uint8_t>& out_buf,
    Array<Out::Instruction>& instr_buf,
    State& state,
    const FillAreaParams& params,
    const uintmax_t fd
) {
    auto instrs = expandFillArea(params);

    submitInstructions(out_buf, instr_buf, state, instrs, fd);

    instrs.free();
}

static void submitDrawBuffer(
    Array<uint8_t>& out_buf,
    Array<Out::Instruction>& instr_buf,
    State& state,
    const DrawBufferParams& params,
    const uintmax_t fd
) {
    auto instrs = expandDrawBuffer(params);

    submitInstructions(out_buf, instr_buf, state, instrs, fd);

    instrs.free();
}

static void submitInstruction(
    Array<uint8_t>& out_buf,
    Array<Out::Instruction>& instr_buf,
    State& state,
    const Instruction instr,
    const uintmax_t fd
) {
    switch(instr._type) {
        case InstructionE::String: {
            submitString(out_buf, instr_buf, state, instr._value.String, fd);
        } break;
        case InstructionE::Repeat: {
            submitRepeat(out_buf, instr_buf, state, instr._value.Repeat, fd);
        } break;
        case InstructionE::EraseDisplay: {
            submitEraseDisplay(out_buf, instr_buf, state, instr._value.EraseDisplay, fd);
        } break;
        case InstructionE::EraseDisplayForwards: {
            submitEraseDisplayForwards(out_buf, instr_buf, state, instr._value.EraseDisplayForwards, fd);
        } break;
        case InstructionE::EraseDisplayBackwards: {
            submitEraseDisplayBackwards(out_buf, instr_buf, state, instr._value.EraseDisplayBackwards, fd);
        } break;
        case InstructionE::FillArea: {
            submitFillArea(out_buf, instr_buf, state, instr._value.FillArea, fd);
        } break;
        case InstructionE::DrawBuffer: {
            submitDrawBuffer(out_buf, instr_buf, state, instr._value.DrawBuffer, fd);
        } break;

    }
}

static void submitInstructions(
    Array<uint8_t>& out_buf,
    Array<Out::Instruction>& instr_buf,
    State& state,
    const LinkedList<Instruction>& instrs,
    const uintmax_t fd
) {
    Node<Instruction>* cur = instrs._front;

    while(cur != nullptr) {
        submitInstruction(out_buf, instr_buf, state, cur->_value, fd);

        cur = cur->_next;
    }
}


}

}
