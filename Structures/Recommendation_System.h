#ifndef RECOMMENDATIONENGINE_H
#define RECOMMENDATIONENGINE_H

#include <iostream>
#include "Structures.h"


class RecommendationEngine
{
public:
    static int computeMutualFriends(UserNode *masterHead, int activeUserID, Suggestion outputArray[]);

private:
    static bool isDirectFriend(UserNode *user, int candidateID);
    static void sortSuggestions(Suggestion arr[], int count);
};

#endif