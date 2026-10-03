#pragma once

class NodePool; // forward declare

class LinkedList_Pooled {
    private:
        struct Node {
            int value;
            Node* prev;
            Node* next;
            bool inUse = false;
        };

        NodePool& pool;
        Node* head;
        Node* tail;
        int size;
    
    public:
        LinkedList_Pooled(NodePool& p);
        ~LinkedList_Pooled(); // free all nodes

        void insertFront(int value);
        void insertBack(int value);
        bool deleteValue(int value);
        void print() const;

        int getSize() const { return size; }
        LinkedList_Pooled::Node* getHead() const { return head; }

        void clear();
        
        friend class NodePool;
        friend class Visualizer;
};