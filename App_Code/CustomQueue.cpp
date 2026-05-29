#include "../Structures/CustomQueue.h"
#include <stdexcept>

using namespace std;

CustomQueue::CustomQueue() {
    frontPtr = rearPtr = nullptr;      
}


CustomQueue::~CustomQueue() {    
    while (!isEmpty()) {
        dequeue();        
    }
}


void CustomQueue::enqueue(int value) {
    Node* newNode = new Node(value);

    if (rearPtr == nullptr) {
        frontPtr = rearPtr = newNode;          
    } else {

        rearPtr->next = newNode;  
        rearPtr = newNode;        
    }
}


int CustomQueue::dequeue() {
    if (isEmpty()) {
        throw runtime_error("Cannot dequeue from empty queue");
    }
    
    Node* temp = frontPtr;   
    int value = temp->data;
    frontPtr = frontPtr->next;
    
    if (frontPtr == nullptr) {
        rearPtr = nullptr;
    }
    
    delete temp;
    temp = nullptr;     
    
    return value;
}

bool CustomQueue::isEmpty(){
    return frontPtr == nullptr;
}