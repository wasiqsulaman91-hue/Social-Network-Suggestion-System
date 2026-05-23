#ifndef STRUCTURES_H
#define STRUCTURES_H
#include <string>

struct FriendNode {
    int friendID;        
    FriendNode* next;   
    

    FriendNode(int id) {
        friendID = id;     
        next = nullptr;     
    }
};

struct UserNode {
    int id;               
    std::string name;     
    float x;              
    float y;              
    FriendNode* friendsHead;
    UserNode* next;        
   
    UserNode(int userId, std::string userName, float posX, float posY) {
        id = userId;      
        name = userName;    
        x = posX;      
        y = posY;         
        friendsHead = nullptr;  
        next = nullptr;     
    }
};


struct Suggestion {
    int userID;          
    int mutualCount;   
    

    Suggestion() {
        userID = -1;     
        mutualCount = 0;    
    }

    Suggestion(int id, int count) {
        userID = id;     
        mutualCount = count;
    }
};

#endif