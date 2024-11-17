#pragma once

#include "array_list.hpp"

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <strings.h>

namespace Tesix {

template<typename T>
struct Node {
    Node<T>* prev = nullptr;
    Node<T>* next = nullptr;
    T value;
};

template<typename T>
static inline Node<T>* allocNode() {
    Node<T>* node = static_cast<Node<T>*>(malloc(sizeof(Node<T>)));

    node->next = nullptr;
    node->prev = nullptr;

    return node;
}

template<typename T>
static void freeNodes(Node<T>* node) {
    if(node == nullptr) {
        return;
    }

    Node<T>* next = node->next;

    free(node);

    freeNodes(next);
}

template<typename T>
static size_t getNodePathLenForwards(Node<T>* node) {
    assert(node != nullptr);

    size_t result = 1;

    while(node->next != nullptr) {
        node = node->next;
        result += 1;
    }

    return result;
}

template<typename T>
static size_t getNodePathLenBackwards(Node<T>* node) {
    assert(node != nullptr);

    size_t result = 1;

    while(node->prev != nullptr) {
        node = node->prev;
        result += 1;
    }

    return result;
}

template<typename T>
static std::pair<Node<T>*, size_t> getPathBackAndLen(Node<T>* front) {
    Node<T>* cur = front;

    size_t lenght = 1;

    while(cur->next != nullptr) {
        cur = cur->next;
        lenght++;
    }

    return std::pair<Node<T>*, size_t>(cur, lenght);
}

template<typename T>
static bool doNodesMeet(Node<T>* front, Node<T>* back) {
    Node<T>* cur = front;

    while(cur->next != nullptr) {
        if(cur == back) {
            return true;
        }

        cur = cur->next;
    }

    if(cur == back) {
        return true;
    }

    return false;
}

template<typename T>
static inline Node<T>* backNode(Node<T>* node) {
    assert(node != nullptr);

    if(node->next != nullptr) {
        node = node->next;
    }

    return node;
}

template<typename T>
static inline Node<T>* frontNode(Node<T>* node) {
    assert(node != nullptr);

    if(node->prev != nullptr) {
        node = node->prev;
    }

    return node;
}

template<typename T>
static inline Node<T>* getNodeForwards(Node<T>* node, const size_t n) {
    assert(node != nullptr);
    assert(getNodePathLenForwards(node) > n);

    for(size_t i = 0; i < n; i++) {
        node = node->next;
    }

    return node;
}

template<typename T>
static inline Node<T>* getNodeBackwards(Node<T>* node, const size_t n) {
    assert(node != nullptr);
    assert(getNodePathLenBackwards(node) > n);

    for(size_t i = 0; i < n; i++) {
        node = node->prev;
    }

    return node;
}

template<typename T>
static inline void connectNodes(Node<T>* prev, Node<T>* next) {
    assert(prev != nullptr);
    assert(next != nullptr);

    prev->next = next;
    next->prev = prev;
}

template<typename T>
[[clang::always_inline]] static inline void disconnectNodeNext(Node<T>* node) {
    assert(node != nullptr);

    node->next = nullptr;
}

template<typename T>
[[clang::always_inline]] static inline void disconnectNodePrev(Node<T>* node) {
    assert(node != nullptr);

    node->prev = nullptr;
}

template<typename T>
static Node<T>* extractNodeRange(Node<T>* prev, Node<T>* next) {
    assert(prev != next);
    assert(prev->next != next);
    assert(doNodesMeet(prev, next));

    Node<T>* extracted = prev->next;

    disconnectNodePrev(prev->next);
    disconnectNodeNext(next->prev);

    connectNodes(prev, next);

    assert(extracted->prev == nullptr);
    assert(!isLoopingNodePath(extracted));

    return extracted;
}

template<typename T>
static bool isLoopingNodePath(Node<T>* front) {
    Node<T>* cur = front;

    ArrayList<Node<T>*, Dynamic> known(5);

    while(cur->next != nullptr) {
        for(size_t i = 0; i < known.len; i++) {
            if(cur == known.ptr[i]) {
                return true;
            }
        }

        known.append(cur);

        cur = cur->next;
    }

    return false;
}

template<typename T>
struct LinkedList {
    Node<T>* front = nullptr;
    Node<T>* back = nullptr;
    size_t len = 0;

    LinkedList() = default;
    ~LinkedList() {
        freeNodes(front);
    }

    void init() {
        assert(!isInited());
        assert(len == 0);

        front = allocNode<T>();
        back = allocNode<T>();

        connectNodes(front, back);

        len = 0;
    }

    inline bool isInited() {
        return front != nullptr && back != nullptr && doNodesMeet(front, back);
    }

    bool isEmpty() {
        assert(isInited());

        return front->next == back;
    }

    T& get(const size_t index) {
        assert(len > index);

        return getNode(index + 1)->value;
    }

