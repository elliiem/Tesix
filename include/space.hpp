#pragma once

#include <cassert>
#include <cmath>
#include <cstddef>

namespace Tesix {

struct Box;

struct Position {
    size_t _x;
    size_t _y;

    inline bool isInside(const Box& area) const;

};

struct Box {
    Position _pos;
    size_t _width;
    size_t _height;

    inline bool isInside(const Position& pos) const {
        return (pos._x >= _pos._x && pos._x <= _pos._x + _width) && (pos._y >= _pos._y && pos._y <= pos._y + _height);
    }

    inline size_t right() const {
        return _pos._x + _width;
    }

    inline size_t bottom() const {
        return _pos._y + _height;
    }

    inline Position topLeft() const {
        return {_pos._x, _pos._y};
    }

    inline Position bottomLeft() const {
        return {_pos._x, _pos._y + _height};
    }

    inline Position topRight() const {
        return {_pos._x + _width, _pos._y};
    }

    inline Position bottomRight() const {
        return {_pos._x + _width, _pos._y + _height};
    }

    inline bool contains(const Box& child) const {
        return (child._pos._x >= _pos._x && child.right() <= right()) && (child._pos._y >= _pos._y && child.bottom() <= bottom());
    }

    inline bool containsLeft(const Box& child) {
        return child._pos._x >= _pos._x && (child._pos._y >= _pos._y && child.bottom() <= bottom());
    }

    void growToContain(const Box& child) {
        if(contains(child)) {
            return;
        }

        if(child._pos._x < _pos._x) {
            _width = (_pos._x - child._pos._x) + _width;

            _pos._x = child._pos._x;
        } else if(child.right() > right()) {
            _width = child.right() - _pos._x;
        }

        if(child._pos._y < _pos._y) {
            _height = (_pos._y - child._pos._y) + _height;

            _pos._y = child._pos._y;
        } else if(child.bottom() > bottom()) {
            _height = child.bottom() - _pos._y;
        }
    }

    static bool overlap(const Box& a, const Box& b) {
        return a.isInside(b.topLeft()) || a.isInside(b.bottomLeft()) || a.isInside(b.topRight()) || a.isInside(b.bottomRight()) ||
               b.isInside(a.topLeft()) || b.isInside(a.bottomLeft()) || b.isInside(a.topRight()) || b.isInside(a.bottomRight());
    }
};

inline bool Position::isInside(const Box& area) const {
        return (_x >= area._pos._x && _x <= area._pos._x + area._width) && (_y >= area._pos._y && _y <= area._pos._y + area._height);
}

}
