#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace Tesix {

struct Position {
    size_t x;
    size_t y;
};

struct Area {
    size_t x;
    size_t y;
    size_t width;
    size_t height;

    [[clang::always_inline]] inline bool isInside(const Position& pos) const {
        return (pos.x > x && pos.x < x + width) && (pos.y > y && pos.y < y + height);
    }

    [[clang::always_inline]] inline size_t left() const {
        return x;
    }

    [[clang::always_inline]] inline size_t right() const {
        return x + width;
    }

    [[clang::always_inline]] inline size_t top() const {
        return y;
    }

    [[clang::always_inline]] inline size_t bottom() const {
        return y + height;
    }

    [[clang::always_inline]] inline Position topLeft() const {
        return {x, y};
    }

    [[clang::always_inline]] inline Position bottomLeft() const {
        return {x, y + height};
    }

    [[clang::always_inline]] inline Position topRight() const {
        return {x + width, y};
    }

    [[clang::always_inline]] inline Position bottomRight() const {
        return {x + width, y + height};
    }

    [[clang::always_inline]] inline bool contains(const Area& child) const {
        return (child.left() >= left() && child.right() <= right()) && (child.top() >= top() && child.bottom() <= bottom());
    }

    void growToContain(const Area& child) {
        if(contains(child)) {
            return;
        }

        if(child.left() < left()) {
            width = (left() - child.left()) + width;

            x = child.left();
        } else if(child.right() > right()) {
            width = child.right() - left();
        }

        if(child.top() < top()) {
            height = (top() - child.top()) + height;

            y = child.top();
        } else if(child.bottom() > bottom()) {
            height = child.bottom() - top();
        }
    }

    static bool overlap(const Area& a, const Area& b) {
        return a.isInside(b.topLeft()) || a.isInside(b.bottomLeft()) || a.isInside(b.topRight()) ||
               a.isInside(b.bottomRight()) || b.isInside(a.topLeft()) || b.isInside(a.bottomLeft()) ||
               b.isInside(a.topRight()) || b.isInside(a.bottomRight());
    }
};

struct Selection {
    Position start;
    Position end;

    static inline Selection fromArea(const Area& area, const Area& term) {
        assert(term.isInside(area.topLeft()));

        const intmax_t oob = area.x + area.width - term.width;

        const Position end = {
            .x = (oob > 0) ? oob % term.width : area.right(),
            .y = area.y + oob / term.width,
        };

        return {.start = area.topLeft(), .end = end};
    }
};

enum class SpaceKind {
    Area,
    Selection,
};

union SpaceValue {
    struct Area Area;
    struct Selection Selection;
};

struct Space {
    SpaceKind kind;
    SpaceValue value;

    static Space createArea(const struct Area& area) {
        return {.kind = SpaceKind::Area, .value = {.Area = area}};
    }

    static Space createSelection(const struct Selection& selection) {
        return {.kind = SpaceKind::Selection, .value = {.Selection = selection}};
    }
};

} // namespace Tesix
