#include "../Structures/NetworkGraph.h"

// ============================================================================
// CONSTRUCTOR: NetworkGraph
// Purpose: Initializes an empty social network graph
// ============================================================================
NetworkGraph::NetworkGraph() {
    masterHead = nullptr;   // Initially, no users exist in the network
}

// ============================================================================
// DESTRUCTOR: ~NetworkGraph
// Purpose: Cleans up all dynamically allocated memory for users and friendships
// ============================================================================
NetworkGraph::~NetworkGraph() {
    // Step 1: Traverse the master user list and delete each user node
    UserNode* currentUser = masterHead;
    
    while (currentUser != nullptr) {
        // Step 2: For each user, first delete their entire adjacency list
        FriendNode* currentFriend = currentUser->friendsHead;
        
        while (currentFriend != nullptr) {
            // Save pointer to current friend node
            FriendNode* tempFriend = currentFriend;
            // Move to next friend in the list
            currentFriend = currentFriend->next;
            // Delete the saved friend node
            delete tempFriend;
            tempFriend = nullptr;
        }
        
        // Step 3: After adjacency list is cleared, delete the user node itself
        UserNode* tempUser = currentUser;
        // Move to next user in master list
        currentUser = currentUser->next;
        // Delete the saved user node
        delete tempUser;
        tempUser = nullptr;
    }
    
    // Step 4: Set master head to null (graph is now completely empty)
    masterHead = nullptr;
}

// ============================================================================
// FUNCTION: addUser
// Purpose: Creates and adds a new user vertex to the social network graph
// Parameters: id - unique user identifier
//             name - display name of the user
//             x, y - coordinates for visual representation
// ============================================================================
void NetworkGraph::addUser(int id, std::string name, float x, float y) {
    // Step 1: Dynamically allocate a new UserNode with provided parameters
    UserNode* newUser = new UserNode(id, name, x, y);
    
    // Step 2: Check if this is the first user being added to the graph
    if (masterHead == nullptr) {
        // Graph is empty - new user becomes the head of master list
        masterHead = newUser;
    } else {
        // Graph has existing users - append new user at the end of master list
        
        // Step 3: Traverse to find the last user in the master list
        UserNode* currentUser = masterHead;
        
        while (currentUser->next != nullptr) {
            // Keep moving forward until we find the last node
            currentUser = currentUser->next;
        }
        
        // Step 4: Attach new user to the end of the list
        currentUser->next = newUser;
    }
}

// ============================================================================
// FUNCTION: addFriendship
// Purpose: Creates a bidirectional friendship edge between two users
// Parameters: id1 - ID of the first user
//             id2 - ID of the second user
// Note: This implements an undirected edge by adding each user to the other's
//       adjacency list
// ============================================================================
void NetworkGraph::addFriendship(int id1, int id2) {
    // Step 1: Find the first user node using their ID
    UserNode* user1 = findUser(id1);
    
    // Step 2: Find the second user node using their ID
    UserNode* user2 = findUser(id2);
    
    // Step 3: Safety check - verify both users exist in the graph
    if (user1 == nullptr || user2 == nullptr) {
        // One or both users don't exist - cannot create friendship
        return;  // Exit function early
    }
    
    // Step 4: Add user2 to user1's adjacency list (friend list)
    
    // Create a new friend node representing user2
    FriendNode* friend1 = new FriendNode(id2);
    
    // Insert at the head of user1's adjacency list (constant time insertion)
    friend1->next = user1->friendsHead;  // New node points to current head
    user1->friendsHead = friend1;         // Update head to new node
    
    // Step 5: Add user1 to user2's adjacency list (making it bidirectional)
    
    // Create a new friend node representing user1
    FriendNode* friend2 = new FriendNode(id1);
    
    // Insert at the head of user2's adjacency list
    friend2->next = user2->friendsHead;  // New node points to current head
    user2->friendsHead = friend2;         // Update head to new node
}

