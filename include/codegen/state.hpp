#pragma once

#include "util/space.hpp"

#include <stdint.h>

namespace Tesix {

namespace Codegen {

struct State {
    uint64_t _style;
    uint32_t _last_ch;
    Position _cursor_pos;

    static inline State initial() {
        return {
            ._style = 0,
            ._last_ch = 0,
            ._cursor_pos = {
                ._x = 0,
                ._y = 0
            }
        };
    }
};

}

}
