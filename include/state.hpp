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
    const uint64_t* _style;
    uint32_t _last_ch;
    Buffer2D<uint32_t> _screen_buffer;
};

} // namespace Tesix
