#include <cassert>
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
void printBits(size_t const size, void const* const ptr) {
    unsigned char* b = (unsigned char*)ptr;
    unsigned char byte;
    int i, j;

    for(i = size - 1; i >= 0; i--) {
        for(j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("");
}

int main() {
    enableRawMode();

    ArrayList<Tesix::ControlSeq::Instruction, Dynamic> esc(5);
    esc.append(Tesix::ControlSeq::Instruction::createEraseDisplay(Tesix::ControlSeq::EraseDisplay::All));
    esc.append(Tesix::ControlSeq::Instruction::createCursorTo({._row = 1, ._column = 1}));

    uint64_t initial_style = Tesix::Style::Style {}.fgDefault().bgDefault().toEncoding();

    Tesix::State state = {
        ._pos = {0, 0},
        ._style = Tesix::StyleContainer::createPtr(&initial_style),
        ._last_ch = 0,
        ._screen_buffer = Tesix::createScreenBuffer(),
    };

    Tesix::LinkedList<Tesix::Intermediary::Instruction> intermediary;
    intermediary.init();

    uint32_t a_str[5] = {'x', 'x', 'x', 'x', 'x'};
    uint64_t a_style = Tesix::Style::Style {}.bgFullColor({._r = 255, ._g = 0, ._b = 0, ._a = 13}).toEncoding();
    uint32_t b_str[3] = {'a', 'a', 'a'};
    uint64_t b_style = Tesix::Style::Style {}.fgPalette(1).bgFullColor({._r = 0, ._g = 0, ._b = 255, ._a = 15}).bold().toEncoding();

    intermediary.append(Tesix::Intermediary::Instruction::createInsertString({
        ._pos = {12, 10},
        ._str = a_str,
        ._len = 5,
        ._style = Tesix::StyleContainer::createPtr(&a_style),
    }));

    intermediary.append(Tesix::Intermediary::Instruction::createInsertString({
        ._pos = {10, 10},
        ._str = b_str,
        ._len = 3,
        ._style = Tesix::StyleContainer::createPtr(&b_style),
    }));

    Tesix::Intermediary::resolveStringOnStringOverdraw(intermediary.first(), intermediary.first()->_next, intermediary);

    Tesix::Intermediary::submit(intermediary, esc, state);

    esc.append(Tesix::ControlSeq::Instruction::createResetStyle());
    esc.append(Tesix::ControlSeq::Instruction::createCursorTo({._row = 1, ._column = 90}));

    ArrayList<uint8_t, Dynamic> out(128);

    FILE* file = fopen("out", "w");

    Tesix::ControlSeq::submit(esc, out, STDOUT_FILENO, file->_fileno);

    close(file->_fileno);
}
