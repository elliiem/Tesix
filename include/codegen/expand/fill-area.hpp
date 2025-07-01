#pragma once

#include "codegen/instruction.hpp"

#include "util/linked-list.hpp"

namespace Tesix {

namespace Codegen {

static LinkedList<Instruction> expandFillArea(
    const FillAreaParams& params
) {
    auto instrs = LinkedList<Instruction>::init();

    for(uintmax_t i = 0; i < params._area._box._height; i++) {
        instrs.append(Instruction::createRepeat(RepeatParams{
                    ._area = {
                        ._pos = {
                            ._x = params._area._pos._x,
                            ._y = params._area._pos._y + i
                        },
                        ._len = params._area._box._width
                    },
                    ._ch = params._ch,
                    ._style = params._style
                }
        ));
    }

    return instrs;
}

}

}
