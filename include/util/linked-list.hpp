#pragma once

#include "util/array-list.hpp"

#include <stdint.h>
#include <stdlib.h>

namespace Tesix {

template<typename T>
struct Node {
    Node<T>* _prev;
    Node<T>* _next;
    T _value;

    static Node<T>* alloc() {
        return static_cast<Node<T>*>(malloc(sizeof(Node<T>)));
    }
};

template<typename T>
static void freeNodes(
    Node<T>* node
) {
    if(node == nullptr) {
        return;
    }

    assert(!isLoopingNodePath(node));

    Node<T>* next = node->_next;

    free(node);

    freeNodes(next);
}

template<typename T>
static inline void connectNodes(
    Node<T>* prev,
    Node<T>* next
) {
    assert(prev != nullptr);
    assert(next != nullptr);

    prev->_next = next;
    next->_prev = prev;
}

template<typename T>
static size_t countNodesForwards(
    const Node<T>* const node
) {
    assert(node != nullptr);

    size_t result = 0;

    const Node<T>* cur = node;

    while(cur != nullptr) {
        result += 1;

        cur = cur->_next;
    }

    return result;
}

template<typename T>
static Node<T>* const walkNodesForwards(
    Node<T>* const node,
    const uintmax_t n
) {
    assert(countNodesForwards(node) > n);

    Node<T>* cur = node;

    for(uint64_t i = 0; i < n; i++) {
        if(cur == nullptr) {
            return nullptr;
        }

        cur = cur->_next;
    }

    return cur;
}

template<typename T>
static const Node<T>* const walkNodesForwards(
    const Node<T>* const node,
    const uintmax_t n
) {
    assert(countNodesForwards(node) > n);

    Node<T>* cur = node;

    for(uint64_t i = 0; i < n; i++) {
        if(cur == nullptr) {
            return nullptr;
        }

        cur = cur->_next;
    }

    return cur;
}

template<typename T>
static size_t countNodesBackwards(
    const Node<T>* const node
) {
    assert(node != nullptr);

    size_t result = 0;

    const Node<T>* cur = node;

    while(cur != nullptr) {
        cur = cur->_prev;
        result += 1;
    }

    return result;
}

template<typename T>
static Node<T>* const walkNodesBackwards(
    Node<T>* const node,
    const uintmax_t n
) {
    assert(countNodesBackwards(node) >= n);

    Node<T>* cur = node;

    for(uint64_t i = 0; i < n; i++) {
        if(cur == nullptr) {
            return nullptr;
        }

        cur = cur->_prev;
    }

    return cur;
}

template<typename T>
static const Node<T>* const walkNodesBackwards(
    const Node<T>* const node,
    const uintmax_t n
) {
    assert(countNodesBackwards(node) >= n);

    Node<T>* cur = node;

    for(uint64_t i = 0; i < n; i++) {
        if(cur == nullptr) {
            return nullptr;
        }

        cur = cur->_prev;
    }

    return cur;
}

template<typename T>
static bool doNodesMeet(
    const Node<T>* const front,
    const Node<T>* const back
) {
    assert(front != nullptr);
    assert(back != nullptr);
    assert(!isLoopingNodePath(front));

    const Node<T>* cur = front;

    while(cur->_next != nullptr) {
        if(cur == back) {
            return true;
        }

        cur = cur->_next;
    }

    if(cur == back) {
        return true;
    }

    return false;
}

template<typename T>
static bool isLoopingNodePath(
    const Node<T>* const front
) {
    assert(front != nullptr);

    const Node<T>* cur = front;

    ArrayList<const Node<T>*> known(5);

    while(cur->_next != nullptr) {
        for(size_t i = 0; i < known.len; i++) {
            if(cur == known.ptr[i]) {
                return true;
            }
        }

        known.append(cur);

        cur = cur->_next;
    }

    return false;
}

template<typename T>
static bool containsNode(
    const Node<T>* const front,
    const Node<T>* const node
) {
    const Node<T>* cur = front;

    while(cur != nullptr) {
        if(cur == node) {
            return true;
        }

        cur = cur->_next;
    }

    return false;
}

template<typename T>
struct LinkedList {
    Node<T>* _front;
    Node<T>* _back;
    uintmax_t _len;

    static inline LinkedList<T> init() {
        return {._front = nullptr, ._back = nullptr, ._len = 0};
    }

    inline void free() {
        assert(isValid());

        freeNodes(_front);
    }

    inline void append(
        const T& item
    ) {
        assert(isValid());

        auto node = Node<T>::alloc();

        if(_len == 0) {
            node->_prev = nullptr;
            node->_next = nullptr;
            node->_value = item;

            _front = node;
            _back = node;

            _len = 1;

            assert(isValidFilled());
            return;
        }

        connectNodes(_back, node);
        node->_next = nullptr;
        node->_value = item;

        _back = node;

        _len += 1;

        assert(isValidFilled());
    }

