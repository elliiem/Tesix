#pragma once

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <optional>
#include <utility>

namespace Tesix {

struct Box;
struct FloatingBox;
struct Selection;

template<typename T>
static constexpr bool is_box_v = false;

template<>
static constexpr bool is_box_v<FloatingBox> = true;

template<>
static constexpr bool is_box_v<Box> = true;

template<typename T>
concept AnyBox = is_box_v<T>;

template<typename T>
static constexpr bool is_selection_v = false;

template<>
static constexpr bool is_selection_v<Selection> = true;

template<typename T>
concept AnySelection = is_selection_v<T>;

template<typename T>
concept AnyArea = is_box_v<T> || is_selection_v<T>;

struct Position {
    size_t _x;
    size_t _y;

    template<AnyArea T>
    inline bool isInside(const T& area) const;

    inline Position operator+(const Position& other) const {
        return {._x = _x + other._x, ._y = _y + other._y};
    }
};

struct Box {
    size_t _width;
    size_t _height;

    inline Position topLeft() const {
        assert(_width > 0);
        assert(_height > 0);

        return {._x = 0, ._y = 0};
    }

    inline Position bottomLeft() const {
        assert(_width > 0);
        assert(_height > 0);

        return {._x = 0, ._y = _height - 1};
    }

    inline Position topRight() const {
        assert(_width > 0);
        assert(_height > 0);

        return {._x = _width - 1, ._y = 0};
    }

    inline Position bottomRight() const {
        assert(_width > 0);
        assert(_height > 0);

        return {._x = _width - 1, ._y = _height - 1};
    }

    template<AnyArea T>
    inline bool contains(const T& child) const;

    template<>
    inline bool contains(const FloatingBox& child) const;

    template<>
    inline bool contains(const Box& child) const;

    inline bool contains(const Selection& child, const Box& term) const;

    inline size_t size() {
        return _width * _height;
    }

    inline bool takesUpSpace() const {
        return _width > 0 && _height > 0;
    }
};

struct FloatingBox {
    Position _pos;
    Box _box;

    inline size_t right() const {
        return _pos._x + _box._width - 1;
    }

    inline size_t bottom() const {
        return _pos._y + _box._height - 1;
    }

    inline Position topLeft() const {
        return {_pos._x, _pos._y};
    }

    inline Position bottomLeft() const {
        return {_pos._x, _pos._y + _box._height - 1};
    }

    inline Position topRight() const {
        return {_pos._x + _box._width - 1, _pos._y};
    }

    inline Position bottomRight() const {
        return {._x = _pos._x + _box._width - 1, ._y = _pos._y + _box._height - 1};
    }

    template<AnyArea T>
    inline bool contains(const T& child) const;

    template<>
    inline bool contains(const Box& child) const;

    template<>
    inline bool contains(const FloatingBox& child) const;

    inline bool contains(const Selection& child, const Box& term) const;

    inline size_t size() {
        return _box.size();
    }

    inline bool takesUpSpace() const {
        return _box._width > 0 && _box._height > 0;
    }
};

struct Selection {
    Position _start;
    Position _end;

    template<AnyArea T>
    inline bool contains(const T& child) const;

    template<>
    inline bool contains(const Box& child) const;

    template<>
    inline bool contains(const FloatingBox& child) const;

    template<>
    inline bool contains(const Selection& child) const;
};

template<>
inline bool Position::isInside(const FloatingBox& area) const {
    return (_x >= area._pos._x && _x <= area.right()) && (_y >= area._pos._y && _y <= area.bottom());
}

template<>
inline bool Position::isInside(const Box& area) const {
    return _x < area._width && _y < area._height;
}

template<>
inline bool Position::isInside(const Selection& area) const {
    return _y >= area._start._y && _x >= area._start._x && _y <= area._end._y && _x <= area._end._x;
}

template<>
inline bool FloatingBox::contains(const Box& child) const {
    return (_pos._x == 0 && child._width <= right()) && (_pos._y == 0 && child._height <= bottom());
}

template<>
inline bool FloatingBox::contains(const FloatingBox& child) const {
    return (child._pos._x >= _pos._x && child.right() <= right()) && (child._pos._y >= _pos._y && child.bottom() <= bottom());
}

inline bool FloatingBox::contains(const Selection& child, const Box& term) const {
    const bool start_check = child._start.isInside(*this);
    const bool end_check = child._end.isInside(*this);

    if(child._start._y == child._end._y) {
        return start_check && end_check;
    } else {
        const bool width_check = _box._width >= term._width && _pos._x == 0;

        return start_check && end_check && width_check;
    }
}

template<>
inline bool Box::contains(const Box& child) const {
    return child._width <= _width && child._height <= _height;
}

template<>
inline bool Box::contains(const FloatingBox& child) const {
    return child.right() < _width && child.bottom() < _height;
}

inline bool Box::contains(const Selection& child, const Box& term) const {
    const bool start_check = child._start.isInside(*this);
    const bool end_check = child._end.isInside(*this);

    if(child._start._y == child._end._y) {
        return start_check && end_check;
    } else {
        const bool width_check = _width >= term._width;

        return start_check && end_check && width_check;
    }
}

template<>
inline bool Selection::contains(const Box& child) const {
    return child.topLeft().isInside(*this) && child.bottomRight().isInside(*this);
}

template<>
inline bool Selection::contains(const FloatingBox& child) const {
    return child.topLeft().isInside(*this) && child.bottomRight().isInside(*this);
}

template<>
inline bool Selection::contains(const Selection& child) const {
    return child._start.isInside(*this) && child._end.isInside(*this);
}

template<AnyBox A, AnyBox B>
static inline bool overlap(const A& a, const B& b) {
    return a.topLeft().isInside(b) || a.bottomLeft().isInside(b) || a.topRight().isInside(b) || a.bottomRight().isInside(b) ||
           b.topLeft().isInside(a) || b.bottomLeft().isInside(a) || b.topRight().isInside(a) || b.bottomRight().isInside(a);
}

template<AnyBox B>
static inline bool overlap(const Selection& a, const B& b) {
    return a._start.isInside(b) && a._end.isInside(b);
}

template<AnyBox A>
static inline bool overlap(const A& a, const Selection& b) {
    return overlap(b, a);
}

static inline bool overlap(const Selection& a, const Selection& b) {
    return a._start.isInside(b) && a._end.isInside(b);
}

} // namespace Tesix
