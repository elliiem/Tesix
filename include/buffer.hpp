#pragma once

#include <cassert>
#include <cstddef>
#include <cstdlib>

template<typename T>
struct Buffer2D {
    T* buffer = nullptr;

    size_t width  = 0;
    size_t height = 0;

    Buffer2D() = default;

    Buffer2D(const size_t width, const size_t height) {
        alloc(width, height);
    }

    ~Buffer2D() {
        free(buffer);
    }

    inline size_t alloc(const size_t _width, const size_t _height) {
        assert(buffer == nullptr);

        width  = _width;
        height = _height;

        buffer = static_cast<T*>(malloc(width * height));
    }

    inline size_t offset(const size_t x, const size_t y) {
        assert(x < width);
        assert(y < height);

        return x + y * width;
    }

    inline T& at(const size_t x, const size_t y) {
        assert(x < width);
        assert(y < height);

        return buffer[offset(x, y)];
    }

    inline size_t size() {
        return height * width;
    }
};
