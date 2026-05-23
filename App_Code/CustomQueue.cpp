#include "../Structures/CustomQueue.h"
#include <stdexcept>

using namespace std;

CustomQueue::CustomQueue() {
    frontPtr = nullptr;    
    rearPtr = nullptr;  
}


CustomQueue::~CustomQueue() {
    
    while (!isEmpty()) {
        dequeue();        
    }
}


void CustomQueue::enqueue(int value) {
    QueueNode* newNode = new QueueNode(value);

    if (rearPtr == nullptr) {

        frontPtr = newNode;     
        rearPtr = newNode;     
    } else {

        rearPtr->next = newNode;  
        rearPtr = newNode;        
    }
}


int CustomQueue::dequeue() {
    if (isEmpty()) {
        throw runtime_error("Cannot dequeue from empty queue");
    }
    
    QueueNode* tempNode = frontPtr;
    
    int value = tempNode->data;
    

    frontPtr = frontPtr->next;
    
    if (frontPtr == nullptr) {
        rearPtr = nullptr;
    }
    
    delete tempNode;
    tempNode = nullptr;     
    
    return value;
}

bool CustomQueue::isEmpty() {
    return (frontPtr == nullptr);
}