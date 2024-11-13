// #include "../include/linked_list.hpp"
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

int main() {
    enableRawMode();

    ArrayList<Tesix::ControlSeq::Instruction, Dynamic> esc(5);

    Tesix::LinkedList<Tesix::Intermediary::Instruction> intermediary;
    intermediary.init();

    intermediary.append(Tesix::Intermediary::Instruction::createInsertChar({.pos = {.x = 10, .y = 10}, .ch = 'x'}));


    Tesix::Intermediary::State state;
    Tesix::Intermediary::submit(intermediary, esc, state);

    ArrayList<uint8_t, Dynamic> out(10);

    // FILE* file = fopen("out","w");

    Tesix::ControlSeq::submit(esc, out);

    // close(file->_fileno);

    while(true) {

    }
}
