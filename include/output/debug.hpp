#pragma once


#include "output/debug/header.hpp"
#include "output/debug/object.hpp"

#include "util/box.hpp"
#include "util/string.hpp"

#include <unistd.h>

namespace Tesix {

namespace Debug {

template<typename T>

static consteval const char* const objName(
);

template<>
consteval const char* const objName<Area::Position> (
) {
    return "TESIX::Position";
}

template<typename T>
static void infoDebug(
    const T& obj,
    const uintmax_t indent_width,
    const uintmax_t fd = STDOUT_FILENO
);

template<>
void infoDebug(
    const Area::Position& obj,
    const uintmax_t indent_width,
    const uintmax_t fd
) {
    constexpr const char* const header_name = objName<Area::Position>();

    const auto out_c = countHeader<MsgType::Debug>(cstrlen(header_name)) + countObj(obj, 0, indent_width);

    uint8_t* out = static_cast<uint8_t*>(malloc(out_c));

    auto out_rem = out;

    out_rem = writeHeader<MsgType::Debug>(out_rem, asByteStr(header_name), cstrlen(header_name));
    out_rem = writeObj(out_rem, obj, 0, indent_width);

    write(fd, out, out_c);

    free(out);
}

/*static void infoInstrNode(*/
/*    const INTRM::InstrNode node,*/
/*    const size_t fd = STDOUT_FILENO*/
/*) {*/
/*    const auto instr = node->_value;*/
/**/
/*    switch (instr._type) {*/
/*        case INTRM::InstrType::InsertChar: {*/
/*            const auto params = instr._value.InsertChar;*/
/*        } break;*/
/*        case INTRM::InstrType::InsertString: {*/
/*        } break;*/
/*        case INTRM::InstrType::Repeat: {*/
/*        } break;*/
/*        case INTRM::InstrType::FillArea: {*/
/*        } break;*/
/*        case INTRM::InstrType::DrawBuffer: {*/
/*        } break;*/
/*        case INTRM::InstrType::Clear: {*/
/*        } break;*/
/*        case INTRM::InstrType::ClearFw: {*/
/*        } break;*/
/*        case INTRM::InstrType::ClearBw: {*/
/*        } break;*/
/*        case INTRM::InstrType::ClearLine: {*/
/*        } break;*/
/*        case INTRM::InstrType::ClearLineFw: {*/
/*        } break;*/
/*        case INTRM::InstrType::ClearLineBw: {*/
/*        } break;*/
/*        case INTRM::InstrType::ClearArea: {*/
/*        } break;*/
/*        case INTRM::InstrType::Erase: {*/
/*        } break;*/
/*        case INTRM::InstrType::EraseArea: {*/
/*        } break;*/
/*        case INTRM::InstrType::RestoreArea: {*/
/*        } break;*/
/*    }*/
/*}*/

}

}
