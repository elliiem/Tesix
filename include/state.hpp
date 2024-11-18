#include "box.hpp"
#include "buffer.hpp"

#include <cstdint>
#include <sys/ioctl.h>

namespace Tesix {

inline Buffer2D<uint32_t> createScreenBuffer() {
    struct winsize size;
    ioctl(0, TIOCGWINSZ, &size);

    return Buffer2D<uint32_t>(size.ws_col, size.ws_row);
}

struct State {
    Position _pos;
    uint64_t _style;
    uint32_t _last_ch;
    Buffer2D<uint32_t> _screen_buffer;

    static State inital() {
        return {
            ._pos = {
                ._x = 0,
                ._y = 0,
            },
            ._style = 0,
            ._last_ch = 0,
            ._screen_buffer = createScreenBuffer(),
        };
    }
};

} // namespace Tesix
