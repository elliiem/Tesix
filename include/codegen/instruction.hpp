#pragma once

#include "util/buffer.hpp"
#include "util/space.hpp"
#include "util/style.hpp"

namespace Tesix {

namespace Codegen {

enum class InstructionE {
    String,
    Repeat,

    FillArea,
    DrawBuffer,

    EraseDisplay,
    EraseDisplayForwards,
    EraseDisplayBackwards,
    EraseLine,
    EraseLineForwards,
    EraseLineBackwards,
    EraseArea,
    Erase,

    RestoreArea,
};

struct StringParams {
    Text _area;
    const uint32_t* _str;
    Style::StyleContainer _style;
};

struct RepeatParams {
    Text _area;
    uint32_t _ch;
    Style::StyleContainer _style;
};

struct FillAreaParams {
    FloatingBox _area;
    uint32_t _ch;
    Style::StyleContainer _style;
};

struct DrawBufferParams {
    Position _pos;
    StyledBufferArea _contents;
};

struct EraseDisplayParams {
    Style::StyleContainer _style;
};

struct EraseDisplayForwardsParams {
    Position _pos;
    Style::StyleContainer _style;
};

struct EraseDisplayBackwardsParams {
    Position _pos;
    Style::StyleContainer _style;
};

struct EraseLineParams {
    uintmax_t _y;
    Style::StyleContainer _style;
};

struct EraseLineForwardsParams {
    Position _pos;
    Style::StyleContainer _style;
};

struct EraseLineBackwardsParams {
    Position _pos;
    Style::StyleContainer _style;
};

struct EraseAreaParams {
    FloatingBox _area;
    Style::StyleContainer _style;
};

struct EraseParams {
    Position _pos;
    uintmax_t _n;
    Style::StyleContainer _style;
};

struct RestoreAreaParams {
    FloatingBox _area;
};

union InstructionU {
    StringParams String;
    RepeatParams Repeat;
    FillAreaParams FillArea;
    DrawBufferParams DrawBuffer;
    EraseDisplayParams EraseDisplay;
    EraseDisplayForwardsParams EraseDisplayForwards;
    EraseDisplayBackwardsParams EraseDisplayBackwards;
    EraseLineParams EraseLine;
    EraseLineForwardsParams EraseLineForwards;
    EraseLineBackwardsParams EraseLineBackwards;
    EraseAreaParams ClearArea;
    EraseParams Erase;
    EraseAreaParams EraseArea;
    RestoreAreaParams RestoreArea;
};

struct Instruction {
    InstructionE _type;
    InstructionU _value;

    static inline Instruction createString(
        const StringParams& params
    ) {
        return {._type = InstructionE::String, ._value = {.String = params}};
    }

    static inline Instruction createRepeat(
        const RepeatParams& params
    ) {
        return {._type = InstructionE::Repeat, ._value = {.Repeat = params}};
    }

    static inline Instruction createErase(
        const EraseParams& params
    ) {
        return {._type = InstructionE::Erase, ._value = {.Erase = params}};
    }

    static inline Instruction createEraseDisplay(
        const EraseDisplayParams& params
    ) {
        return {._type = InstructionE::EraseDisplay, ._value = {.EraseDisplay = params}};
    }

    static inline Instruction createEraseDisplayForwards(
        const EraseDisplayForwardsParams& params
    ) {
        return {._type = InstructionE::EraseDisplay, ._value = {.EraseDisplayForwards = params}};
    }

    static inline Instruction createEraseDisplayBackwards(
        const EraseDisplayBackwardsParams& params
    ) {
        return {._type = InstructionE::EraseDisplay, ._value = {.EraseDisplayBackwards = params}};
    }

    static inline Instruction createEraseLine(
        const EraseLineParams& params
    ) {
        return {._type = InstructionE::EraseLine, ._value = {.EraseLine = params}};
    }

    static inline Instruction createEraseLineForwards(
        const EraseLineForwardsParams& params
    ) {
        return {._type = InstructionE::EraseLine, ._value = {.EraseLineForwards = params}};
    }

    static inline Instruction createEraseLineBackwards(
        const EraseLineBackwardsParams& params
    ) {
        return {._type = InstructionE::EraseLine, ._value = {.EraseLineBackwards = params}};
    }

    static inline Instruction createEraseArea(
        const EraseAreaParams& params
    ) {
        return {._type = InstructionE::EraseArea, ._value = {.ClearArea = params}};
    }

    static inline Instruction createFillArea(
        const FillAreaParams& params
    ) {
        return {._type = InstructionE::FillArea, ._value = {.FillArea = params}};
    }

    static inline Instruction createDrawBuffer(
        const DrawBufferParams& params
    ) {
        return {._type = InstructionE::DrawBuffer, ._value = {.DrawBuffer = params}};
    }
};

}

} // namespace Tesix
