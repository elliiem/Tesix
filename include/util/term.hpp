#pragma once

#include <termios.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>

namespace Tesix {

static void enableRawMode() {
    struct termios raw;

    tcgetattr(STDIN_FILENO, &raw);

    raw.c_lflag &= ~(ECHO);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

static intmax_t msleep(
    const uintmax_t ms
) {
    struct timespec ts;
    intmax_t ret;

    if (ms < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;

    do {
        ret = nanosleep(&ts, &ts);
    } while (ret && errno == EINTR);

    return ret;
}

}
