#include "codegen/submit.hpp"

#include "util/term.hpp"

int main() {
    Tesix::enableRawMode();

    auto out_buf = Tesix::Array<uint8_t>::alloc(100);
    auto instr_buf = Tesix::Array<Tesix::Out::Instruction>::alloc(100);

    auto state = Tesix::Codegen::State::initial();

    const uintmax_t fd = STDOUT_FILENO;

    const uint64_t bg_enc = Tesix::Style::Style().bgFullColor({._r = 0, ._g = 0, ._b = 0, ._a = 15}).toEncoding();
    const auto bg_cont = Tesix::Style::StyleContainer::createValue(bg_enc);

    Tesix::Codegen::submitInstruction(out_buf, instr_buf, state, Tesix::Codegen::Instruction::createEraseDisplay({._style = bg_cont}), fd);

    const uint32_t filler[] = {' '};

    for(uintmax_t y = 1; y < 10; y++) {
        for(uintmax_t x = 1; x < 10; x++) {
            Tesix::Codegen::submitInstruction(out_buf, instr_buf, state, Tesix::Codegen::Instruction::createString({
                ._area = {
                    ._pos = {
                        ._x = x + 10,
                        ._y = y + 10
                    },
                    ._len = 1
                },
                ._str = filler,
                ._style = Tesix::Style::StyleContainer::createValue(
                    Tesix::Style::Style{}.bgFullColor( {
                    ._r = static_cast<uint8_t>(x * 20),
                    ._g = static_cast<uint8_t>(y * 20),
                    ._b = 0,
                    ._a = 15,
                }).toEncoding()
                )
            }), fd);

            Tesix::Out::emptyInstructionBuffer(out_buf, instr_buf, fd);
            Tesix::Out::emptyOutBuffer(out_buf, fd);
        }
    }

    Tesix::Out::streamInstruction(out_buf, instr_buf, Tesix::Out::Instruction::createCursorPositionAbsolute(Tesix::Position::create(0, 50)), fd);

    Tesix::Out::emptyInstructionBuffer(out_buf, instr_buf, fd);
    Tesix::Out::emptyOutBuffer(out_buf, fd);

    out_buf.free();
    instr_buf.free();
}
