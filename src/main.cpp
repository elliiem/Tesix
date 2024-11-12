// #include "../include/linked_list.hpp"
#include "../include/codegen/intermediary.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <termios.h>

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

    intermediary.append(Tesix::Intermediary::Instruction::createInsertChar({.x = 10, .y = 10, .ch = 'x'}));


    Tesix::Intermediary::State state;
    Tesix::Intermediary::createEscapeSeqence(intermediary, esc, state);

    ArrayList<uint8_t, Dynamic> out(10);

    Tesix::ControlSeq::submit(esc, out);

    while(true) {

    }
}
