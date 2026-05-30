#include <SFML/Graphics.hpp>
#include <cstdint>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include "Structures/NetworkGraph.h"
#include "Structures/Recommendation_System.h"

using namespace std;
using namespace sf;

int main()
{
    RenderWindow window(VideoMode({1100, 700}), "SUPER SPY", Style::Titlebar | Style::Close);

    window.setFramerateLimit(60);

    Font font;

    if (!font.openFromFile("arial.ttf") && !font.openFromFile("C:\\Windows\\Fonts\\Arial.ttf")){
        cerr << "\n[ERROR] Failed to load font!\n";
        return -1;
    }

    NetworkGraph n;


    n.addUser(1, "Wasiq", 150.0f, 150.0f);
    n.addUser(2, "Shariq", 350.0f, 100.0f);
    n.addUser(3, "Shameen", 550.0f, 150.0f);
    n.addUser(4, "Zainab", 150.0f, 350.0f);
    n.addUser(5, "Farwa", 350.0f, 300.0f);
    n.addUser(6, "Zohaib", 550.0f, 350.0f);
    n.addUser(7, "Muneeb", 250.0f, 500.0f);
    n.addUser(8, "Mazhar", 450.0f, 500.0f);

    n.addFriendship(1, 2);
    n.addFriendship(1, 4);
    n.addFriendship(2, 3);
    n.addFriendship(2, 5);
    n.addFriendship(3, 6);
    n.addFriendship(4, 5);
    n.addFriendship(4, 7);
    n.addFriendship(5, 6);
    n.addFriendship(5, 8);
    n.addFriendship(6, 8);
    n.addFriendship(7, 8);

    int activeUserID = -1;
    Suggestion suggestions[50];
    int suggestionCount = 0;

    enum AppState{
        NORMAL, 
        WAITING_FOR_NODE_POSITION,
        WAITING_FOR_NODE_NAME,
        WAITING_FOR_EDGE_NODE1,
        WAITING_FOR_EDGE_NODE2,
        WAITING_FOR_RENAME
    };

    AppState currentState = NORMAL;
    string inputString = "";
    int edgeNode1 = -1;
    Vector2f newNodePos;
    int nextNodeID = 9;

    Clock animationClock;

    while (window.isOpen()){
        float animationTime = animationClock.getElapsedTime().asSeconds();
        float sidebarReveal = animationTime < 1.0f ? animationTime : 1.0f;
        float sidebarOffset = (1.0f - sidebarReveal) * 80.0f;
        float sidebarAlpha = sidebarReveal;

        while (const auto event = window.pollEvent()){
            if (event->is<Event::Closed>()){
                window.close();
            }

            if (const auto* textEvent = event->getIf<Event::TextEntered>()){
                if (currentState == WAITING_FOR_NODE_NAME || currentState == WAITING_FOR_RENAME){
                    if (textEvent->unicode == '\b') {
                        if (!inputString.empty()) inputString.pop_back();
                    } else if (textEvent->unicode == '\r' || textEvent->unicode == '\n') {
                        if (!inputString.empty()) {
                            if (currentState == WAITING_FOR_NODE_NAME) {
                                n.addUser(nextNodeID++, inputString, newNodePos.x, newNodePos.y);
                            } else if (currentState == WAITING_FOR_RENAME && activeUserID != -1) {
                                UserNode* u = n.findUser(activeUserID);
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

            if (const auto* mouseButton = event->getIf<Event::MouseButtonPressed>())
            {
                if (mouseButton->button == Mouse::Button::Left)
                {
                    Vector2f mousePos(
                        static_cast<float>(mouseButton->position.x),
                        static_cast<float>(mouseButton->position.y));

                    bool buttonClicked = false;
                    FloatRect btnAddUser(Vector2f(720.0f + sidebarOffset, 520.0f), Vector2f(175.0f, 35.0f));
                    FloatRect btnAddEdge(Vector2f(905.0f + sidebarOffset, 520.0f), Vector2f(175.0f, 35.0f));
                    FloatRect btnRename(Vector2f(720.0f + sidebarOffset, 565.0f), Vector2f(175.0f, 35.0f));
                    FloatRect btnDelete(Vector2f(905.0f + sidebarOffset, 565.0f), Vector2f(175.0f, 35.0f));

                    if (btnAddUser.contains(mousePos)) { currentState = WAITING_FOR_NODE_POSITION; buttonClicked = true; }
                    else if (btnAddEdge.contains(mousePos)) { currentState = WAITING_FOR_EDGE_NODE1; buttonClicked = true; }
                    else if (btnRename.contains(mousePos)) { 
                        if (activeUserID != -1) { currentState = WAITING_FOR_RENAME; inputString = ""; }
                        buttonClicked = true; 
                    }
                    else if (btnDelete.contains(mousePos)) { 
                        if (activeUserID != -1) { 
                            n.removeUser(activeUserID); 
                            activeUserID = -1; 
                            suggestionCount = 0;
                            currentState = NORMAL;
                        }
                        buttonClicked = true; 
                    }

                    if (!buttonClicked){
                        if (currentState == WAITING_FOR_NODE_POSITION){
                            if (mousePos.x < 700.0f){
                                newNodePos = mousePos;
                                currentState = WAITING_FOR_NODE_NAME;
                                inputString = "";
                            }
                        }else if(currentState != WAITING_FOR_NODE_NAME && currentState != WAITING_FOR_RENAME){
                            UserNode *clickUser = n.getMasterHead();
                            bool userClicked = false;
                            while (clickUser != nullptr){
                                CircleShape nodeCircle(20.0f);
                                nodeCircle.setPosition(Vector2f(clickUser->x - 20.0f, clickUser->y - 20.0f));
                                if (nodeCircle.getGlobalBounds().contains(mousePos)){
                                    userClicked = true;
                                    if (currentState == WAITING_FOR_EDGE_NODE1){
                                        edgeNode1 = clickUser->id;
                                        currentState = WAITING_FOR_EDGE_NODE2;
                                    } else if (currentState == WAITING_FOR_EDGE_NODE2){
                                        if (clickUser->id != edgeNode1){
                                            n.addFriendship(edgeNode1, clickUser->id);
                                            currentState = NORMAL;
                                            if (activeUserID != -1) {
                                                suggestionCount = RecommendationEngine::computeMutualFriends(n.getMasterHead(), activeUserID, suggestions);
                                            }
                                        }
                                    } else {
                                        activeUserID = clickUser->id;
                                        suggestionCount = RecommendationEngine::computeMutualFriends(
                                            n.getMasterHead(),
                                            activeUserID,
                                            suggestions);
                                        currentState = NORMAL;
                                    }
                                    break;
                                }
                                clickUser = clickUser->next;
                            }
                            
                            if (!userClicked && mousePos.x < 700.0f)
                            {
                                activeUserID = -1;
                                suggestionCount = 0;
                                currentState = NORMAL;
                            }
                        }
                    }
                }
            }
        }

        window.clear(Color(10, 12, 16));

        for (int i = 0; i < 4; i++)
        {
            float radius = 70.0f + static_cast<float>(i) * 18.0f;
            CircleShape glow(radius);
            glow.setPointCount(60);
            glow.setOrigin(Vector2f(radius, radius));
            glow.setPosition(Vector2f(
                120.0f + static_cast<float>(i) * 140.0f + sin(animationTime * 0.6f + static_cast<float>(i)) * 10.0f,
                120.0f + static_cast<float>(i % 2) * 110.0f + cos(animationTime * 0.5f + static_cast<float>(i)) * 8.0f));
            glow.setFillColor(Color(255, 255, 255, 10));
            window.draw(glow);
        }

        RectangleShape sidebar(Vector2f(400.0f, 700.0f));
        sidebar.setPosition(Vector2f(700.0f + sidebarOffset, 0.0f));
        sidebar.setFillColor(Color(42, 44, 54, static_cast<uint8_t>(235.0f * sidebarAlpha)));
        window.draw(sidebar);

        Text titleText(font, "Social n Analyzer", 22);
        titleText.setFillColor(Color(255, 255, 255, static_cast<uint8_t>(255.0f * sidebarAlpha)));
        titleText.setPosition(Vector2f(720.0f + sidebarOffset, 20.0f));
        titleText.setStyle(Text::Bold);
        window.draw(titleText);

        Text subtitleText(font, "Click a user to view details", 14);
        subtitleText.setFillColor(Color(200, 200, 200, static_cast<uint8_t>(255.0f * sidebarAlpha)));
        subtitleText.setPosition(Vector2f(720.0f + sidebarOffset, 50.0f));
        window.draw(subtitleText);

        VertexArray edges(PrimitiveType::Lines);

        UserNode *currentUser = n.getMasterHead();

        while (currentUser != nullptr)
        {
            FriendNode *currentFriend = currentUser->friendsHead;

            while (currentFriend != nullptr)
            {
                UserNode *friendUser = n.findUser(currentFriend->friendID);

                if (friendUser != nullptr)
                {
                    if (currentUser->id < friendUser->id)
                    {
                        Vertex startVertex;
                        startVertex.position = Vector2f(currentUser->x, currentUser->y);
                        startVertex.color = Color(180, 180, 180, static_cast<uint8_t>(145.0f + 10.0f * sin(animationTime * 2.0f)));

                        Vertex endVertex;
                        endVertex.position = Vector2f(friendUser->x, friendUser->y);
                        endVertex.color = Color(180, 180, 180, static_cast<uint8_t>(145.0f + 10.0f * sin(animationTime * 2.0f)));

                        edges.append(startVertex);
                        edges.append(endVertex);
                    }
                }

                currentFriend = currentFriend->next;
            }

            currentUser = currentUser->next;
        }

        window.draw(edges);

        currentUser = n.getMasterHead();

        while (currentUser != nullptr)
        {
            Color nodeColor = Color(255, 255, 255);
            float nodeBob = sin(animationTime * 1.6f + static_cast<float>(currentUser->id)) * 3.0f;
            float nodeScale = 1.0f + sin(animationTime * 2.0f + static_cast<float>(currentUser->id)) * 0.03f;

            if (activeUserID != -1 && currentUser->id == activeUserID)
            {
                nodeColor = Color(70, 130, 255);
                nodeScale = 1.08f + sin(animationTime * 4.0f) * 0.04f;
            }
            else if (activeUserID != -1)
            {
                UserNode *activeUser = n.findUser(activeUserID);
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
                    nodeColor = Color(80, 200, 120);
                }
                else
                {
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
                        nodeColor = Color(255, 215, 0);
                        nodeScale = 1.03f + sin(animationTime * 3.0f + static_cast<float>(currentUser->id)) * 0.02f;
                    }
                    else
                    {
                        nodeColor = Color(255, 255, 255);
                    }
                }
            }

            CircleShape nodeCircle(20.0f);
            nodeCircle.setPointCount(50);
            nodeCircle.setOrigin(Vector2f(20.0f, 20.0f));
            nodeCircle.setScale(Vector2f(nodeScale, nodeScale));
            nodeCircle.setPosition(Vector2f(currentUser->x, currentUser->y + nodeBob));
            nodeCircle.setFillColor(nodeColor);
            nodeCircle.setOutlineThickness(2.0f);
            nodeCircle.setOutlineColor(Color(255, 255, 255));
            window.draw(nodeCircle);

            Text nameLabel(font, currentUser->name, 12);
            nameLabel.setFillColor(Color(235, 235, 235));

            FloatRect textBounds = nameLabel.getLocalBounds();
            nameLabel.setPosition(Vector2f(
                currentUser->x - (textBounds.position.x + textBounds.size.x / 2.0f),
                currentUser->y + 25.0f - textBounds.position.y));
            window.draw(nameLabel);

            currentUser = currentUser->next;
        }

        if (activeUserID != -1)
        {
            UserNode *activeUser = n.findUser(activeUserID);

            if (activeUser != nullptr)
            {
                Text profileHeader(font, "Active User Profile", 18);
                profileHeader.setFillColor(Color(70, 130, 255));
                profileHeader.setPosition(Vector2f(720.0f + sidebarOffset, 90.0f));
                profileHeader.setStyle(Text::Bold);
                window.draw(profileHeader);

                Text userName(font, "Name: " + activeUser->name, 16);
                userName.setFillColor(Color(255, 255, 255));
                userName.setPosition(Vector2f(720.0f + sidebarOffset, 120.0f));
                window.draw(userName);

                int friendCount = 0;
                FriendNode *friendCounter = activeUser->friendsHead;
                while (friendCounter != nullptr)
                {
                    friendCount++;
                    friendCounter = friendCounter->next;
                }

                ostringstream oss;
                oss << "Direct Friends: " << friendCount;
                Text friendCountText(font, oss.str(), 16);
                friendCountText.setFillColor(Color(255, 255, 255));
                friendCountText.setPosition(Vector2f(720.0f + sidebarOffset, 145.0f));
                window.draw(friendCountText);

                Text friendsListHeader(font, "Friend List:", 14);
                friendsListHeader.setFillColor(Color(80, 200, 120));
                friendsListHeader.setPosition(Vector2f(720.0f + sidebarOffset, 175.0f));
                friendsListHeader.setStyle(Text::Bold);
                window.draw(friendsListHeader);

                FriendNode *friendList = activeUser->friendsHead;
                int yOffset = 200;

                while (friendList != nullptr)
                {
                    UserNode *friendUser = n.findUser(friendList->friendID);

                    if (friendUser != nullptr)
                    {
                        Text friendName(font, "- " + friendUser->name, 13);
                        friendName.setFillColor(Color(200, 200, 200, static_cast<uint8_t>(255.0f * sidebarAlpha)));
                        friendName.setPosition(Vector2f(730.0f + sidebarOffset, static_cast<float>(yOffset)));
                        window.draw(friendName);

                        yOffset += 20;
                    }

                    friendList = friendList->next;
                }

                Text recommendHeader(font, "People You May Know:", 16);
                recommendHeader.setFillColor(Color(255, 215, 0));
                recommendHeader.setPosition(Vector2f(720.0f + sidebarOffset, static_cast<float>(yOffset + 20)));
                recommendHeader.setStyle(Text::Bold);
                window.draw(recommendHeader);

                yOffset += 50;

                if (suggestionCount == 0)
                {
                    Text noSuggestions(font, "No suggestions available", 13);
                    noSuggestions.setFillColor(Color(150, 150, 150));
                    noSuggestions.setPosition(Vector2f(730.0f + sidebarOffset, static_cast<float>(yOffset)));
                    window.draw(noSuggestions);
                }
                else
                {
                    for (int i = 0; i < suggestionCount; i++)
                    {
                        UserNode *suggestedUser = n.findUser(suggestions[i].userID);

                        if (suggestedUser != nullptr)
                        {
                            ostringstream suggestionText;
                            suggestionText << "- " << suggestedUser->name
                                           << " (" << suggestions[i].mutualCount
                                           << " mutual)";

                            Text suggestion(font, suggestionText.str(), 13);
                            suggestion.setFillColor(Color(220, 220, 220, static_cast<uint8_t>(255.0f * sidebarAlpha)));
                            suggestion.setPosition(Vector2f(730.0f + sidebarOffset, static_cast<float>(yOffset)));
                            window.draw(suggestion);

                            yOffset += 22;
                        }
                    }
                }
            }
        }
        else
        {
            Text instructionText(font, "Select a user node\nto view their profile\nand friend suggestions", 16);
            instructionText.setFillColor(Color(220, 220, 220));
            instructionText.setPosition(Vector2f(750.0f + sidebarOffset, 250.0f));
            window.draw(instructionText);
        }


        CircleShape blueCircle(8.0f);
        blueCircle.setPointCount(30);
        blueCircle.setPosition(Vector2f(720.0f + sidebarOffset, 645.0f));
        blueCircle.setFillColor(Color(70, 130, 255));
        window.draw(blueCircle);

        Text blueLabel(font, "Active User", 12);
        blueLabel.setFillColor(Color(200, 200, 200));
        blueLabel.setPosition(Vector2f(740.0f + sidebarOffset, 645.0f));
        window.draw(blueLabel);

        CircleShape greenCircle(8.0f);
        greenCircle.setPointCount(30);
        greenCircle.setPosition(Vector2f(850.0f + sidebarOffset, 645.0f));
        greenCircle.setFillColor(Color(80, 200, 120));
        window.draw(greenCircle);

        Text greenLabel(font, "Direct Friend", 12);
        greenLabel.setFillColor(Color(200, 200, 200));
        greenLabel.setPosition(Vector2f(870.0f + sidebarOffset, 645.0f));
        window.draw(greenLabel);

        CircleShape goldCircle(8.0f);
        goldCircle.setPointCount(30);
        goldCircle.setPosition(Vector2f(990.0f + sidebarOffset, 645.0f));
        goldCircle.setFillColor(Color(255, 215, 0));
        window.draw(goldCircle);

        Text goldLabel(font, "Suggested", 12);
        goldLabel.setFillColor(Color(200, 200, 200));
        goldLabel.setPosition(Vector2f(1010.0f + sidebarOffset, 645.0f));
        window.draw(goldLabel);

        auto drawButton = [&](const string& text, float x, float y, bool active) {
            RectangleShape btn(Vector2f(175.0f, 35.0f));
            btn.setPosition(Vector2f(x, y));
            btn.setFillColor(active ? Color(100, 150, 200, 200) : Color(60, 65, 75, 200));
            btn.setOutlineThickness(1.0f);
            btn.setOutlineColor(Color(100, 100, 100));
            window.draw(btn);
            
            Text btnText(font, text, 14);
            btnText.setFillColor(Color::White);
            FloatRect textBounds = btnText.getLocalBounds();
            btnText.setPosition(Vector2f(x + (175.0f - textBounds.size.x) / 2.0f, y + 8.0f));
            window.draw(btnText);
        };

        drawButton("Add User", 720.0f + sidebarOffset, 520.0f, currentState == WAITING_FOR_NODE_POSITION || currentState == WAITING_FOR_NODE_NAME);
        drawButton("Add Friend", 905.0f + sidebarOffset, 520.0f, currentState == WAITING_FOR_EDGE_NODE1 || currentState == WAITING_FOR_EDGE_NODE2);
        drawButton("Rename User", 720.0f + sidebarOffset, 565.0f, currentState == WAITING_FOR_RENAME);
        drawButton("Delete User", 905.0f + sidebarOffset, 565.0f, false);

        if (currentState != NORMAL) {
            Text alertText(font, "", 16);
            alertText.setFillColor(Color(255, 200, 50));
            
            if (currentState == WAITING_FOR_NODE_POSITION) alertText.setString("Action: Click empty space on the canvas to place a new user node.");
            else if (currentState == WAITING_FOR_NODE_NAME) alertText.setString("Action: Type new user's name and press Enter -> " + inputString);
            else if (currentState == WAITING_FOR_EDGE_NODE1) alertText.setString("Action: Click the first user node for the new friendship.");
            else if (currentState == WAITING_FOR_EDGE_NODE2) alertText.setString("Action: Click the second user node to complete the friendship.");
            else if (currentState == WAITING_FOR_RENAME) alertText.setString("Action: Type new name and press Enter -> " + inputString);
            
            alertText.setPosition(Vector2f(20.0f, 660.0f));
            window.draw(alertText);
        }
        window.display();
    }
}