#include "LinkedList.h"
#include "LinkedList_Pooled.h"
#include "NodePool.h"
#include <iostream>

void Test1() {
    LinkedList l;

    // 1. Delete from empty list -> should return false, not crash
    std::cout << l.deleteValue(42) << "\n";  // 0

    // 2. Single-node insert/delete
    l.insertFront(1);
    l.deleteValue(1);
    l.print();  // NULL

    // 3. Delete head, middle, tail of a 3-node list
    l.insertBack(10);
    l.insertBack(20);
    l.insertBack(30);
    l.deleteValue(10);  // head
    l.deleteValue(20);  // middle
    l.deleteValue(30);  // tail
    l.print();          // NULL

    // 4. Size is actually tracked
    for (int i = 0; i < 5; i++) l.insertBack(i);
    for (int i = 0; i < 5; i++) l.deleteValue(i);
    std::cout << "size=" << l.getSize() << "\n";  // should be 0
}

void Test2() {
    NodePool pool(10);

    {
        LinkedList_Pooled list(pool);
        list.insertBack(10);
        list.insertBack(20);
        list.insertBack(30);
        std::cout << "after inserts, pool used: " << pool.getUsed() << "\n";   // 3
    }   // list destroyed here

    std::cout << "after scope exit, pool used: " << pool.getUsed() << "\n";    // 0 (Option B)
    std::cout << "pool capacity: " << pool.getCapacity() << "\n";              // 10
}

// int main() {
//     //Test1();
//     //Test2();
//     return 0;
// }