#pragma once

#include <cassert>
#include <cmath>
#include <cstddef>

namespace Tesix {

struct FloatingBox;
struct Box;

template<typename T>
static constexpr bool is_box_v = false;

template<>
static constexpr bool is_box_v<FloatingBox> = true;

template<>
static constexpr bool is_box_v<Box> = true;

template<typename T>
concept AnyBox = is_box_v<T>;

struct Position {
    size_t _x;
    size_t _y;

    template<AnyBox T>
    inline bool isInside(const T& area) const;
};

struct Box {
    size_t _width;
    size_t _height;

    inline Position topLeft() {
        assert(_width > 0);
        assert(_height > 0);

        return {._x = 0, ._y = 0};
    }

    inline Position bottomLeft() {
        assert(_width > 0);
        assert(_height > 0);

        return {._x = 0, ._y = _height - 1};
    }

    inline Position topRight() {
        assert(_width > 0);
        assert(_height > 0);

        return {._x = _width - 1, ._y = 0};
    }

    inline Position bottomRight() {
        assert(_width > 0);
        assert(_height > 0);

        return {._x = _width - 1, ._y = _height - 1};
    }

    template<AnyBox T>
    inline bool contains(const T& child) const;

    template<>
    inline bool contains(const FloatingBox& child) const;

    template<>
    inline bool contains(const Box& child) const;

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
        return _pos._x + _box._width;
    }

    inline size_t bottom() const {
        return _pos._y + _box._height;
    }

    inline Position topLeft() const {
        return {_pos._x, _pos._y};
    }

    inline Position bottomLeft() const {
        return {_pos._x, _pos._y + _box._height};
    }

    inline Position topRight() const {
        return {_pos._x + _box._width, _pos._y};
    }

    inline Position bottomRight() const {
        return {_pos._x + _box._width, _pos._y + _box._height};
    }

    template<AnyBox T>
    inline bool contains(const T& child) const;

    template<>
    inline bool contains(const FloatingBox& child) const;

    inline size_t size() {
        return _box.size();
    }

    inline bool takesUpSpace() const {
        return _box._width > 0 && _box._height > 0;
    }

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
inline bool FloatingBox::contains(const FloatingBox& child) const {
    return (child._pos._x >= _pos._x && child.right() <= right()) && (child._pos._y >= _pos._y && child.bottom() <= bottom());
}

template<>
inline bool FloatingBox::contains(const Box& child) const {
    return (_pos._x == 0 && child._width <= right()) && (_pos._y == 0 && child._height <= bottom());
}

template<>
inline bool Box::contains(const FloatingBox& child) const {
    return child.right() < _width && child.bottom() < _height;
}

template<>
inline bool Box::contains(const Box& child) const {
    return child._width < _width && child._height < _height;
}

template<AnyBox A, AnyBox B>
static bool overlap(const A& a, const B& b) {
    return a.topLeft().isInside(b) || a.bottomLeft().isInside(b) || a.topRight().isInside(b) || a.bottomRight().isInside(b) ||
           b.topLeft().isInside(a) || b.bottomLeft().isInside(a) || b.topRight().isInside(a) || b.bottomRight().isInside(a);
}

} // namespace Tesix
