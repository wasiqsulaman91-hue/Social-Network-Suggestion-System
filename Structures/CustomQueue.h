#ifndef CUSTOMQUEUE_H
#define CUSTOMQUEUE_H

class Node {
public:
    int data;               
    Node* next;      
    
    Node(int value) {
        data = value;       
        next = nullptr;    
    }
};

class CustomQueue {
private:
    Node* frontPtr;  
    Node* rearPtr;   
    
public:
    CustomQueue(); 
    ~CustomQueue();
    
    void enqueue(int value);
    int dequeue();
    bool isEmpty();
};

#endif 