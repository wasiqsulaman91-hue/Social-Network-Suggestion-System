#include "../Structures/NetworkGraph.h"
#include <string>

using namespace std;

NetworkGraph::NetworkGraph() {
    masterHead = nullptr;
}

NetworkGraph::~NetworkGraph(){
    UserNode* currentUser = masterHead;

    while(currentUser != nullptr){
        FriendNode* currentFriend = currentUser->friendsHead;

        while(currentFriend != nullptr){
            FriendNode* tempFriend = currentFriend;
            currentFriend = currentFriend->next;
            delete tempFriend;
            tempFriend = nullptr;
        }

        UserNode* tempUser = currentUser;
        currentUser = currentUser->next;
        delete tempUser;
        tempUser = nullptr;
    }

    masterHead = nullptr;
}

void NetworkGraph::addUser(int id, string name, float x, float y){
    UserNode* newUser = new UserNode(id, name, x, y);

    if(masterHead == nullptr){
        masterHead = newUser;
    }else{
        UserNode* currentUser = masterHead;
        
        while(currentUser->next != nullptr){
            currentUser = currentUser->next;
        }

        currentUser->next = newUser;
    }
}

void NetworkGraph::addFriendship(int id1, int id2) {
    UserNode* user1 = findUser(id1);
    UserNode* user2 = findUser(id2);
    
    if (user1 == nullptr || user2 == nullptr) {
        return;
    }
    
    FriendNode* friend1 = new FriendNode(id2);
    friend1->next = user1->friendsHead;
    user1->friendsHead = friend1;
    
    FriendNode* friend2 = new FriendNode(id1);
    friend2->next = user2->friendsHead;
    user2->friendsHead = friend2;
}

void NetworkGraph::removeFriendship(int id1, int id2) {
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

void NetworkGraph::removeUser(int targetID) {
    UserNode* userToDelete = findUser(targetID);
    if (userToDelete == nullptr){ return;
    }
    
    int friendIDs[50];
    int count = 0;

    FriendNode* curr = userToDelete->friendsHead;
    while (curr != nullptr) {
        friendIDs[count] = curr->friendID;
        count++;
        curr = curr->next;
    }

    for (int i = 0; i < count; i++) {
        removeFriendship(targetID, friendIDs[i]);
    }

    // Step 3: unlink and delete the UserNode from the master list
    UserNode* prevUser = nullptr;
    UserNode* currUser = masterHead;
    while (currUser != nullptr) {
        if (currUser->id == targetID) {
            if (prevUser != nullptr){ 
                prevUser->next = currUser->next;
            }else{
                 masterHead = currUser->next;
            }
            delete currUser;
            break;
        }
        prevUser = currUser;
        currUser = currUser->next;
    }
}

UserNode* NetworkGraph::findUser(int targetID) {
    UserNode* currentUser = masterHead;
    
    while (currentUser != nullptr) {
        if (currentUser->id == targetID) {
            return currentUser;
        }
        
        currentUser = currentUser->next;
    }
    
    return nullptr;
}

UserNode* NetworkGraph::getMasterHead() {
    return masterHead;
}