    inline void appendMulti(
        const T* items,
        const uintmax_t item_c
    ) {
        for(uintmax_t i = 0; i < item_c; i++) {
            append(items[i]);
        }
    }

    inline void appendNodes(
        const LinkedList<T>& items
    ) {
        assert(isValid());
        assert(items.isValid());

        if(items._len == 0) {
            return;
        }

        if(_len == 0) {
            _front = items._front;
            _back = items._back;
            _len = items._len;

            assert(isValidFilled());
            return;
        }

        connectNodes(_back, items._front);
        _back = items._back;
        _len += items._len;

        assert(isValidFilled());
    }

    inline void empty() {
        assert(isValid());

        if(_len > 0) {
            freeNodes(_front);

            _front = nullptr;
            _back = nullptr;
            _len = 0;
        }

        assert(isValidEmpty());
    }

    inline void emptyUnowned() {
        assert(isValid());

        if(_len > 0) {
            _front = nullptr;
            _back = nullptr;
            _len = 0;
        }

        assert(isValidEmpty());
    }

    inline void eraseNode(
        const Node<T>* const node
    ) {
        assert(isValidFilled());

        assert(containsNode(_front, node));

        if(_len == 1) {
            empty();

            assert(isValid());
            return;
        }

        if(node->_prev == nullptr) {
            assert(node == _front);

            _front = _front->_next;
            _len -= 1;

            ::free(const_cast<Node<T>* const>(node));

            assert(isValid());
            return;
        }

        if(node->_next == nullptr) {
            assert(node == _back);

            _back = node->_prev;
            _len -= 1;

            ::free(const_cast<Node<T>* const>(node));

            assert(isValid());
            return;
        }

        connectNodes(node->_prev, node->_next);
        _len -= 1;

        assert(isValid());
    }

    //  TODO: Optimize searching end
    inline void eraseNodes(
        Node<T>* const node,
        const uintmax_t n
    ) {
        assert(isValidFilled());

        assert(containsNode(_front, node));
        assert(countNodesForwards(node) >= n);

        if(n == 0) {
            return;
        }

        if(node->_prev == nullptr) {
            assert(node == _front);

            if(n == _len) {
                empty();

                assert(isValidEmpty());
                return;
            }

            Node<T>* const last_erased = walkNodesForwards(node, n - 1);

            _front = last_erased->_next;

            _len -= n;

            last_erased->_next = nullptr;
            freeNodes(node);

            assert(isValid());
            return;
        }

        Node<T>* const last_erased = walkNodesForwards(node, n - 1);

        if(last_erased->_next == nullptr) {
            _back = node->_prev;
            _back->_next = nullptr;

            _len -= n;

            freeNodes(node);

            assert(isValid());
            return;
        }

        connectNodes(node->_prev, last_erased->_next);

        _len -= n;

        assert(isValid());
    }

    inline void replaceNode(
        const Node<T>* const node,
        const LinkedList<T>& items
    ) {
        assert(isValidFilled());
        assert(items.isValid());

        assert(containsNode(_front, node));

        if(items._len == 0) {
            eraseNode(node);

            return;
        }

        if(_len == 1) {
            ::free(_front);

            _front = items._front;
            _back = items._back;
            _len = items._len;

            assert(isValidFilled());
            return;
        }

        if(node->_prev == nullptr) {
            assert(node == _front);

            connectNodes(items._back, _front->_next);

            ::free(_front);

            _front = items._front;
            _len += items._len - 1;

            assert(isValidFilled());
            return;
        }

        if(node->_next == nullptr) {
            assert(node == _back);

            connectNodes(_back->_prev, items._front);

            ::free(_back);

            _back = items._back;
            _len += items._len - 1;

            assert(isValidFilled());
            return;
        }

        connectNodes(node->_prev, items._front);
        connectNodes(items._back, node->_next);

        _len += items._len - 1;

        ::free(const_cast<Node<T>* const>(node));

        assert(isValidFilled());
    }

    inline bool isValidFilled() const {
        bool fine = true;

        fine = _front != nullptr;
        fine = _back != nullptr;
        fine = _len > 0;

        fine = _front->_prev == nullptr;
        fine = _back->_next == nullptr;

        fine = !isLoopingNodePath(_front);

        if(_len > 1) {
            fine = doNodesMeet(_front, _back);
        }

        fine = countNodesForwards(_front) == _len;

        return fine;
    }

    inline bool isValidEmpty() const {
        bool fine = true;

        fine = _front == nullptr;
        fine = _back == nullptr;

        return fine;
    }

    inline bool isValid() const {
        if(_len == 0) {
            return isValidEmpty();
        } else {
            return isValidFilled();
        }
    }
};

}
