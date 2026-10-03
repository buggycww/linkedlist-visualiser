#include "LinkedList.h"
#include <iostream>

LinkedList::LinkedList() : head(nullptr), tail(nullptr), size(0) {}

void LinkedList::insertFront(int value) {
    // 'n' (the pointer) dies at scope exit, but the Node on the heap survives
    // MUST 'delete' later, but not necessary here bc another pointer takes its value
    Node* n = new Node { value, nullptr, head };
    
    if (head != nullptr) head->prev = n;
    else tail = n;

    head = n;
    size++;
}

void LinkedList::insertBack(int value) {
    Node* n = new Node { value, tail, nullptr };

    if (tail != nullptr) tail->next = n;
    else head = n;

    tail = n;
    size++;
}

bool LinkedList::deleteValue(int value) {
    Node* itr = head;

    while (itr != nullptr) {
        if (itr->value == value) {
            if (itr->prev != nullptr) itr->prev->next = itr->next;
            else head = itr->next;

            if (itr->next != nullptr) itr->next->prev = itr->prev;
            else tail = itr->prev;

            delete itr;
            size--;
            return true;
        }
        itr = itr->next;
    }
    return false;
}

LinkedList::~LinkedList() {
    Node* itr = head;

    // looping thorugh size is fragile, since it relies on size to be correct
    // while loop more reliable
    while (itr != nullptr) { 
        Node* next = itr->next;
        delete itr;
        itr = next;
    }
    head = tail = nullptr;
    size = 0;
}

void LinkedList::print() const {
    Node* cur = head;
    while (cur != nullptr) {
        std::cout << cur->value << "<->";
        cur = cur->next;
    }
    std::cout << "Null\n";
}