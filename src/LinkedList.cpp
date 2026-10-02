#include "LinkedList.h"
#include <iostream>

LinkedList::LinkedList() : head(nullptr), tail(nullptr), size(0) {}

void LinkedList::insertFront(int value) {
    Node *n = new Node { value, nullptr, head }; // n is local variable that disappears when out of scope
    
    if (head != nullptr) {
        head->prev = n;
    }
    else {
        tail = n;
    }

    head = n;
    size++;
}

void LinkedList::print() const {
    Node *cur = head;
    while (cur != nullptr) {
        std::cout << cur->value << "<->";
        cur = cur->next;
    }
    std::cout << "Null\n";
}