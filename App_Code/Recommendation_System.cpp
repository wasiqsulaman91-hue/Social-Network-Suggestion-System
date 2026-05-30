#include "../Structures/Recommendation_System.h"

UserNode* findUserByID(UserNode* head, int id) {
    while (head != nullptr) {
        if (head->id == id){
            return head;
        }
        head = head->next;
    }
    return nullptr;
}


int RecommendationEngine::computeMutualFriends(UserNode* masterHead, int activeUserID, Suggestion array[]){
    int mutualCounts[50] = {0};

    UserNode* activeUser = findUserByID(masterHead, activeUserID);

    if(activeUser == nullptr){
        return 0;
    }

    FriendNode* level1 = activeUser->friendsHead;

    while(level1 != nullptr){
        UserNode* friendUser = findUserByID(masterHead, level1->friendID);

        if(friendUser != nullptr){
            FriendNode* level2 = friendUser->friendsHead;

            while(level2 != nullptr){
                int fofID = level2->friendID;

                if(fofID != activeUserID && !isDirectFriend(activeUser, fofID)){
                    mutualCounts[fofID] = mutualCounts[fofID] + 1; 
                }
                level2 = level2->next;
            }
        }
        level1 = level1->next;
    }

    int count = 0;
    for(int i = 0; i < 50; i++){
        if(mutualCounts[i] > 0){
            array[count] = Suggestion(i, mutualCounts[i]);
            count++;
        }
    }

    sortSuggestions(array, count);
    return count;
}

bool RecommendationEngine::isDirectFriend(UserNode* user, int candidateID){
    FriendNode* currentFriend = user->friendsHead;

    while(currentFriend != nullptr){
        if(currentFriend->friendID == candidateID){
            return true;
        }
        currentFriend = currentFriend->next;
    }
    return false;
}

void RecommendationEngine::sortSuggestions(Suggestion arr[], int count)
{
    for (int i = 0; i < count - 1; i++){
        int maxIndex = i;
        for (int j = i + 1; j < count; j++){
            if (arr[j].mutualCount > arr[maxIndex].mutualCount){
                maxIndex = j;
            }
        }
        if (maxIndex != i){
            Suggestion temp = arr[i];
            arr[i] = arr[maxIndex];
            arr[maxIndex] = temp;
        }
    }
}