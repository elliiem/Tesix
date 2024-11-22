#pragma once

#include "array_list.hpp"

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <strings.h>
#include <utility>

namespace Tesix {

template<typename T>
struct Node {
    Node<T>* _prev = nullptr;
    Node<T>* _next = nullptr;
    T _value;
};

template<typename T>
struct Nodes {
    Node<T>* _front;
    Node<T>* _back;
    size_t _len;
};

template<typename T>
static inline Node<T>* allocNode() {
    Node<T>* node = static_cast<Node<T>*>(malloc(sizeof(Node<T>)));

    node->_next = nullptr;
    node->_prev = nullptr;

    return node;
}

template<typename T>
static void freeNodes(Node<T>* node) {
    if(node == nullptr) {
        return;
    }

    Node<T>* next = node->_next;

    free(node);

    freeNodes(next);
}

template<typename T>
static bool doNodesMeet(const Node<T>* front, const Node<T>* back) {
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
static Nodes<T> cloneNodes(Nodes<T> nodes) {
    assert(doNodesMeet(nodes._front, nodes._back));

    const Node<T>* cur = nodes._front;

    Node<T>* cloned_front = allocNode<T>();
    cloned_front->_value = cur->_value;

    Node<T>* cloned_cur = cloned_front;
    Node<T>* cloned_prev = nullptr;

    while(cur != nodes._back) {
        cloned_cur->_next = allocNode<T>();
        cloned_cur->_prev = cloned_prev;

        cloned_cur->_value = cur->_value;

        cur = cur->_next;

        cloned_prev = cloned_cur;
        cloned_cur = cloned_cur->_next;
    }

    return {
        .front = cloned_front,
        .back = cloned_cur,
        .len = nodes._len,
    };
}

template<typename T>
static size_t getNodePathLenForwards(Node<T>* node) {
    assert(node != nullptr);

    size_t result = 1;

    while(node->_next != nullptr) {
        node = node->_next;
        result += 1;
    }

    return result;
}

template<typename T>
static size_t getNodePathLenBackwards(Node<T>* node) {
    assert(node != nullptr);

    size_t result = 1;

    while(node->_prev != nullptr) {
        node = node->_prev;
        result += 1;
    }

    return result;
}

template<typename T>
static std::pair<Node<T>*, size_t> getPathBackAndLen(Node<T>* front) {
    Node<T>* cur = front;

    size_t lenght = 1;

    while(cur->_next != nullptr) {
        cur = cur->_next;
        lenght++;
    }

    return std::pair<Node<T>*, size_t>(cur, lenght);
}

template<typename T>
static inline Node<T>* backNode(Node<T>* node) {
    assert(node != nullptr);

    if(node->_next != nullptr) {
        node = node->_next;
    }

    return node;
}

template<typename T>
static inline Node<T>* frontNode(Node<T>* node) {
    assert(node != nullptr);

    if(node->_prev != nullptr) {
        node = node->_prev;
    }

    return node;
}

template<typename T>
static inline Node<T>* getNodeForwards(Node<T>* node, const size_t n) {
    assert(node != nullptr);
    assert(getNodePathLenForwards(node) > n);

    for(size_t i = 0; i < n; i++) {
        node = node->_next;
    }

    return node;
}

template<typename T>
static inline const Node<T>* getNodeForwards(const Node<T>* node, const size_t n) {
    assert(node != nullptr);
    assert(getNodePathLenForwards(node) > n);

    for(size_t i = 0; i < n; i++) {
        node = node->_next;
    }

    return node;
}

template<typename T>
static inline Node<T>* getNodeBackwards(Node<T>* node, const size_t n) {
    assert(node != nullptr);
    assert(getNodePathLenBackwards(node) > n);

    for(size_t i = 0; i < n; i++) {
        node = node->_prev;
    }

    return node;
}

template<typename T>
static inline const Node<T>* getNodeBackwards(const Node<T>* node, const size_t n) {
    assert(node != nullptr);
    assert(getNodePathLenBackwards(node) > n);

    for(size_t i = 0; i < n; i++) {
        node = node->_prev;
    }

    return node;
}

template<typename T>
static inline void connectNodes(Node<T>* prev, Node<T>* next) {
    assert(prev != nullptr);
    assert(next != nullptr);

    prev->_next = next;
    next->_prev = prev;
}

template<typename T>
static inline void disconnectNodeNext(Node<T>* node) {
    assert(node != nullptr);

    node->_next = nullptr;
}

template<typename T>
static inline void disconnectNodePrev(Node<T>* node) {
    assert(node != nullptr);

    node->_prev = nullptr;
}

template<typename T>
static Node<T>* extractNodeRange(Node<T>* prev, Node<T>* next) {
    assert(prev != next);
    assert(prev->_next != next);
    assert(doNodesMeet(prev, next));

    Node<T>* extracted = prev->_next;

    disconnectNodePrev(prev->_next);
    disconnectNodeNext(next->_prev);

    connectNodes(prev, next);

    assert(extracted->_prev == nullptr);
    assert(!isLoopingNodePath(extracted));

    return extracted;
}

template<typename T>
static bool isLoopingNodePath(Node<T>* front) {
    Node<T>* cur = front;

    ArrayList<Node<T>*, Dynamic> known(5);

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
void emplaceNodes(Node<T>* items_front, Node<T>* items_back, Node<T>* node) {
    assert(items_front != nullptr);
    assert(items_back != nullptr);

    assert(node != nullptr);
    assert(node->_prev != nullptr);
    assert(node->_next != nullptr);

    connectNodes(node->_prev, items_front);
    connectNodes(items_back, node->_next);

    free(node);
}

template<typename T>
struct LinkedList {
    Node<T>* _front = nullptr;
    Node<T>* _back = nullptr;
    size_t _len = 0;

    LinkedList() = default;
    ~LinkedList() {
        freeNodes(_front);
    }

    void init() {
        assert(!isInited());
        assert(_len == 0);

        _front = allocNode<T>();
        _back = allocNode<T>();

        connectNodes(_front, _back);

        _len = 0;
    }

    void append(const T& value) {
        assert(isInited());

        Node<T>* new_node = allocNode<T>();

        new_node->_value = value;

        connectNodes(_back->_prev, new_node);
        connectNodes(new_node, _back);

        _len++;

        assert(!isLoopingNodePath(_front));
    }

    //  TODO: Implement
    void appendMulti() {
        throw std::runtime_error("Not Implemented!");
    }

    void insert(const T& value, const size_t index) {
        assert(isInited());
        assert(_len >= index);

        Node<T>* item = allocNode<T>();
        item->_value = value;

        Node<T>* node = getNode(index);

        connectNodes(item, node->_next);
        connectNodes(node, item);

        _len += 1;

        assert(!isLoopingNodePath(_front));
    }

    //  TODO: Implement
    void insertMulti() {
        throw std::runtime_error("Not Implemented!");
    }

    void nodeAppend(const T& value, Node<T>* node) {
        assert(node != nullptr);
        assert(node->_next != nullptr);

        Node<T>* next = node->_next;

        Node<T>* item = allocNode<T>();
        item->_value = value;

        connectNodes(node, item);
        connectNodes(item, next);

        _len += 1;

        assert(!isLoopingNodePath(_front));
    }

    void nodeAppendMulti(Nodes<T>& items, Node<T>* node) {
        assert(isInited());
        assert(items._front != nullptr);

        connectNodes(node, items._front);
        connectNodes(items._back, node->_next);

        _len += items._len;

        assert(!isLoopingNodePath(_front));
    }

    void nodeReplaceMulti(Nodes<T>& items, Node<T>* node) {
        emplaceNodes(items._front, items._back, node);

        _len += items._len;
    }

    void popFront() {
        assert(isInited());
        assert(_len > 0);

        Node<T>* node = _front->_next;

        connectNodes(_front, _front->_next->next);

        free(node);

        _len--;

        assert(!isLoopingNodePath(_front));
    }

    void popBack() {
        assert(isInited());
        assert(_len > 0);

        Node<T>* node = _back->_prev;

        connectNodes(_back->_prev->prev, _back);

        free(node);

        _len--;

        assert(!isLoopingNodePath(_front));
    }

    void erase(const size_t index, const size_t n) {
        assert(isInited());
        assert(_len >= index + n);

        if(n == 0) {
            return;
        }

        Node<T>* range_prev = getNode(index);
        Node<T>* range_next = getNode(index + 1 + n);

        Node<T>* extracted = extractNodeRange(range_prev, range_next);

        freeNodes(extracted);

        _len -= n;

        assert(!isLoopingNodePath(_front));
    }

    void nodeErase(Node<T>* node) {
        assert(node != nullptr);
        assert(node->_prev != nullptr && node->_next != nullptr);

        connectNodes(node->_prev, node->_next);

        _len -= 1;

        free(node);
    }

    //  TODO: Implement
    void nodeEraseMulti() {

    }

    T& get(const size_t index) {
        assert(_len > index);

        return getNode(index + 1)->value;
    }

    T get(const size_t index) const {
        assert(_len > index);

        return getNode(index + 1)->value;
    }

    Node<T>* first() {
        assert(isInited());
        assert(!isEmpty());

        return _front->_next;
    }

    const Node<T>* first() const {
        assert(isInited());
        assert(!isEmpty());

        return _front->_next;
    }

    Node<T>* last() {
        assert(isInited());
        assert(!isEmpty());

        return _back->_prev;
    }

    const Node<T>* last() const {
        assert(isInited());
        assert(!isEmpty());

        return _back->_prev;
    }

    Node<T>* getNode(const size_t index) {
        assert(_front != nullptr && _back != nullptr);

        if(_len - index < index) {
            return getNodeBackwards(_back, _len - index + 1);
        } else {
            return getNodeForwards(_front, index);
        }
    }

    const Node<T>* getNode(const size_t index) const {
        assert(_front != nullptr && _back != nullptr);

        if(_len - index < index) {
            return getNodeBackwards(_back, _len - index + 1);
        } else {
            return getNodeForwards(_front, index);
        }
    }

    Nodes<T> takeItems() {
        const Nodes<T> ret = {
            ._front = first(),
            ._back = last(),
            ._len = _len,
        };

        connectNodes(_front, _back);
        _len = 0;

        return ret;
    }

    Nodes<T> cloneItems() {
        const auto ret = cloneNodes({.front = first(), .back = last(), .len = _len});

        connectNodes(_front, _back);
        _len = 0;

        return ret;
    }

    inline bool isInited() const {
        return _front != nullptr && _back != nullptr && doNodesMeet(_front, _back);
    }

    bool isEmpty() const {
        assert(isInited());

        return _front->_next == _back;
    }
};

} // namespace Tesix
