#ifndef NETWORKGRAPH_H
#define NETWORKGRAPH_H
#include "Structures.h"
#include <string>

using namespace std;

class NetworkGraph {
private:
    UserNode* masterHead;   
public:
    NetworkGraph();  
    ~NetworkGraph();
    
    void addUser(int id, string name, float x, float y);
    void addFriendship(int id1, int id2);
    void removeFriendship(int id1, int id2);
    void removeUser(int targetID);
    UserNode* findUser(int targetID);
    UserNode* getMasterHead();
};

#endif