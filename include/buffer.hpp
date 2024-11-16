#pragma once

#include "space.hpp"

#include <cassert>
#include <cstddef>
#include <cstdlib>

namespace Tesix {

template<typename T>
struct Buffer2D {
    T* _buffer = nullptr;

    Box _box;

    struct SubBuffer {
        Buffer2D<T>& _parent;

        FloatingBox _area;

        inline size_t index(const size_t x, const size_t y) const {
            return _parent.index(_area._pos._x + x, _area._pos._y + y);
        }

        inline T& at(const size_t x, const size_t y) {
            assert((Position {._x = x, ._y = y}.isInside(_area)));

            return _parent.at(_area._pos._x + x, _area._pos._y + y);
        }

        inline T at(const size_t x, const size_t y) const {
            assert((Position {._x = x, ._y = y}.isInside(_area)));

            return _parent.at(_area._pos._x + x, _area._pos._y + y);
        }
    };

    Buffer2D() = default;

    Buffer2D(const size_t width, const size_t height) {
        init(width, height);
    }

    ~Buffer2D() {
        free(_buffer);
    }

    inline size_t init(const size_t width, const size_t height) {
        assert(_buffer == nullptr);

        _buffer = static_cast<T*>(malloc(width * height));

        _box._width = width;
        _box._height = height;
    }

    inline size_t index(const size_t x, const size_t y) const {
        assert(x < _box._width);
        assert(y < _box._height);

        return x + y * _box._width;
    }

    inline T& at(const size_t x, const size_t y) {
        assert(x < _box._width);
        assert(y < _box._height);

        return _buffer[index(x, y)];
    }

    inline T at(const size_t x, const size_t y) const {
        assert(x < _box._width);
        assert(y < _box._height);

        return _buffer[index(x, y)];
    }

    inline SubBuffer subBuffer(const FloatingBox& area) {
        assert(_box.contains(area));

        return {._parent = *this, ._area = area};
    }
};

} // namespace Tesix
