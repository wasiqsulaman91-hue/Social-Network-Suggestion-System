#include "../Structures/Recommendation_System.h"

int RecommendationEngine::computeMutualFriends(UserNode *masterHead, int activeUserID, Suggestion outputArray[])
{
    int mutualCounts[50];

    for (int i = 0; i < 50; i++)
    {
        mutualCounts[i] = 0;
    }

    UserNode *activeUser = nullptr;
    UserNode *currentUser = masterHead;

    while (currentUser != nullptr)
    {
        if (currentUser->id == activeUserID)
        {
            activeUser = currentUser;
            break;
        }
        currentUser = currentUser->next;
    }

    if (activeUser == nullptr)
    {
        return 0;
    }

    FriendNode *level1Friend = activeUser->friendsHead;

    while (level1Friend != nullptr)
    {
        int friendID = level1Friend->friendID;
        UserNode *friendUser = nullptr;

        currentUser = masterHead;
        while (currentUser != nullptr)
        {
            if (currentUser->id == friendID)
            {
                friendUser = currentUser;
                break;
            }
            currentUser = currentUser->next;
        }

        if (friendUser != nullptr)
        {
            FriendNode *level2Friend = friendUser->friendsHead;

            while (level2Friend != nullptr)
            {
                int fofID = level2Friend->friendID;

                if (fofID != activeUserID)
                {
                    if (!isDirectFriend(activeUser, fofID))
                    {
                        mutualCounts[fofID]++;
                    }
                }

                level2Friend = level2Friend->next;
            }
        }

        level1Friend = level1Friend->next;
    }

    int suggestionCount = 0;

    for (int i = 0; i < 50; i++)
    {
        if (mutualCounts[i] > 0)
        {
            outputArray[suggestionCount].userID = i;
            outputArray[suggestionCount].mutualCount = mutualCounts[i];
            suggestionCount++;
        }
    }

    sortSuggestions(outputArray, suggestionCount);

    return suggestionCount;
}

bool RecommendationEngine::isDirectFriend(UserNode *user, int candidateID)
{
    FriendNode *currentFriend = user->friendsHead;

    while (currentFriend != nullptr)
    {
        if (currentFriend->friendID == candidateID)
        {
            return true;
        }

        currentFriend = currentFriend->next;
    }

    return false;
}

void RecommendationEngine::sortSuggestions(Suggestion arr[], int count)
{
    for (int i = 0; i < count - 1; i++)
    {
        int maxIndex = i;

        for (int j = i + 1; j < count; j++)
        {
            if (arr[j].mutualCount > arr[maxIndex].mutualCount)
            {
                maxIndex = j;
            }
        }

        if (maxIndex != i)
        {
            Suggestion temp = arr[i];
            arr[i] = arr[maxIndex];
            arr[maxIndex] = temp;
        }
    }
}