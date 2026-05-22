#ifndef RECOMMENDATIONENGINE_H
#define RECOMMENDATIONENGINE_H

#include "iostream"
#include "Structures.h"

// ============================================================================
// CLASS: RecommendationEngine
// Purpose: Encapsulates the friend recommendation algorithm logic.
//          Uses BFS-bounded traversal to find friends-of-friends and
//          calculates mutual friend counts for "People You May Know" suggestions.
// ============================================================================
class RecommendationEngine {
public:
    // Computes friend suggestions for a given active user
    // Returns the count of suggestions and populates outputArray with results
    static int computeMutualFriends(UserNode* masterHead, int activeUserID, Suggestion outputArray[]);
    
private:
    // Helper function to check if a user is already a direct friend
    static bool isDirectFriend(UserNode* user, int candidateID);
    
    // Helper function to sort suggestions in descending order by mutual count
    static void sortSuggestions(Suggestion arr[], int count);
};

#endif // RECOMMENDATIONENGINE_H