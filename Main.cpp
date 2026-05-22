#include <SFML/Graphics.hpp>
#include <cstdint>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include "Structures/NetworkGraph.h"
#include "Structures/Recommendation_System.h"

// ============================================================================
// MAIN APPLICATION
// Purpose: Entry point for the Social Network Analyzer GUI application
//          Implements SFML rendering, user interaction, and visual updates
// ============================================================================

int main()
{

    // ========================================================================
    // SECTION 1: WINDOW AND GRAPHICS INITIALIZATION
    // ========================================================================

    // Create main application window (1100px wide, 700px tall)
    // Disabled resizing by explicitly setting the window style to Titlebar | Close
    sf::RenderWindow window(sf::VideoMode({1100, 700}), "Social Network Analyzer - DSA Project", sf::Style::Titlebar | sf::Style::Close);

    // Set frame rate limit for smooth rendering
    window.setFramerateLimit(60);

    // Load font for text rendering
    sf::Font font;
    // Attempt local folder first, then standard Windows paths
    if (!font.openFromFile("arial.ttf") &&
        !font.openFromFile("C:\\Windows\\Fonts\\Arial.ttf") && 
        !font.openFromFile("C:\\Windows\\Fonts\\arial.ttf"))
    {
        // If font fails to load, try alternative system path
        if (!font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"))
        {
            // Font loading failed - application cannot display text properly
            std::cerr << "\n[ERROR] Failed to load fonts. Please copy a .ttf file (like arial.ttf) into your project folder!\n";
            return -1; // Exit with error code
        }
    }

    // ========================================================================
    // SECTION 2: NETWORK GRAPH INITIALIZATION AND DATA SEEDING
    // ========================================================================

    // Instantiate the social network graph
    NetworkGraph network;

    // Add sample users to the network with specific coordinates for visual layout
    // Users are positioned in a spatially distributed manner across the canvas

    network.addUser(1, "Alice", 150.0f, 150.0f);   // Top-left region
    network.addUser(2, "Bob", 350.0f, 100.0f);     // Top-center region
    network.addUser(3, "Charlie", 550.0f, 150.0f); // Top-right region
    network.addUser(4, "Diana", 150.0f, 350.0f);   // Middle-left region
    network.addUser(5, "Eve", 350.0f, 300.0f);     // Center region
    network.addUser(6, "Frank", 550.0f, 350.0f);   // Middle-right region
    network.addUser(7, "Grace", 250.0f, 500.0f);   // Bottom-left region
    network.addUser(8, "Hank", 450.0f, 500.0f);    // Bottom-right region

    // Create friendship connections (undirected edges) between users
    // This creates a sample social network with specific connectivity patterns

    network.addFriendship(1, 2); // Alice <-> Bob
    network.addFriendship(1, 4); // Alice <-> Diana
    network.addFriendship(2, 3); // Bob <-> Charlie
    network.addFriendship(2, 5); // Bob <-> Eve
    network.addFriendship(3, 6); // Charlie <-> Frank
    network.addFriendship(4, 5); // Diana <-> Eve
    network.addFriendship(4, 7); // Diana <-> Grace
    network.addFriendship(5, 6); // Eve <-> Frank
    network.addFriendship(5, 8); // Eve <-> Hank
    network.addFriendship(6, 8); // Frank <-> Hank
    network.addFriendship(7, 8); // Grace <-> Hank

    // ========================================================================
    // SECTION 3: UI STATE TRACKING VARIABLES
    // ========================================================================

    int activeUserID = -1;      // Currently selected user ID (-1 = none selected)
    Suggestion suggestions[50]; // Array to store recommendation results
    int suggestionCount = 0;    // Number of valid suggestions computed

    enum AppState {
        NORMAL,
        WAITING_FOR_NODE_POS,
        WAITING_FOR_NODE_NAME,
        WAITING_FOR_EDGE_NODE1,
        WAITING_FOR_EDGE_NODE2,
        WAITING_FOR_RENAME
    };
    AppState currentState = NORMAL;
    std::string inputString = "";
    int edgeNode1 = -1;
    sf::Vector2f newNodePos;
    int nextNodeID = 9; // Since we initialized IDs 1 through 8

    sf::Clock animationClock;

    // ========================================================================
    // SECTION 4: MAIN EVENT AND RENDERING LOOP
    // ========================================================================

    while (window.isOpen())
    {
        float animationTime = animationClock.getElapsedTime().asSeconds();
        float sidebarReveal = animationTime < 1.0f ? animationTime : 1.0f;
        float sidebarOffset = (1.0f - sidebarReveal) * 80.0f;
        float sidebarAlpha = sidebarReveal;

        // --------------------------------------------------------------------
        // SUBSECTION 4.1: EVENT HANDLING
        // --------------------------------------------------------------------

        while (const auto event = window.pollEvent())
        {

            // Handle window close event
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            // Handle keyboard text entry for naming
            if (const auto* textEvent = event->getIf<sf::Event::TextEntered>())
            {
                if (currentState == WAITING_FOR_NODE_NAME || currentState == WAITING_FOR_RENAME)
                {
                    if (textEvent->unicode == '\b') { // Backspace
                        if (!inputString.empty()) inputString.pop_back();
                    } else if (textEvent->unicode == '\r' || textEvent->unicode == '\n') { // Enter
                        if (!inputString.empty()) {
                            if (currentState == WAITING_FOR_NODE_NAME) {
                                network.addUser(nextNodeID++, inputString, newNodePos.x, newNodePos.y);
                            } else if (currentState == WAITING_FOR_RENAME && activeUserID != -1) {
                                UserNode* u = network.findUser(activeUserID);
                                if (u) u->name = inputString;
                            }
                            currentState = NORMAL;
                            inputString = "";
                        }
                    } else if (textEvent->unicode < 128 && textEvent->unicode > 31) {
                        inputString += static_cast<char>(textEvent->unicode);
                    }
                }
            }

            // Handle mouse click events
            if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>())
            {

                // Check if left mouse button was clicked
                if (mouseButton->button == sf::Mouse::Button::Left)
                {

                    // Get the mouse click position
                    sf::Vector2f mousePos(
                        static_cast<float>(mouseButton->position.x),
                        static_cast<float>(mouseButton->position.y));

                    // Check if UI buttons were clicked
                    bool buttonClicked = false;
                    sf::FloatRect btnAddUser(sf::Vector2f(720.0f + sidebarOffset, 520.0f), sf::Vector2f(175.0f, 35.0f));
                    sf::FloatRect btnAddEdge(sf::Vector2f(905.0f + sidebarOffset, 520.0f), sf::Vector2f(175.0f, 35.0f));
                    sf::FloatRect btnRename(sf::Vector2f(720.0f + sidebarOffset, 565.0f), sf::Vector2f(175.0f, 35.0f));
                    sf::FloatRect btnDelete(sf::Vector2f(905.0f + sidebarOffset, 565.0f), sf::Vector2f(175.0f, 35.0f));

                    if (btnAddUser.contains(mousePos)) { currentState = WAITING_FOR_NODE_POS; buttonClicked = true; }
                    else if (btnAddEdge.contains(mousePos)) { currentState = WAITING_FOR_EDGE_NODE1; buttonClicked = true; }
                    else if (btnRename.contains(mousePos)) { 
                        if (activeUserID != -1) { currentState = WAITING_FOR_RENAME; inputString = ""; }
                        buttonClicked = true; 
                    }
                    else if (btnDelete.contains(mousePos)) { 
                        if (activeUserID != -1) { 
                            network.removeUser(activeUserID); 
                            activeUserID = -1; 
                            suggestionCount = 0;
                            currentState = NORMAL;
                        }
                        buttonClicked = true; 
                    }

                    if (!buttonClicked) {
                        if (currentState == WAITING_FOR_NODE_POS) {
                            if (mousePos.x < 700.0f) { // Click on the canvas area
                                newNodePos = mousePos;
                                currentState = WAITING_FOR_NODE_NAME;
                                inputString = "";
                            }
                        } else if (currentState != WAITING_FOR_NODE_NAME && currentState != WAITING_FOR_RENAME) {
                            // Traverse all users to check if any node was clicked
                            UserNode *clickUser = network.getMasterHead();
                            bool userClicked = false; // Flag to track if click hit a node
                            
                            while (clickUser != nullptr)
                            {
                                // Create a circle shape representing this user's visual node
                                sf::CircleShape nodeCircle(20.0f); // 20px radius
                                nodeCircle.setPosition(sf::Vector2f(clickUser->x - 20.0f, clickUser->y - 20.0f));
                                
                                // Check if mouse click position is within this circle's bounds
                                if (nodeCircle.getGlobalBounds().contains(mousePos))
                                {
                                    userClicked = true;
                                    
                                    if (currentState == WAITING_FOR_EDGE_NODE1) {
                                        edgeNode1 = clickUser->id;
                                        currentState = WAITING_FOR_EDGE_NODE2;
                                    } else if (currentState == WAITING_FOR_EDGE_NODE2) {
                                        if (clickUser->id != edgeNode1) {
                                            network.addFriendship(edgeNode1, clickUser->id);
                                            currentState = NORMAL;
                                            if (activeUserID != -1) {
                                                suggestionCount = RecommendationEngine::computeMutualFriends(network.getMasterHead(), activeUserID, suggestions);
                                            }
                                        }
                                    } else {
                                        // User node was clicked - update active user
                                        activeUserID = clickUser->id;
                                        suggestionCount = RecommendationEngine::computeMutualFriends(
                                            network.getMasterHead(),
                                            activeUserID,
                                            suggestions);
                                        currentState = NORMAL;
                                    }
                                    break; // Exit search loop
                                }
                                clickUser = clickUser->next;
                            }
                            
                            // If click was outside all nodes, deselect active user
                            if (!userClicked && mousePos.x < 700.0f)
                            {
                                activeUserID = -1;   // Clear selection
                                suggestionCount = 0; // Clear suggestions
                                currentState = NORMAL;
                            }
                        }
                    }
                }
            }
        }

        // --------------------------------------------------------------------
        // SUBSECTION 4.2: CLEAR WINDOW FOR NEW FRAME
        // --------------------------------------------------------------------

        window.clear(sf::Color(10, 12, 16)); // Attractive dark slate background

        // Soft animated background accents for a more polished look.
        for (int i = 0; i < 4; i++)
        {
            float radius = 70.0f + static_cast<float>(i) * 18.0f;
            sf::CircleShape glow(radius);
            glow.setPointCount(60); // Smooth glowing circles
            glow.setOrigin(sf::Vector2f(radius, radius));
            glow.setPosition(sf::Vector2f(
                120.0f + static_cast<float>(i) * 140.0f + std::sin(animationTime * 0.6f + static_cast<float>(i)) * 10.0f,
                120.0f + static_cast<float>(i % 2) * 110.0f + std::cos(animationTime * 0.5f + static_cast<float>(i)) * 8.0f));
            glow.setFillColor(sf::Color(255, 255, 255, 10));
            window.draw(glow);
        }

        // --------------------------------------------------------------------
        // SUBSECTION 4.3: DRAW RIGHT SIDEBAR PANEL
        // --------------------------------------------------------------------

        // Create sidebar background rectangle
        sf::RectangleShape sidebar(sf::Vector2f(400.0f, 700.0f));
        sidebar.setPosition(sf::Vector2f(700.0f + sidebarOffset, 0.0f));
        sidebar.setFillColor(sf::Color(42, 44, 54, static_cast<std::uint8_t>(235.0f * sidebarAlpha))); // Dark gray background
        window.draw(sidebar);

        // Draw sidebar title text
        sf::Text titleText(font, "Social Network Analyzer", 22);
        titleText.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(255.0f * sidebarAlpha)));
        titleText.setPosition(sf::Vector2f(720.0f + sidebarOffset, 20.0f));
        titleText.setStyle(sf::Text::Bold);
        window.draw(titleText);

        // Draw subtitle text
        sf::Text subtitleText(font, "Click a user to view details", 14);
        subtitleText.setFillColor(sf::Color(200, 200, 200, static_cast<std::uint8_t>(255.0f * sidebarAlpha)));
        subtitleText.setPosition(sf::Vector2f(720.0f + sidebarOffset, 50.0f));
        window.draw(subtitleText);

        // --------------------------------------------------------------------
        // SUBSECTION 4.4: DRAW FRIENDSHIP EDGES (CONNECTIONS)
        // --------------------------------------------------------------------

        // Create vertex array to hold all edge lines
        sf::VertexArray edges(sf::PrimitiveType::Lines);

        // Traverse all users to draw their friendship connections
        UserNode *currentUser = network.getMasterHead();

        while (currentUser != nullptr)
        {

            // For each user, traverse their friend list
            FriendNode *currentFriend = currentUser->friendsHead;

            while (currentFriend != nullptr)
            {

                // Find the friend's user node to get coordinates
                UserNode *friendUser = network.findUser(currentFriend->friendID);

                if (friendUser != nullptr)
                {

                    // Avoid drawing duplicate edges (draw only if current ID < friend ID)
                    if (currentUser->id < friendUser->id)
                    {

                        // Create line start point (current user's position)
                        sf::Vertex startVertex;
                        startVertex.position = sf::Vector2f(currentUser->x, currentUser->y);
                        startVertex.color = sf::Color(180, 180, 180, static_cast<std::uint8_t>(145.0f + 10.0f * std::sin(animationTime * 2.0f)));

                        // Create line end point (friend's position)
                        sf::Vertex endVertex;
                        endVertex.position = sf::Vector2f(friendUser->x, friendUser->y);
                        endVertex.color = sf::Color(180, 180, 180, static_cast<std::uint8_t>(145.0f + 10.0f * std::sin(animationTime * 2.0f)));

                        // Add both vertices to edge array (forms one line)
                        edges.append(startVertex);
                        edges.append(endVertex);
                    }
                }

                // Move to next friend
                currentFriend = currentFriend->next;
            }

            // Move to next user
            currentUser = currentUser->next;
        }

        // Draw all edges at once
        window.draw(edges);

        // --------------------------------------------------------------------
        // SUBSECTION 4.5: DRAW USER NODES (VERTICES)
        // --------------------------------------------------------------------

        // Traverse all users again to draw their visual nodes
        currentUser = network.getMasterHead();

        while (currentUser != nullptr)
        {

            // Determine node color based on relationship to active user
            sf::Color nodeColor = sf::Color(255, 255, 255);
            float nodeBob = std::sin(animationTime * 1.6f + static_cast<float>(currentUser->id)) * 3.0f;
            float nodeScale = 1.0f + std::sin(animationTime * 2.0f + static_cast<float>(currentUser->id)) * 0.03f;

            if (activeUserID != -1 && currentUser->id == activeUserID)
            {
                // This is the active user - color blue
                nodeColor = sf::Color(70, 130, 255); // Bright blue
                nodeScale = 1.08f + std::sin(animationTime * 4.0f) * 0.04f;
            }
            else if (activeUserID != -1)
            {
                // Check if this user is a direct friend of active user
                UserNode *activeUser = network.findUser(activeUserID);
                bool isFriend = false;

                if (activeUser != nullptr)
                {
                    FriendNode *friendCheck = activeUser->friendsHead;

                    while (friendCheck != nullptr)
                    {
                        if (friendCheck->friendID == currentUser->id)
                        {
                            isFriend = true;
                            break;
                        }
                        friendCheck = friendCheck->next;
                    }
                }

                if (isFriend)
                {
                    // This user is a direct friend - color green
                    nodeColor = sf::Color(80, 200, 120); // Green
                }
                else
                {
                    // Check if this user is in suggestions list
                    bool isSuggested = false;

                    for (int i = 0; i < suggestionCount; i++)
                    {
                        if (suggestions[i].userID == currentUser->id)
                        {
                            isSuggested = true;
                            break;
                        }
                    }

                    if (isSuggested)
                    {
                        // This user is a suggestion - color gold
                        nodeColor = sf::Color(255, 215, 0); // Gold
                        nodeScale = 1.03f + std::sin(animationTime * 3.0f + static_cast<float>(currentUser->id)) * 0.02f;
                    }
                    else
                    {
                        // Unrelated users stay white on the black canvas
                        nodeColor = sf::Color(255, 255, 255);
                    }
                }
            }

            // Create and draw the circular node
            sf::CircleShape nodeCircle(20.0f); // 20px radius
            nodeCircle.setPointCount(50); // Make nodes perfectly round
            nodeCircle.setOrigin(sf::Vector2f(20.0f, 20.0f));
            nodeCircle.setScale(sf::Vector2f(nodeScale, nodeScale));
            nodeCircle.setPosition(sf::Vector2f(currentUser->x, currentUser->y + nodeBob));
            nodeCircle.setFillColor(nodeColor);
            nodeCircle.setOutlineThickness(2.0f);
            nodeCircle.setOutlineColor(sf::Color(255, 255, 255));
            window.draw(nodeCircle);

            // Draw user name label below the node
            sf::Text nameLabel(font, currentUser->name, 12);
            nameLabel.setFillColor(sf::Color(235, 235, 235));

            // Center the text below the node
            sf::FloatRect textBounds = nameLabel.getLocalBounds();
            nameLabel.setPosition(sf::Vector2f(
                currentUser->x - (textBounds.position.x + textBounds.size.x / 2.0f),
                currentUser->y + 25.0f - textBounds.position.y));
            window.draw(nameLabel);

            // Move to next user
            currentUser = currentUser->next;
        }

        // --------------------------------------------------------------------
        // SUBSECTION 4.6: DRAW SIDEBAR PROFILE AND RECOMMENDATIONS
        // --------------------------------------------------------------------

        if (activeUserID != -1)
        {

            // Find active user node
            UserNode *activeUser = network.findUser(activeUserID);

            if (activeUser != nullptr)
            {

                // Draw profile section header
                sf::Text profileHeader(font, "Active User Profile", 18);
                profileHeader.setFillColor(sf::Color(70, 130, 255));
                profileHeader.setPosition(sf::Vector2f(720.0f + sidebarOffset, 90.0f));
                profileHeader.setStyle(sf::Text::Bold);
                window.draw(profileHeader);

                // Draw user name
                sf::Text userName(font, "Name: " + activeUser->name, 16);
                userName.setFillColor(sf::Color(255, 255, 255));
                userName.setPosition(sf::Vector2f(720.0f + sidebarOffset, 120.0f));
                window.draw(userName);

                // Count direct friends
                int friendCount = 0;
                FriendNode *friendCounter = activeUser->friendsHead;
                while (friendCounter != nullptr)
                {
                    friendCount++;
                    friendCounter = friendCounter->next;
                }

                // Draw friend count
                std::ostringstream oss;
                oss << "Direct Friends: " << friendCount;
                sf::Text friendCountText(font, oss.str(), 16);
                friendCountText.setFillColor(sf::Color(255, 255, 255));
                friendCountText.setPosition(sf::Vector2f(720.0f + sidebarOffset, 145.0f));
                window.draw(friendCountText);

                // Draw friends list
                sf::Text friendsListHeader(font, "Friend List:", 14);
                friendsListHeader.setFillColor(sf::Color(80, 200, 120));
                friendsListHeader.setPosition(sf::Vector2f(720.0f + sidebarOffset, 175.0f));
                friendsListHeader.setStyle(sf::Text::Bold);
                window.draw(friendsListHeader);

                // List each friend's name
                FriendNode *friendList = activeUser->friendsHead;
                int yOffset = 200;

                while (friendList != nullptr)
                {
                    UserNode *friendUser = network.findUser(friendList->friendID);

                    if (friendUser != nullptr)
                    {
                        sf::Text friendName(font, "- " + friendUser->name, 13);
                        friendName.setFillColor(sf::Color(200, 200, 200, static_cast<std::uint8_t>(255.0f * sidebarAlpha)));
                        friendName.setPosition(sf::Vector2f(730.0f + sidebarOffset, static_cast<float>(yOffset)));
                        window.draw(friendName);

                        yOffset += 20;
                    }

                    friendList = friendList->next;
                }

                // Draw recommendations section
                sf::Text recommendHeader(font, "People You May Know:", 16);
                recommendHeader.setFillColor(sf::Color(255, 215, 0));
                recommendHeader.setPosition(sf::Vector2f(720.0f + sidebarOffset, static_cast<float>(yOffset + 20)));
                recommendHeader.setStyle(sf::Text::Bold);
                window.draw(recommendHeader);

                yOffset += 50;

                // Display each suggestion with mutual friend count
                if (suggestionCount == 0)
                {
                    sf::Text noSuggestions(font, "No suggestions available", 13);
                    noSuggestions.setFillColor(sf::Color(150, 150, 150));
                    noSuggestions.setPosition(sf::Vector2f(730.0f + sidebarOffset, static_cast<float>(yOffset)));
                    window.draw(noSuggestions);
                }
                else
                {
                    for (int i = 0; i < suggestionCount; i++)
                    {
                        UserNode *suggestedUser = network.findUser(suggestions[i].userID);

                        if (suggestedUser != nullptr)
                        {
                            std::ostringstream suggestionText;
                            suggestionText << "- " << suggestedUser->name
                                           << " (" << suggestions[i].mutualCount
                                           << " mutual)";

                            sf::Text suggestion(font, suggestionText.str(), 13);
                            suggestion.setFillColor(sf::Color(220, 220, 220, static_cast<std::uint8_t>(255.0f * sidebarAlpha)));
                            suggestion.setPosition(sf::Vector2f(730.0f + sidebarOffset, static_cast<float>(yOffset)));
                            window.draw(suggestion);

                            yOffset += 22;
                        }
                    }
                }
            }
        }
        else
        {
            // No user selected - show instructions
            sf::Text instructionText(font, "Select a user node\nto view their profile\nand friend suggestions", 16);
            instructionText.setFillColor(sf::Color(220, 220, 220));
            instructionText.setPosition(sf::Vector2f(750.0f + sidebarOffset, 250.0f));
            window.draw(instructionText);
        }

        // Draw color legend
        sf::Text legendTitle(font, "Color Legend:", 14);
        legendTitle.setFillColor(sf::Color(255, 255, 255));
        legendTitle.setPosition(sf::Vector2f(720.0f + sidebarOffset, 620.0f));
        legendTitle.setStyle(sf::Text::Bold);
        window.draw(legendTitle);

        // Blue - Active user
        sf::CircleShape blueCircle(8.0f);
        blueCircle.setPointCount(30);
        blueCircle.setPosition(sf::Vector2f(720.0f + sidebarOffset, 645.0f));
        blueCircle.setFillColor(sf::Color(70, 130, 255));
        window.draw(blueCircle);

        sf::Text blueLabel(font, "Active User", 12);
        blueLabel.setFillColor(sf::Color(200, 200, 200));
        blueLabel.setPosition(sf::Vector2f(740.0f + sidebarOffset, 645.0f));
        window.draw(blueLabel);

        // Green - Direct friend
        sf::CircleShape greenCircle(8.0f);
        greenCircle.setPointCount(30);
        greenCircle.setPosition(sf::Vector2f(850.0f + sidebarOffset, 645.0f));
        greenCircle.setFillColor(sf::Color(80, 200, 120));
        window.draw(greenCircle);

        sf::Text greenLabel(font, "Direct Friend", 12);
        greenLabel.setFillColor(sf::Color(200, 200, 200));
        greenLabel.setPosition(sf::Vector2f(870.0f + sidebarOffset, 645.0f));
        window.draw(greenLabel);

        // Gold - Suggestion
        sf::CircleShape goldCircle(8.0f);
        goldCircle.setPointCount(30);
        goldCircle.setPosition(sf::Vector2f(990.0f + sidebarOffset, 645.0f));
        goldCircle.setFillColor(sf::Color(255, 215, 0));
        window.draw(goldCircle);

        sf::Text goldLabel(font, "Suggested", 12);
        goldLabel.setFillColor(sf::Color(200, 200, 200));
        goldLabel.setPosition(sf::Vector2f(1010.0f + sidebarOffset, 645.0f));
        window.draw(goldLabel);

        // --------------------------------------------------------------------
        // SUBSECTION 4.7: DRAW CONTROL BUTTONS & STATE ALERTS
        // --------------------------------------------------------------------
        
        auto drawButton = [&](const std::string& text, float x, float y, bool active) {
            sf::RectangleShape btn(sf::Vector2f(175.0f, 35.0f));
            btn.setPosition(sf::Vector2f(x, y));
            btn.setFillColor(active ? sf::Color(100, 150, 200, 200) : sf::Color(60, 65, 75, 200));
            btn.setOutlineThickness(1.0f);
            btn.setOutlineColor(sf::Color(100, 100, 100));
            window.draw(btn);
            
            sf::Text btnText(font, text, 14);
            btnText.setFillColor(sf::Color::White);
            sf::FloatRect textBounds = btnText.getLocalBounds();
            btnText.setPosition(sf::Vector2f(x + (175.0f - textBounds.size.x) / 2.0f, y + 8.0f));
            window.draw(btnText);
        };

        drawButton("Add User", 720.0f + sidebarOffset, 520.0f, currentState == WAITING_FOR_NODE_POS || currentState == WAITING_FOR_NODE_NAME);
        drawButton("Add Friend", 905.0f + sidebarOffset, 520.0f, currentState == WAITING_FOR_EDGE_NODE1 || currentState == WAITING_FOR_EDGE_NODE2);
        drawButton("Rename User", 720.0f + sidebarOffset, 565.0f, currentState == WAITING_FOR_RENAME);
        drawButton("Delete User", 905.0f + sidebarOffset, 565.0f, false);

        if (currentState != NORMAL) {
            sf::Text alertText(font, "", 16);
            alertText.setFillColor(sf::Color(255, 200, 50));
            
            if (currentState == WAITING_FOR_NODE_POS) alertText.setString("Action: Click empty space on the canvas to place a new user node.");
            else if (currentState == WAITING_FOR_NODE_NAME) alertText.setString("Action: Type new user's name and press Enter -> " + inputString);
            else if (currentState == WAITING_FOR_EDGE_NODE1) alertText.setString("Action: Click the first user node for the new friendship.");
            else if (currentState == WAITING_FOR_EDGE_NODE2) alertText.setString("Action: Click the second user node to complete the friendship.");
            else if (currentState == WAITING_FOR_RENAME) alertText.setString("Action: Type new name and press Enter -> " + inputString);
            
            alertText.setPosition(sf::Vector2f(20.0f, 660.0f)); // Bottom left of canvas
            window.draw(alertText);
        }

        // --------------------------------------------------------------------
        // SUBSECTION 4.8: DISPLAY RENDERED FRAME
        // --------------------------------------------------------------------

        window.display();
    }

    return 0;
}