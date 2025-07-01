#pragma once

#include "codegen/instruction.hpp"

#include "util/linked-list.hpp"

namespace Tesix {

namespace Codegen {

static LinkedList<Instruction> optStringRepeat(
    const StringParams& params
) {
    auto instrs = LinkedList<Instruction>::init();

    assert(params._area._len > 0);

    uintmax_t str_start = 0;
    uintmax_t rep_start = 0;
    uint32_t rep_ch = params._str[0];
    uintmax_t run = 1;

    for(uintmax_t i = 1; i < params._area._len; i++) {
        const uint32_t ch = params._str[i];

        if(rep_ch != ch) {
            if(run > 5) {
                if(str_start != rep_start) {
                    instrs.append(Instruction::createString({
                                ._area = {
                                    ._pos = params._area._pos + Position::create(str_start, 0),
                                    ._len = rep_start - str_start
                                },
                                ._str = params._str + str_start,
                                ._style = params._style
                            }));
                }

                str_start = i;

                instrs.append(Instruction::createRepeat({
                            ._area = {
                                ._pos = params._area._pos + Position::create(rep_start, 0),
                                ._len = run
                            },
                            ._ch = rep_ch,
                            ._style = params._style
                        }));

            }

            run = 1;
            rep_ch = ch;
            rep_start = i;
        } else {
            run++;
        }
    }

    if(run > 5) {
        if(str_start != rep_start) {
            instrs.append(Instruction::createString({
                        ._area = {
                            ._pos = params._area._pos + Position::create(str_start, 0),
                            ._len = rep_start - str_start
                        },
                        ._str = params._str + str_start,
                        ._style = params._style
                    }));
        }

        instrs.append(Instruction::createRepeat({
                    ._area = {
                        ._pos = params._area._pos + Position::create(rep_start, 0),
                        ._len = run
                    },
                    ._ch = rep_ch,
                    ._style = params._style
                }));
    } else {
        instrs.append(Instruction::createString({
                    ._area = {
                        ._pos = params._area._pos + Position::create(str_start, 0),
                        ._len = params._area._len - str_start
                    },
                    ._str = params._str + str_start,
                    ._style = params._style
                }));
    }

    return instrs;
}

}

}
