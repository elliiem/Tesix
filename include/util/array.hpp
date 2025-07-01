#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdint.h>
#include <assert.h>
#include <sys/types.h>

namespace Tesix {

template<typename T>
struct Array {
    T* _ptr;
    uintmax_t _cap;
    uintmax_t _n;

    static inline Array<T> alloc(
        const uintmax_t n
    ) {
        return {._ptr = (T*)(malloc(sizeof(T) * n)), ._cap = n, ._n = 0};
    }

    static inline T* const allocRaw(
        const uintmax_t n
    ) {
        return (T*)(malloc(sizeof(T) * n));
    }

    static inline Array<T> fromRawEmpty(
        T* const ptr,
        const uintmax_t cap
    ) {
        return {._ptr = ptr, ._cap = cap, ._n = 0};
    }

    static inline Array<T> fromRawFull(
        T* const ptr,
        const uintmax_t cap
    ) {
        return {._ptr = ptr, ._cap = cap, ._n = cap};
    }

    static inline const Array<T> fromRawEmpty (
        const T* const ptr,
        const uintmax_t cap
    ) {
        return {._ptr = const_cast<T* const>(ptr), ._cap = cap, ._n = 0};
    }

    static inline const Array<T> fromRawFull(
        const T* const ptr,
        const uintmax_t cap
    ) {
        return {._ptr = const_cast<T* const>(ptr), ._cap = cap, ._n = cap};
    }

    inline void free() const {
        ::free(_ptr);
    }

    inline uintmax_t remaining() const {
        return _cap - _n;
    }

    inline T* const end() {
        return _ptr + _n;
    }

    inline const T* const end() const {
        return _ptr + _n;
    }

    inline void append(
        const T& item
    ) {
        assert(remaining() >= 1);

        _ptr[_n] = item;
        _n += 1;
    }

    inline void appendMulti(
        const T* const items,
        const uintmax_t n
    ) {
        assert(remaining() >= n);

        memcpy(end(), items, n * sizeof(T));

        _n += n;
    }
};

}
