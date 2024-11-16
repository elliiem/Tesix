// since std::vector is a stupid template bloated mess lets create our own
// this implementation is heavily inspired by the implementation in the zig std (its even called the same)
// which is unlike c++ a good language.

#pragma once

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

[[clang::always_inline]] inline size_t getExponent(size_t x) {
    return 64 - __builtin_clzl(x);
}

[[clang::always_inline]] inline size_t pow2(size_t exp) {
    return (exp > 0) ? 1 << exp : 0;
}

enum ArrayType {
    // array is always its initial size
    Static,
    // array will be resized when needed
    Dynamic,
};

template<typename T, ArrayType type>
struct ArrayList;

template<typename T>
struct ArrayList<T, Dynamic> {
    T* ptr = nullptr;
    size_t len = 0;
    size_t capacity = 0;

    //  TODO: ? shold I keep having init be its own function
    ArrayList() = default;

    ArrayList(size_t size) {
        reserveExact(size);
    }

    ~ArrayList() {
        if(ptr != nullptr) {
            free(ptr);
        }
    }

    void reserve(const size_t exp) {
        assert(ptr == nullptr);

        const size_t size = pow2(exp);

        ptr = static_cast<T*>(malloc(sizeof(T) * size));
        if(ptr == nullptr) exit(1);

        capacity = size;
    }

    void reserveExact(const size_t size) {
        ptr = static_cast<T*>(malloc(sizeof(T) * size));
        if(ptr == nullptr) exit(1);

        capacity = size;
    }

    void append(const T item) {
        expandCapacity(len + 1);

        appendAssume(item);
    }

    void appendArray(const T* items, const size_t n) {
        expandCapacity(len + n);

        appendArrayAssume(items, n);
    }

    void insert(const T item, const size_t index) {
        expandCapacity(len + 1);

        insertAssume(item, index);
    }

    inline void pop(const size_t index) {
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

    inline void appendAssume(const T item) {
        assert(len < capacity);

        ptr[len] = item;
        len += 1;
    }

    inline void appendArrayAssume(const T* items, const size_t n) {
        assert(len + n <= capacity);

        for(size_t i = 0; i < n; i++) {
            ptr[i + len] = items[i];
        }

        len += n;
    }

    inline void insertAssume(const T item, const size_t index) {
        assert(index < len && len < capacity);

        memmove(ptr + index + 1, ptr + index, (len - index) * sizeof(T));
        ptr[index] = item;

        len += 1;
    }

    void expandCapacity(const size_t size) {
        assert(ptr != nullptr); // call reserve before doing anything when initializing with the default constructor

        if(size > capacity) {
            realloc(pow2(getExponent(size)));
        }
    }

    void expandCapacityNoCopy(const size_t size) {
        assert(ptr != nullptr); // call reserve before doing anything when initializing with the default constructor

        if(size > capacity) {
            reallocNoCopy(pow2(getExponent(size)));
        }
    }

    void expandCapacityExact(const size_t size) {
        assert(ptr != nullptr); // call reserve before doing anything when initializing with the default constructor

        if(size > capacity) {
            realloc(size);
        }
    }

    void expandCapacityExactNoCopy(const size_t size) {
        assert(ptr != nullptr); // call reserve before doing anything when initializing with the default constructor

        if(size > capacity) {
            reallocNoCopy(size);
        }
    }

    void minify() {
        realloc(getExponent(len));
    }

    void realloc(const size_t size) {
        assert(!(ptr == nullptr && len > 0));

        T* new_ptr = static_cast<T*>(malloc(sizeof(T) * size));
        if(new_ptr == nullptr) exit(1);

        memcpy(new_ptr, ptr, len * sizeof(T));
        free(ptr);

        ptr = new_ptr;
        capacity = size;
    }

    void reallocNoCopy(const size_t size) {
        assert(!(ptr == nullptr && len > 0));

        T* new_ptr = static_cast<T*>(malloc(sizeof(T) * size));
        if(new_ptr == nullptr) exit(1);

        free(ptr);

        ptr = new_ptr;
        capacity = size;
    }
};

template<typename T>
struct ArrayList<T, Static> {
    T* ptr = nullptr;
    size_t len = 0;
    size_t capacity = 0;

    ArrayList() = default;

    ArrayList(const size_t size) {
        reserve(size);
    }

    ~ArrayList() {
        if(ptr != nullptr) {
            free(ptr);
        }
    }

    void reserve(const size_t size) {
        assert(ptr == nullptr); // should be a fresh array

        ptr = static_cast<uint8_t*>(malloc(size));
        capacity = size;
    }

    void append(const T item) {
        assert(len < capacity);

        ptr[len] = item;
        len += 1;
    }

    void appendArray(const uint8_t* items, const size_t n) {
        assert(len + n < capacity);

        for(size_t i = 0; i < n; i++) {
            ptr[len + i] = items[i];
        }

        len += n;
    }

    void insert(const T item, const size_t index) {
        assert(index < len && len < capacity);

        memmove(ptr + index + 1, ptr + index, (len - index) * sizeof(T));
        ptr[index] = item;

        len += 1;
    }
    inline void pop(const size_t index) {
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

    void appendAssume(const T item) {
        append(item);
    }

    void appendArrayAssume(const uint8_t* items, const size_t size) {
        appendArray(items, size);
    }

    void insertAssume(const T item, const size_t index) {
        insert(item, index);
    }
};

template<typename T>
static constexpr bool is_array_list = false;

template<typename T, ArrayType type>
static constexpr bool is_array_list<ArrayList<T, type>> = true;

template<typename T>
concept AnyArrayList = is_array_list<T>;

template<typename T, typename type>
static constexpr bool is_type_array_list = false;

template<ArrayType array_type, typename type>
static constexpr bool is_type_array_list<ArrayList<type, array_type>, type> = true;

template<typename T, typename type>
concept TypeArrayList = is_type_array_list<T, type>;

template<AnyArrayList T>
static constexpr bool can_grow = false;

template<typename T>
static constexpr bool can_grow<ArrayList<T, Dynamic>> = true;
