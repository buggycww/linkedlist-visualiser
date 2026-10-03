// new, delete is slow and fragments memory
// use pool instead to pre-allocate a big block, hand out chunks, and recycle

#pragma once
#include "LinkedList_Pooled.h"

class NodePool {
    private:
        LinkedList_Pooled::Node* storage; // one big contiguos array of nodes
        LinkedList_Pooled::Node* freeList;
        int capacity;
        int usedCount;
    
    public:
        // 'explicit' prevents implicit conversions and copy-initialisation for constructors and conversion operators
        explicit NodePool(int capacity);  
        ~NodePool();

        LinkedList_Pooled::Node* allocate();
        void deallocate(LinkedList_Pooled::Node* n);

        int getCapacity() const { return capacity; }
        int getUsed() const { return usedCount; }
        bool isInUse(int index) const { return storage[index].inUse; }
        void reset();
};