// since std::vector is a stupid template bloated mess lets create our own
// this implementation is heavily inspired by the implementation in the zig std (its even called the same)
// which is unlike c++ a good language.

#pragma once

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

[[clang::always_inline]] inline size_t getExponent(
    size_t x
) {
    return 64 - __builtin_clzl(x);
}

[[clang::always_inline]] inline size_t pow2(
    size_t exp
) {
    return (exp > 0) ? 1 << exp : 0;
}

template<typename T>
struct ArrayList {
    T* ptr = nullptr;
    size_t len = 0;
    size_t capacity = 0;

    ArrayList() = default;

    ArrayList(
        size_t size
    ) {
        reserveExact(size);
    }

    ~ArrayList() {
        if(ptr != nullptr) {
            free(ptr);
        }
    }

    void reserve(
        const size_t exp
    ) {
        assert(ptr == nullptr);

        const size_t size = pow2(exp);

        ptr = static_cast<T*>(malloc(sizeof(T) * size));
        if(ptr == nullptr) exit(1);

        capacity = size;
    }

    void reserveExact(
        const size_t size
    ) {
        ptr = static_cast<T*>(malloc(sizeof(T) * size));
        if(ptr == nullptr) exit(1);

        capacity = size;
    }

    void append(
        const T& item
    ) {
        expandCapacity(len + 1);

        appendAssume(item);
    }

    void appendArray(
        const T* items,
        const size_t n
    ) {
        expandCapacity(len + n);

        appendArrayAssume(items, n);
    }

    void insert(
        const T item,
        const size_t index
    ) {
        expandCapacity(len + 1);

        insertAssume(item, index);
    }

    inline void pop(
        const size_t index
    ) {
        assert(index < len);

        memmove(ptr + index, ptr + index + 1, (len - index) * sizeof(T));

        len -= 1;
    }

    inline void popBack() {
        assert(len > 0);

        len -= 1;
    }

    inline void clear() {
        len = 0;
    }

    inline size_t freeSpace() {
        return capacity - len;
    }

    inline void appendAssume(
        const T item
    ) {
        assert(len < capacity);

        ptr[len] = item;
        len += 1;
    }

    inline void appendArrayAssume(
        const T* items,
        const size_t n
    ) {
        assert(len + n <= capacity);

        for(size_t i = 0; i < n; i++) {
            ptr[i + len] = items[i];
        }

        len += n;
    }

    inline void insertAssume(
        const T item,
        const size_t index
    ) {
        assert(index < len && len < capacity);

        memmove(ptr + index + 1, ptr + index, (len - index) * sizeof(T));
        ptr[index] = item;

        len += 1;
    }

    void expandCapacity(
        const size_t size
    ) {
        assert(ptr != nullptr); // call reserve before doing anything when initializing with the default constructor

        if(size > capacity) {
            realloc(pow2(getExponent(size)));
        }
    }

    void expandCapacityNoCopy(
        const size_t size
    ) {
        assert(ptr != nullptr); // call reserve before doing anything when initializing with the default constructor

        if(size > capacity) {
            reallocNoCopy(pow2(getExponent(size)));
        }
    }

    void expandCapacityExact(
        const size_t size
    ) {
        assert(ptr != nullptr); // call reserve before doing anything when initializing with the default constructor

        if(size > capacity) {
            realloc(size);
        }
    }

    void expandCapacityExactNoCopy(
        const size_t size
    ) {
        assert(ptr != nullptr); // call reserve before doing anything when initializing with the default constructor

        if(size > capacity) {
            reallocNoCopy(size);
        }
    }

    void minify() {
        realloc(getExponent(len));
    }

    void realloc(
        const size_t size
    ) {
        assert(!(ptr == nullptr && len > 0));

        T* new_ptr = static_cast<T*>(malloc(sizeof(T) * size));
        if(new_ptr == nullptr) exit(1);

        memcpy(new_ptr, ptr, len * sizeof(T));
        free(ptr);

        ptr = new_ptr;
        capacity = size;
    }

    void reallocNoCopy(
        const size_t size
    ) {
        assert(!(ptr == nullptr && len > 0));

        T* new_ptr = static_cast<T*>(malloc(sizeof(T) * size));
        if(new_ptr == nullptr) exit(1);

        free(ptr);

        ptr = new_ptr;
        capacity = size;
    }
};
