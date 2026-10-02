#pragma once

struct Node {
    int value;
    Node *prev;
    Node *next;
};

class LinkedList {
    private:
        Node *head;
        Node *tail;
        int size;
    
    public:
        LinkedList();
        //~LinkedList(); // free all nodes

        void insertFront(int value);
        void insertBack(int value);
        bool deleteValue(int value);
        void print() const;

        int getSize() const { return size; }
};