#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <string>

// ============================================================================
// STRUCT: FriendNode
// Purpose: Represents a single node in the adjacency list (singly linked list)
//          that stores one friendship connection for a particular user.
// ============================================================================
struct FriendNode {
    int friendID;           // The ID of the friend this node represents
    FriendNode* next;       // Pointer to the next friend in the list (nullptr if last)
    
    // Default constructor - initializes a new friend node with given ID
    FriendNode(int id) {
        friendID = id;      // Store the friend's user ID
        next = nullptr;     // Initially, this node points to nothing (end of list)
    }
};

// ============================================================================
// STRUCT: UserNode
// Purpose: Represents a single user (vertex) in the social network graph.
//          Contains user profile data, visual position, adjacency list head,
//          and pointer to next user in the master directory list.
// ============================================================================
struct UserNode {
    int id;                 // Unique identifier for this user
    std::string name;       // Display name of the user
    float x;                // X-coordinate for visual rendering on canvas
    float y;                // Y-coordinate for visual rendering on canvas
    FriendNode* friendsHead;// Head pointer to adjacency list (linked list of friends)
    UserNode* next;         // Pointer to next user in master user directory list
    
    // Default constructor - initializes a new user with given parameters
    UserNode(int userId, std::string userName, float posX, float posY) {
        id = userId;        // Assign unique user ID
        name = userName;    // Assign user's display name
        x = posX;           // Set horizontal screen position
        y = posY;           // Set vertical screen position
        friendsHead = nullptr;  // Initially, user has no friends (empty adjacency list)
        next = nullptr;     // Initially, not linked to any other user in directory
    }
};

// ============================================================================
// STRUCT: Suggestion
// Purpose: Represents a single "People You May Know" recommendation result.
//          Stores the suggested user's ID and the count of mutual friends.
// ============================================================================
struct Suggestion {
    int userID;             // The ID of the user being suggested
    int mutualCount;        // Number of mutual friends shared with active user
    
    // Default constructor - initializes empty suggestion
    Suggestion() {
        userID = -1;        // -1 indicates invalid/uninitialized suggestion
        mutualCount = 0;    // No mutual connections initially
    }
    
    // Parameterized constructor - initializes suggestion with specific values
    Suggestion(int id, int count) {
        userID = id;        // Set the suggested user's ID
        mutualCount = count;// Set the mutual friend count
    }
};

#endif // STRUCTURES_H