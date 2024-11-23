#include "box.hpp"
#include "buffer.hpp"
#include "style.hpp"

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
    StyleContainer _style;
    uint32_t _last_ch;
    Buffer2D<uint32_t> _screen_buffer;
};

} // namespace Tesix
