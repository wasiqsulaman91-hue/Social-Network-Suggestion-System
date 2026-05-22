#ifndef NETWORKGRAPH_H
#define NETWORKGRAPH_H

#include "Structures.h"
#include <string>

// ============================================================================
// CLASS: NetworkGraph
// Purpose: Represents the entire social network as an undirected graph.
//          Manages the master linked list of all users (vertices) and provides
//          operations to add users, create friendships (edges), and search.
// ============================================================================
class NetworkGraph {
private:
    UserNode* masterHead;   // Head pointer to the master linked list of all users
    
public:
    // Constructor declaration
    NetworkGraph();
    
    // Destructor declaration
    ~NetworkGraph();
    
    // Adds a new user (vertex) to the social network graph
    void addUser(int id, std::string name, float x, float y);
    
    // Creates a mutual friendship (undirected edge) between two users
    void addFriendship(int id1, int id2);
    
    // Searches for and returns a pointer to a user with the given ID
    UserNode* findUser(int targetID);
    
    // Returns the head pointer to the master user list (for external traversal)
    UserNode* getMasterHead();
};

#endif // NETWORKGRAPH_H