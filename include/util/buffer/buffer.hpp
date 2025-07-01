#pragma once

#include "util/array.hpp"
#include "util/style.hpp"
#include "util/space.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <sys/ioctl.h>

namespace Tesix {

template<typename T>
struct BufferArea;

template<typename T>
struct Buffer {
    T* _ptr = nullptr;

    Box _box;

    ~Buffer() {
        free(_ptr);
    }

    static inline Buffer<T> init(
        const size_t width,
        const size_t height
    ) {
        return {
            ._ptr = Array<T>::allocRaw(width * height),
            ._box = {
                ._width = width,
                ._height = height
            }
        };
    }

    inline size_t index(
        const Position& pos
    ) const {
        assert(pos._x < _box._width);
        assert(pos._y < _box._height);

        return pos._x + pos._y * _box._width;
    }

    inline T& at(
        const Position& pos
    ) {
        assert(pos._x < _box._width);
        assert(pos._y < _box._height);

        return _ptr[index(pos)];
    }

    inline const T& at(
        const Position& pos
    ) const {
        assert(pos._x < _box._width);
        assert(pos._y < _box._height);

        return _ptr[index(pos)];
    }

    inline BufferArea<T> area(
        const FloatingBox& area
    ) {
        assert(_box.contains(area));

        return {._parent = this, ._area = area};
    }

    inline BufferArea<T> all() {
        return BufferArea<T>{._parent = this, ._area = {._pos = {._x = 0, ._y = 0}, ._box = _box}};
    }
};

template<typename T>
struct BufferArea {
    Buffer<T>* _parent;

    FloatingBox _area;

    inline size_t index(
        const size_t x,
        const size_t y
    ) const {
        assert(_parent != nullptr);

        return _parent->index(_area._pos._x + x, _area._pos._y + y);
    }

    inline T& at(
        const Position& pos
    ) {
        assert(_parent != nullptr);
        assert(pos.isInside(_area));

        return _parent->at(pos + _area._pos);
    }

    inline const T& at(
        const Position& pos
    ) const {
        assert(_parent != nullptr);
        assert(pos.isInside(_area));

        return _parent->at(pos + _area._pos);
    }

    inline BufferArea<T> area(
        const FloatingBox& area
    ) {
        assert(_parent != nullptr);
        assert(_area.contains(area));

        return {._parent = _parent, ._area = area};
    }
};

} // namespace Tesix