// ============================================================================
// FUNCTION: removeFriendship
// Purpose: Removes a mutual friendship between two users
// ============================================================================
void NetworkGraph::removeFriendship(int id1, int id2) {
    // Step 1: Remove id2 from user1's friend list
    UserNode* user1 = findUser(id1);
    if (user1 != nullptr) {
        FriendNode* prev = nullptr;
        FriendNode* curr = user1->friendsHead;
        while (curr != nullptr) {
            if (curr->friendID == id2) {
                if (prev != nullptr) prev->next = curr->next;
                else user1->friendsHead = curr->next;
                delete curr;
                break;
            }
            prev = curr;
            curr = curr->next;
        }
    }
    
    // Step 2: Remove id1 from user2's friend list
    UserNode* user2 = findUser(id2);
    if (user2 != nullptr) {
        FriendNode* prev = nullptr;
        FriendNode* curr = user2->friendsHead;
        while (curr != nullptr) {
            if (curr->friendID == id1) {
                if (prev != nullptr) prev->next = curr->next;
                else user2->friendsHead = curr->next;
                delete curr;
                break;
            }
            prev = curr;
            curr = curr->next;
        }
    }
}

// ============================================================================
// FUNCTION: removeUser
// Purpose: Deletes a user and all their friendships from the graph
// ============================================================================
void NetworkGraph::removeUser(int targetID) {
    UserNode* userToDelete = findUser(targetID);
    if (userToDelete == nullptr) return;

    // Step 1: Remove this user from all their friends' friend lists
    FriendNode* currFriend = userToDelete->friendsHead;
    while (currFriend != nullptr) {
        UserNode* friendNode = findUser(currFriend->friendID);
        if (friendNode != nullptr) {
            FriendNode* prev = nullptr;
            FriendNode* curr = friendNode->friendsHead;
            while (curr != nullptr) {
                if (curr->friendID == targetID) {
                    if (prev != nullptr) prev->next = curr->next;
                    else friendNode->friendsHead = curr->next;
                    delete curr;
                    break;
                }
                prev = curr;
                curr = curr->next;
            }
        }
        currFriend = currFriend->next;
    }

    // Step 2: Delete all friend nodes of the userToDelete
    currFriend = userToDelete->friendsHead;
    while (currFriend != nullptr) {
        FriendNode* temp = currFriend;
        currFriend = currFriend->next;
        delete temp;
    }
    userToDelete->friendsHead = nullptr;

    // Step 3: Remove the user from the masterHead list
    UserNode* prevUser = nullptr;
    UserNode* currUser = masterHead;
    while (currUser != nullptr) {
        if (currUser->id == targetID) {
            if (prevUser != nullptr) prevUser->next = currUser->next;
            else masterHead = currUser->next;
            delete currUser;
            break;
        }
        prevUser = currUser;
        currUser = currUser->next;
    }
}

// ============================================================================
// FUNCTION: findUser
// Purpose: Searches for a user in the graph by their unique ID
// Parameters: targetID - the ID of the user to find
// Returns: Pointer to the UserNode if found, nullptr if not found
// ============================================================================
UserNode* NetworkGraph::findUser(int targetID) {
    // Step 1: Start traversal from the head of the master user list
    UserNode* currentUser = masterHead;
    
    // Step 2: Traverse the entire master list until we find the user or reach end
    while (currentUser != nullptr) {
        // Step 3: Check if current user's ID matches the target ID
        if (currentUser->id == targetID) {
            // Found the user - return pointer to this node
            return currentUser;
        }
        
        // Step 4: Move to the next user in the master list
        currentUser = currentUser->next;
    }
    
    // Step 5: Traversed entire list without finding user - return null
    return nullptr;
}

// ============================================================================
// FUNCTION: getMasterHead
// Purpose: Provides external access to the head of the master user list
// Returns: Pointer to the first user in the graph
// Note: Used by rendering and algorithm modules to traverse all users
// ============================================================================
UserNode* NetworkGraph::getMasterHead() {
    return masterHead;
}