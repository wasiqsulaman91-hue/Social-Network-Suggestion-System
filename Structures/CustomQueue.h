#ifndef CUSTOMQUEUE_H
#define CUSTOMQUEUE_H

struct QueueNode {
    int data;               
    QueueNode* next;      
    

    QueueNode(int value) {
        data = value;       
        next = nullptr;    
    }
};

class CustomQueue {
private:
    QueueNode* frontPtr;  
    QueueNode* rearPtr;   
    
public:
    CustomQueue(); 
    ~CustomQueue();
    
    void enqueue(int value);
    int dequeue();
    bool isEmpty();
};

#endif 