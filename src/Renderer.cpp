#include "Renderer.h"
#include <iostream>

void Renderer::drawList(const LinkedList_Pooled& list, int highlight) {
    std::cout << "List: ";
    LinkedList_Pooled::Node* cur = list.getHead();

    while(cur != nullptr) {
        if (cur->value == highlight) std::cout << Color::RED;
        else std::cout << Color::CYAN;
        std::cout << "[" << cur->value << "]" << Color::RESET;

        if (cur->next) std::cout << "<->";
        cur = cur->next;
    }
    std::cout << " -> NULL\n";
}