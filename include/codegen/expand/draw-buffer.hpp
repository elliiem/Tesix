#pragma once

#include "codegen/instruction.hpp"
#include "codegen/optimize/string-repeat.hpp"

#include "util/linked-list.hpp"

namespace Tesix {

namespace Codegen {

static LinkedList<Instruction> expandDrawBuffer(
    const DrawBufferParams& params
) {
    auto instrs = LinkedList<Instruction>::init();

    for(uintmax_t y = 0; y < params._contents._ch._area._box._height; y++) {
        uint64_t cur_style = params._contents._style.at(Position::create(0, y)).value();

        uintmax_t cur_str_start = 0;

        for(uintmax_t x = 1; x < params._contents._ch._area._box._width; x++) {
            const uint64_t ch_style = params._contents._style.at(Position::create(x, y)).value();

            if(ch_style != cur_style) {
                instrs.append(Instruction::createString(StringParams{
                            ._area = {
                                ._pos = {
                                    ._x = params._pos._x + cur_str_start,
                                    ._y = params._pos._y + y,
                                },
                                ._len = x - cur_str_start
                            },
                            ._str = &params._contents._ch.at(Position::create(cur_str_start, y)),
                            ._style = params._contents._style.at(Position::create(cur_str_start, y))
                        }));

                cur_str_start = x;
                cur_style = ch_style;
            }
        }

        instrs.append(Instruction::createString(StringParams{
                    ._area = {
                        ._pos = {
                            ._x = params._pos._x + cur_str_start,
                            ._y = params._pos._y + y,
                        },
                        ._len = params._contents._ch._area._box._width - cur_str_start
                    },
                    ._str = &params._contents._ch.at(Position::create(cur_str_start, y)),
                    ._style = params._contents._style.at(Position::create(cur_str_start, y))
                }));
    }

    const Node<Instruction>* cur = instrs._front;

    while(cur != nullptr) {
        const Node<Instruction>* const next = cur->_next;

        instrs.replaceNode(cur, optStringRepeat(cur->_value._value.String));

        cur = next;
    }

    return instrs;
}

}

}
