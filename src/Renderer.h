#pragma once
#include "LinkedList_Pooled.h"
#include "NodePool.h"

namespace Color {
    const char* RESET  = "\033[0m";
    const char* RED    = "\033[31m";
    const char* GREEN  = "\033[32m";
    const char* YELLOW = "\033[33m";
    const char* BLUE   = "\033[34m";
    const char* CYAN   = "\033[36m";
    const char* BOLD   = "\033[1m";
    const char* CLEARSCREEN   = "\033[2J\033[H";
    const char* HIDECURSOR   = "\033[?25l";
    const char* SHOWCURSOR   = "\033[?25h";
}

class Renderer {
    public:
    static void clear();
    static void hideCursor();
    static void showCursor();

    static void drawList(const LinkedList_Pooled &list, int highlightValue = -1);
    static void drawNode(const NodePool &pool);
    static void frame(const LinkedList_Pooled &list, const NodePool &pool, int highlightValue = -1);
};