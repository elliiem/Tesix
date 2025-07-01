#pragma once

#include "codegen/state.hpp"

#include "output/stream.hpp"
#include "output/instruction.hpp"

#include "util/array.hpp"
#include "util/style.hpp"

#include <stdint.h>

namespace Tesix {

namespace Codegen {

static void submitStyleFCFMToFCFM(
    Array<uint8_t>& out_buf,
    Array<Out::Instruction>& instr_buf,
    const Style::Style& target,
    const Style::Style& current,
    const uintmax_t fd
) {
    bool is_reset = false;

    if(target._fg.FCFM._tag == Style::ColorMode::Default && current._fg.FCFM._tag != Style::ColorMode::Default) {
        Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createResetStyle(), fd);
        is_reset = true;
    } else if(target._bg.FCFM._tag == Style::ColorMode::Default && current._bg.FCFM._tag != Style::ColorMode::Default) {
        Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createResetStyle(), fd);
        is_reset = true;
    }

    switch(target._fg.FCFM._tag) {
        case Style::ColorMode::Default: {
        } break;
        case Style::ColorMode::Palette: {
            if(target._fg.FCFM._value.P != current._fg.FCFM._value.P || is_reset) {
                Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createColorForeground(target._fg.FCFM._value.P), fd);
            }
        } break;
        case Style::ColorMode::FullColor: {
            if(target._fg.FCFM._value.FC != current._fg.FCFM._value.FC || is_reset) {
                Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createColorForegroundFull(target._fg.FCFM._value.FC), fd);
            }
        } break;
    }

    switch(target._bg.FCFM._tag) {
        case Style::ColorMode::Default: {
        } break;
        case Style::ColorMode::Palette: {
            if(target._bg.FCFM._value.P != current._bg.FCFM._value.P || is_reset) {
                Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createColorBackground(target._bg.FCFM._value.P), fd);
            }
        } break;
        case Style::ColorMode::FullColor: {
            if(target._bg.FCFM._value.FC != current._bg.FCFM._value.FC || is_reset) {
                Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createColorBackgroundFull(target._bg.FCFM._value.FC.truncate()), fd);
            }
        } break;
    }

    if(target._mod.FCFM._bold != current._mod.FCFM._bold || (is_reset && target._mod.FCFM._bold)) {
        if(target._mod.FCFM._bold) {
            Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createBoldOn(), fd);
        } else {
            Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createBoldOff(), fd);
        }
    }

    if(target._mod.FCFM._italic != current._mod.FCFM._italic || (is_reset && target._mod.FCFM._italic)) {
        if(target._mod.FCFM._italic) {
            Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createItalicOn(), fd);
        } else {
            Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createItalicOff(), fd);
        }
    }

    if(target._mod.FCFM._underlined != current._mod.FCFM._underlined || (is_reset && target._mod.FCFM._underlined)) {
        if(target._mod.FCFM._underlined) {
            Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createUnderlinedOn(), fd);
        } else {
            Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createUnderlinedOff(), fd);
        }
    }

    if(target._mod.FCFM._blinking != current._mod.FCFM._blinking || (is_reset && target._mod.FCFM._blinking)) {
        if(target._mod.FCFM._blinking) {
            Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createBlinkingOn(), fd);
        } else {
            Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createBlinkingOff(), fd);
        }
    }

    if(target._mod.FCFM._reverse != current._mod.FCFM._reverse || (is_reset && target._mod.FCFM._reverse)) {
        if(target._mod.FCFM._reverse) {
            Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createReverseOn(), fd);
        } else {
            Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createReverseOff(), fd);
        }
    }

    if(target._mod.FCFM._strikethrough != current._mod.FCFM._strikethrough || (is_reset && target._mod.FCFM._strikethrough)) {
        if(target._mod.FCFM._strikethrough) {
            Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createStrikethroughOn(), fd);
        } else {
            Out::streamInstruction(out_buf, instr_buf, Out::Instruction::createStrikethroughOff(), fd);
        }
    }
}

static void submitStyle(
    Array<uint8_t>& out_buf,
    Array<Out::Instruction>& instr_buf,
    State& state,
    const uint64_t target_enc,
    const uintmax_t fd
) {
    if(target_enc == state._style) {
        return;
    }

    const auto target = Style::Style::fromEncoding(target_enc);
    const auto current = Style::Style::fromEncoding(state._style);

    switch(current._tag) {
        case Style::StyleEncoding::FCFM: {
            switch(target._tag) {
                case Style::StyleEncoding::FCFM: {
                    submitStyleFCFMToFCFM(out_buf, instr_buf, target, current, fd);
                } break;
            }
        } break;
    }

    state._style = target_enc;
}

}

}
