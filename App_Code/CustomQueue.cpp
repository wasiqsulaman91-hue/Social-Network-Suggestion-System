#include "../Structures/CustomQueue.h"
#include <stdexcept>

// ============================================================================
// CONSTRUCTOR: CustomQueue
// Purpose: Initializes an empty queue by setting front and rear pointers to null
// ============================================================================
CustomQueue::CustomQueue() {
    frontPtr = nullptr;     // No nodes exist yet, front is null
    rearPtr = nullptr;      // No nodes exist yet, rear is null
}

// ============================================================================
// DESTRUCTOR: ~CustomQueue
// Purpose: Cleans up all dynamically allocated queue nodes to prevent memory leaks
// ============================================================================
CustomQueue::~CustomQueue() {
    // Keep removing nodes from front until queue is completely empty
    while (!isEmpty()) {
        dequeue();          // Dequeue automatically deletes the front node
    }
}

// ============================================================================
// FUNCTION: enqueue
// Purpose: Adds a new integer value to the rear (back) of the queue
// Parameters: value - the integer to be added to queue
// ============================================================================
void CustomQueue::enqueue(int value) {
    // Step 1: Dynamically allocate a new queue node with the given value
    QueueNode* newNode = new QueueNode(value);
    
    // Step 2: Check if queue is currently empty
    if (rearPtr == nullptr) {
        // Queue is empty - new node becomes both front and rear
        frontPtr = newNode;     // Front points to the only node
        rearPtr = newNode;      // Rear also points to the same node
    } else {
        // Queue has existing nodes - attach new node to current rear
        rearPtr->next = newNode;  // Link current rear's next to new node
        rearPtr = newNode;        // Move rear pointer to the new node
    }
}

// ============================================================================
// FUNCTION: dequeue
// Purpose: Removes and returns the integer value from the front of the queue
// Returns: The integer value that was at the front
// Throws: runtime_error if attempting to dequeue from empty queue
// ============================================================================
int CustomQueue::dequeue() {
    // Step 1: Safety check - verify queue is not empty
    if (isEmpty()) {
        throw std::runtime_error("Cannot dequeue from empty queue");
    }
    
    // Step 2: Store the front node pointer temporarily
    QueueNode* tempNode = frontPtr;
    
    // Step 3: Extract the data value from the front node
    int value = tempNode->data;
    
    // Step 4: Move front pointer to the next node in queue
    frontPtr = frontPtr->next;
    
    // Step 5: Check if queue became empty after removal
    if (frontPtr == nullptr) {
        // Queue is now empty - rear should also be null
        rearPtr = nullptr;
    }
    
    // Step 6: Delete the old front node to free memory
    delete tempNode;
    tempNode = nullptr;     // Good practice - avoid dangling pointer
    
    // Step 7: Return the extracted value
    return value;
}

// ============================================================================
// FUNCTION: isEmpty
// Purpose: Checks whether the queue currently contains any elements
// Returns: true if queue is empty, false otherwise
// ============================================================================
bool CustomQueue::isEmpty() {
    // Queue is empty if and only if front pointer is null
    return (frontPtr == nullptr);
}