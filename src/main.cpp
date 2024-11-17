#define _DEBUG 1

#include "../include/codegen/intermediary.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <termios.h>
#include <unistd.h>

void enableRawMode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void contains() {
    const Tesix::FloatingBox area = {
        ._pos = {._x = 10, ._y = 10},
        ._box = {._width = 5, ._height = 5},
    };

    const Tesix::FloatingBox sub = {
        ._pos = {._x = 11, ._y = 11},
        ._box = {._width = 3, ._height = 3},
    };
}

int main() {
    /*contains();*/
    /*return 0;*/

    enableRawMode();

    ArrayList<Tesix::ControlSeq::Instruction, Dynamic> esc(5);
    esc.append(
        Tesix::ControlSeq::Instruction::createErase(Tesix::ControlSeq::EraseInstruction::createEraseDisplay(Tesix::ControlSeq::EraseDisplay::All)));
    esc.append(Tesix::ControlSeq::Instruction::createCursor(Tesix::ControlSeq::CursorInstruction::createCursorTo({.row = 1, .column = 1})));

    Tesix::Buffer2D<uint32_t> screen_buffer;

    Tesix::Intermediary::State state = {
        ._pos =
            {
                ._x = 0,
                ._y = 0,
            },
        ._style = 0,
        ._last_ch = ' ',
        ._screen_buffer = screen_buffer,
        ._term_area =
            {
                ._width = 128,
                ._height = 90,
            },
    };

    Tesix::LinkedList<Tesix::Intermediary::Instruction> intermediary;
    intermediary.init();

    Tesix::Buffer2D<uint32_t> contents(5, 5);

    intermediary.append(Tesix::Intermediary::Instruction::createMoveArea({
        ._from =
            {
                ._x = 5,
                ._y = 6,
            },
        ._to =
            {
                ._x = 5,
                ._y = 5,
            },
        ._contents = contents.subBuffer({
            ._pos = {._x = 0, ._y = 0},
            ._box = {._width = 5, ._height = 5},
        }),
    }));

    Tesix::Intermediary::submit(intermediary, esc, state);

    esc.append(Tesix::ControlSeq::Instruction::createCursor(Tesix::ControlSeq::CursorInstruction::createCursorTo({.row = 1, .column = 90})));

    ArrayList<uint8_t, Dynamic> out(10);

    FILE* file = fopen("out", "w");

    Tesix::ControlSeq::submit(esc, out, STDOUT_FILENO, file->_fileno);

    close(file->_fileno);
}
