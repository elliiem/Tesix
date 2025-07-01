#include "codegen/submit.hpp"

#include "util/buffer/draw.hpp"
#include "util/term.hpp"

int main() {
    Tesix::enableRawMode();

    auto out_buf = Tesix::Array<uint8_t>::alloc(100);
    auto instr_buf = Tesix::Array<Tesix::Out::Instruction>::alloc(100);

    auto state = Tesix::Codegen::State::initial();

    const uintmax_t fd = STDOUT_FILENO;

    const uint64_t bg_enc = Tesix::Style::Style().bgFullColor({._r = 0, ._g = 0, ._b = 0, ._a = 15}).toEncoding();
    const auto bg_cont = Tesix::Style::StyleContainer::createValue(bg_enc);

    const uint64_t red_green_enc = Tesix::Style::Style().italic().fgFullColor({._r = 255, ._g = 0, ._b = 0, ._a = 15}).bgFullColor({._r = 7, ._g = 200, ._b = 25, ._a = 15}).underlined().toEncoding();
    const auto red_green_cont = Tesix::Style::StyleContainer::createValue(red_green_enc);

    auto window_buf = Tesix::StyledBuffer::init(10, 5);

    {
        Tesix::Draw::fill(window_buf.all(), '#', red_green_cont);
    }

    {
        Tesix::Out::streamInstruction(out_buf, instr_buf, Tesix::Out::Instruction::createCursorPositionAbsolute(Tesix::Position::create(0, 0)), fd);
    }

    uintmax_t x = 0;
    uintmax_t y = 0;

    intmax_t x_vel = 1;
    intmax_t y_vel = 1;

    while(true) {

        { // setup frame
            Tesix::Codegen::submitInstruction(out_buf, instr_buf, state, Tesix::Codegen::Instruction::createEraseDisplay({._style = bg_cont}), fd);

            Tesix::Out::emptyInstructionBuffer(out_buf, instr_buf, fd);
            Tesix::Out::emptyOutBuffer(out_buf, fd);
        }

        { // draw window
            const auto draw_window_instr = Tesix::Codegen::Instruction::createDrawBuffer({
                ._pos = {x, y + 1},
                ._contents = window_buf.all()
            });

            Tesix::Codegen::submitInstruction(out_buf, instr_buf, state, draw_window_instr, fd);
            Tesix::Out::streamInstruction(out_buf, instr_buf, Tesix::Out::Instruction::createCursorPositionAbsolute(Tesix::Position::create(0, 0)), fd);

            Tesix::Out::emptyInstructionBuffer(out_buf, instr_buf, fd);
            Tesix::Out::emptyOutBuffer(out_buf, fd);
        }

        { // move window
            x += x_vel;
            y += y_vel;

            if(x == 0 || x + window_buf._ch._box._width >= 240) {
                x_vel = -x_vel;
            }

            if(y == 0 || y + window_buf._ch._box._height >= 56) {
                y_vel = -y_vel;
            }
        }

        Tesix::msleep(100);

    }

    out_buf.free();
    instr_buf.free();
}
