#include "LinkedList.h"
#include <iostream>

int main() {
    LinkedList list;
    list.insertFront(30);
    list.insertFront(20);
    list.insertFront(10);
    list.print();

    return 0;
}