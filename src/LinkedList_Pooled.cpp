#include "LinkedList_Pooled.h"
#include "NodePool.h"
#include <iostream>

LinkedList_Pooled::LinkedList_Pooled(NodePool& p) : pool(p), head(nullptr), tail(nullptr), size(0) {}

void LinkedList_Pooled::insertFront(int value) {
    Node* n = pool.allocate();
    n->value = value;
    n->prev = nullptr;
    n->next = head;
    
    if (head != nullptr) head->prev = n;
    else tail = n;

    head = n;
    size++;
}

void LinkedList_Pooled::insertBack(int value) {
    Node* n = pool.allocate();
    if (n == nullptr) {
        std::cerr << "Pool exhausted\n";
        return;
    }
    n->value = value;
    n->prev = tail;
    n->next = nullptr;

    if (tail != nullptr) tail->next = n;
    else head = n;

    tail = n;
    size++;
}

bool LinkedList_Pooled::deleteValue(int value) {
    Node* itr = head;

    while (itr != nullptr) {
        if (itr->value == value) {
            if (itr->prev != nullptr) itr->prev->next = itr->next;
            else head = itr->next;

            if (itr->next != nullptr) itr->next->prev = itr->prev;
            else tail = itr->prev;

            pool.deallocate(itr);
            size--;
            return true;
        }
        itr = itr->next;
    }
    return false;
}

LinkedList_Pooled::~LinkedList_Pooled() {
    // Ownership: in C++, "who is responsible for freeing this?" must always have a clear answer.
    // NodePool owns the storage array -> its destructor calls delete[]
    // LinkedList_Pooled borrows nodes -> calls deallocate() to return them
    // LinkedList_Pooled borrows the pool itself (reference) → must not delete it (or will result in double free)

    Node* itr = head;
    while (itr != nullptr) {
        Node* next = itr->next;
        pool.deallocate(itr);   // <- deallocate, NOT delete
        itr = next;
    }
    
    head = tail = nullptr;
    size = 0;
}

void LinkedList_Pooled::print() const {
    Node* cur = head;
    while (cur != nullptr) {
        std::cout << cur->value << "<->";
        cur = cur->next;
    }
    std::cout << "Null\n";
}

void LinkedList_Pooled::clear() {
    Node* itr = head;
    while (itr) {
        Node* next = itr->next;
        pool.deallocate(itr);
        itr = next;
    }
    head = tail = nullptr;
    size = 0;
}