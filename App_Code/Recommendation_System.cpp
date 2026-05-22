#include "../Structures/Recommendation_System.h"

// ============================================================================
// FUNCTION: computeMutualFriends
// Purpose: Calculates "People You May Know" recommendations for active user
//          by finding friends-of-friends and counting mutual connections
// Parameters: masterHead - pointer to the master user list
//             activeUserID - ID of the user for whom to generate suggestions
//             outputArray - array to store resulting suggestions
// Returns: Total count of suggestions generated
// Algorithm: 2-level BFS bounded traversal with mutual friend counting
// ============================================================================
int RecommendationEngine::computeMutualFriends(UserNode *masterHead, int activeUserID, Suggestion outputArray[])
{

    // Step 1: Initialize a fixed-size array to track mutual friend counts
    // Index represents user ID, value represents count of mutual friends
    // Sized for up to 50 users (can be adjusted based on network size)
    int mutualCounts[50];

    // Zero out all counts initially
    for (int i = 0; i < 50; i++)
    {
        mutualCounts[i] = 0; // No mutual friends counted yet for any user
    }

    // Step 2: Find the active user node in the master list
    UserNode *activeUser = nullptr;
    UserNode *currentUser = masterHead;

    // Traverse master list to locate active user
    while (currentUser != nullptr)
    {
        if (currentUser->id == activeUserID)
        {
            activeUser = currentUser; // Found the active user
            break;                    // Exit search loop
        }
        currentUser = currentUser->next; // Move to next user
    }

    // Step 3: Safety check - verify active user exists
    if (activeUser == nullptr)
    {
        return 0; // Active user not found - return no suggestions
    }

    // Step 4: Traverse Level 1 - Direct friends of active user
    FriendNode *level1Friend = activeUser->friendsHead;

    while (level1Friend != nullptr)
    {
        // Step 5: For each direct friend, find their user node
        int friendID = level1Friend->friendID;
        UserNode *friendUser = nullptr;

        // Search for this friend's user node in master list
        currentUser = masterHead;
        while (currentUser != nullptr)
        {
            if (currentUser->id == friendID)
            {
                friendUser = currentUser; // Found friend's user node
                break;
            }
            currentUser = currentUser->next;
        }

        // Step 6: If friend user found, traverse their friends (Level 2)
        if (friendUser != nullptr)
        {
            FriendNode *level2Friend = friendUser->friendsHead;

            while (level2Friend != nullptr)
            {
                int fofID = level2Friend->friendID; // Friend-of-friend ID

                // Step 7: Apply filtering rules for valid suggestions:

                // Rule 1: Friend-of-friend must not be the active user themselves
                if (fofID != activeUserID)
                {

                    // Rule 2: Friend-of-friend must not already be a direct friend
                    if (!isDirectFriend(activeUser, fofID))
                    {

                        // Valid suggestion - increment mutual friend count
                        mutualCounts[fofID]++;
                    }
                }

                // Move to next friend-of-friend
                level2Friend = level2Friend->next;
            }
        }

        // Move to next direct friend of active user
        level1Friend = level1Friend->next;
    }

    // Step 8: Collect all non-zero suggestions into output array
    int suggestionCount = 0; // Track number of valid suggestions

    for (int i = 0; i < 50; i++)
    {
        // Check if this user ID has any mutual friend connections
        if (mutualCounts[i] > 0)
        {
            // Create a suggestion entry
            outputArray[suggestionCount].userID = i;
            outputArray[suggestionCount].mutualCount = mutualCounts[i];
            suggestionCount++; // Increment suggestion counter
        }
    }

    // Step 9: Sort suggestions in descending order by mutual friend count
    sortSuggestions(outputArray, suggestionCount);

    // Step 10: Return total count of suggestions generated
    return suggestionCount;
}

// ============================================================================
// FUNCTION: isDirectFriend (HELPER)
// Purpose: Checks if a candidate user is already a direct friend of given user
// Parameters: user - pointer to the user node to check
//             candidateID - ID of potential friend to search for
// Returns: true if candidate is already a direct friend, false otherwise
// ============================================================================
bool RecommendationEngine::isDirectFriend(UserNode *user, int candidateID)
{
    // Step 1: Start at the head of user's adjacency list (friend list)
    FriendNode *currentFriend = user->friendsHead;

    // Step 2: Traverse the entire friend list
    while (currentFriend != nullptr)
    {
        // Step 3: Check if current friend's ID matches candidate ID
        if (currentFriend->friendID == candidateID)
        {
            return true; // Candidate is already a direct friend
        }

        // Step 4: Move to next friend in the list
        currentFriend = currentFriend->next;
    }

    // Step 5: Traversed entire list without finding candidate
    return false; // Candidate is not a direct friend
}

// ============================================================================
// FUNCTION: sortSuggestions (HELPER)
// Purpose: Sorts suggestion array in descending order by mutual friend count
// Parameters: arr - array of Suggestion structs to sort
//             count - number of valid suggestions in the array
// Algorithm: Selection Sort (chosen for clarity and simplicity)
// ============================================================================
void RecommendationEngine::sortSuggestions(Suggestion arr[], int count)
{
    // Outer loop: iterate through each position in array
    for (int i = 0; i < count - 1; i++)
    {

        // Step 1: Assume current position has the maximum mutual count
        int maxIndex = i;

        // Inner loop: find the actual maximum in remaining unsorted portion
        for (int j = i + 1; j < count; j++)
        {

            // Step 2: Compare mutual counts
            if (arr[j].mutualCount > arr[maxIndex].mutualCount)
            {
                // Found a larger mutual count - update max index
                maxIndex = j;
            }
        }

        // Step 3: Swap the maximum element with current position
        if (maxIndex != i)
        {
            // Create temporary variable to hold current element
            Suggestion temp = arr[i];

            // Move maximum element to current position
            arr[i] = arr[maxIndex];

            // Place original current element in max's old position
            arr[maxIndex] = temp;
        }
    }
    // After all iterations, array is sorted in descending order by mutualCount
}