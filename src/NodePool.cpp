#include "NodePool.h"

NodePool::NodePool(int _capacity) : capacity(_capacity), usedCount(0) {
    storage = new LinkedList_Pooled::Node[_capacity]; // single heap allocation of _capacity Node objects, laid out contiguosly in memory

    for (int i = 0; i < _capacity; i++) {
        storage[i].next = &storage[i+1];
    }
    storage[_capacity - 1].next = nullptr;
    freeList = storage;
}

LinkedList_Pooled::Node* NodePool::allocate() {
    if (freeList == nullptr) return nullptr; // ran out of memory
    LinkedList_Pooled::Node* n = freeList;
    freeList = n->next;
    usedCount++;
    return n;
}

void NodePool::deallocate(LinkedList_Pooled::Node* n) {
    n->next = freeList;
    freeList = n;
    usedCount--;
}

NodePool::~NodePool() {
    delete[] storage;
}