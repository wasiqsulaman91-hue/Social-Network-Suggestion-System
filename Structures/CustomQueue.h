#ifndef CUSTOMQUEUE_H
#define CUSTOMQUEUE_H

// ============================================================================
// STRUCT: QueueNode
// Purpose: Internal node structure for implementing a singly linked list
//          based queue. Each node holds one integer value and points to next.
// ============================================================================
struct QueueNode {
    int data;               // The integer value stored in this queue node
    QueueNode* next;        // Pointer to the next node in queue (nullptr if last)
    
    // Constructor to create a new queue node with given value
    QueueNode(int value) {
        data = value;       // Store the provided integer value
        next = nullptr;     // Initially points to nothing
    }
};

// ============================================================================
// CLASS: CustomQueue
// Purpose: A custom implementation of a FIFO queue using singly linked list.
//          Required because std::queue is forbidden in this project.
//          Supports basic enqueue, dequeue, and isEmpty operations.
// ============================================================================
class CustomQueue {
private:
    QueueNode* frontPtr;    // Pointer to the front node (dequeue end)
    QueueNode* rearPtr;     // Pointer to the rear node (enqueue end)
    
public:
    // Constructor declaration
    CustomQueue();
    
    // Destructor declaration
    ~CustomQueue();
    
    // Adds a new integer value to the rear of the queue
    void enqueue(int value);
    
    // Removes and returns the integer value from the front of the queue
    int dequeue();
    
    // Checks if the queue is currently empty
    bool isEmpty();
};

#endif // CUSTOMQUEUE_H