    Node<T>* getNode(const size_t index) {
        assert(front != nullptr && back != nullptr);

        if(len - index < index) {
            return getNodeBackwards(back, len - index + 1);
        } else {
            return getNodeForwards(front, index);
        }
    }

    void append(const T& value) {
        assert(isInited());

        Node<T>* new_node = allocNode<T>();

        new_node->value = value;

        connectNodes(back->prev, new_node);
        connectNodes(new_node, back);

        len++;

        assert(!isLoopingNodePath(front));
    }

    void insert(const T& value, const size_t index) {
        assert(isInited());
        assert(len >= index);

        Node<T>* item = allocNode<T>();
        item->value = value;

        Node<T>* node = getNode(index);

        connectNodes(item, node->next);
        connectNodes(node, item);

        len += 1;

        assert(!isLoopingNodePath(front));
    }

    void insertAtNode(const T& value, Node<T>* node) {
        assert(node != nullptr);
        assert(node->next != nullptr);

        Node<T>* next = node->next;

        Node<T>* item = allocNode<T>();
        item->value = value;

        connectNodes(node, item);
        connectNodes(item, next);

        len += 1;

        assert(!isLoopingNodePath(front));
    }

    void insertNodes(Node<T>* items_front, const size_t index) {
        assert(isInited());
        assert(items_front != nullptr);
        assert(len > index);

        std::pair<Node<T>*, size_t> items_back_and_len = getPathBackAndLen(items_front);

        Node<T>* items_back = items_back_and_len.first;
        size_t item_count = items_back_and_len.second;

        Node<T>* prev = getNode(index);
        Node<T>* next = prev->next;

        connectNodes(prev, items_front);
        connectNodes(items_front, next);

        len += item_count;

        assert(!isLoopingNodePath(front));
    }

    void emplaceNodesAtNode(Node<T>* items_front, Node<T>* node) {
        assert(items_front != nullptr);
        assert(node != nullptr);
        assert(node->prev != nullptr);
        assert(node->next != nullptr);

        auto back_and_len = getPathBackAndLen(items_front);
        Node<T>* items_back = back_and_len.first;
        const size_t item_c = back_and_len.second;

        connectNodes(node->prev, items_front);
        connectNodes(items_back, node->next);

        free(node);

        len += item_c - 1;
    }

    void pop(const size_t index) {
        assert(isInited());
        assert(len > index);

        Node<T>* prev = getNode(index);
        Node<T>* next = getNode(index + 2);

        free(prev->next);

        connectNodes(prev, next);

        len--;

        assert(!isLoopingNodePath(front));
    }

    void popFront() {
        assert(isInited());
        assert(len > 0);

        Node<T>* node = front->next;

        connectNodes(front, front->next->next);

        free(node);

        len--;

        assert(!isLoopingNodePath(front));
    }

    void popBack() {
        assert(isInited());
        assert(len > 0);

        Node<T>* node = back->prev;

        connectNodes(back->prev->prev, back);

        free(node);

        len--;

        assert(!isLoopingNodePath(front));
    }

    void erase(const size_t index, const size_t n) {
        assert(isInited());
        assert(len >= index + n);

        if(n == 0) {
            return;
        }

        Node<T>* range_prev = getNode(index);
        Node<T>* range_next = getNode(index + 1 + n);

        Node<T>* extracted = extractNodeRange(range_prev, range_next);

        freeNodes(extracted);

        len -= n;

        assert(!isLoopingNodePath(front));
    }

    void eraseNode(Node<T>* node) {
        assert(node != nullptr);
        assert(node->prev != nullptr && node->next != nullptr);

        connectNodes(node->prev, node->next);

        len -= 1;

        free(node);
    }

    void move(const size_t from, const size_t n, const size_t to) {
        assert(len >= to);
        assert(len >= from + n);
        assert(to < from || to > from + n);
        assert(to != from);

        Node<T>* items_front = extractNodeRange(getNode(from), getNode(from + n + 1));
        Node<T>* items_back = backNode(items_front);

        Node<T>* prev;

        if(to <= from) {
            prev = getNode(to);
        } else {
            prev = getNode(to - n);
        }

        Node<T>* next = prev->next;

        connectNodes(prev, items_front);
        connectNodes(items_back, next);

        assert(!isLoopingNodePath(front));
        assert(getNodePathLenForwards(front) == len + 2);
    }

    Node<T>* first() {
        assert(isInited());
        assert(!isEmpty());

        return front->next;
    }

    Node<T>* last() {
        assert(isInited());
        assert(!isEmpty());

        return back->prev;
    }

    Node<T>* take() {
        Node<T>* items_front = first();

        disconnectNodePrev(items_front);
        disconnectNodeNext(last());

        connectNodes(front, back);

        return items_front;
    }
};

} // namespace Tesix
