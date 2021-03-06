/*
** EPITECH PROJECT, 2020
** B-CPP-501-MPL-5-1-rtype-antoine.maillard
** File description:
** events
*/

#include "client/Class/GameMenu/GameMenu.hpp"

void GameMenu::handleEvents()
{
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            tryQuit();
            window.close();
            return;
        } else if (event.type == sf::Event::TextEntered) {
            if (event.text.unicode < 128) {
                handleTextEntered();
            }
        } else if (event.type == sf::Event::KeyPressed && !this->getGameEntities()->getDead()) {
            handleKeyPressed();
        } else if (event.type == sf::Event::KeyReleased && !this->getGameEntities()->getDead()) {
            handleKeyReleased();
        } else if (this->getGameEntities()->getDead() && (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)) {
            networkHandler.broadcast(MovePacket(playerId, 0, 0));
        }
    }
}

void GameMenu::handleKeyPressed()
{
    if (gameEntities.isGamePlaying()) {
        sf::Vector2i dir(0, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            networkHandler.broadcast(FirePacket(playerId, true));
            laserSound.play(10.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) || (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))) {
            dir.y += -1;
            isDirectionMaintained[GameEntities::UP] = true;
            movementSound.play(5.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) || (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))) {
            dir.x += -1;
            isDirectionMaintained[GameEntities::LEFT] = true;
            movementSound.play(5.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))) {
            dir.y += 1;
            isDirectionMaintained[GameEntities::DOWN] = true;
            movementSound.play(5.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))) {
            dir.x += 1;
            isDirectionMaintained[GameEntities::RIGHT] = true;
            movementSound.play(5.0f);
        }
        networkHandler.broadcast(MovePacket(playerId, dir.x, dir.y));
    }
}

void GameMenu::handleTextEntered()
{
    char charTyped = static_cast<char>(event.text.unicode);

    if (std::isalnum(charTyped)) {
        if (getScene() == sceneName::LOGIN) {
            if (username == "...") {
                username = "";
            }
            if (username.length() < 16)
                username += charTyped;
            setDrawableTextStr(sceneName::LOGIN, "usernameInput", username);
            setDrawableTextStr(sceneName::MENU, "username", username);
        } else if (getScene() == sceneName::CREATE || getScene() == sceneName::JOIN) {
            if (roomname == "...") {
                roomname = "";
            }
            if (roomname.length() < 16)
                roomname += charTyped;
            setDrawableTextStr(getScene(), "roomnameInput", roomname);
            setDrawableTextStr(sceneName::ROOM, "roomname", roomname);
        }
    }
}

void GameMenu::handleKeyReleased()
{
    if (gameEntities.isGamePlaying()) {
        sf::Vector2i dir(0, 0);
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            networkHandler.broadcast(FirePacket(playerId, false));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) || (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))) {
            dir.y += -1;
        } else {
            isDirectionMaintained[GameEntities::UP] = false;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) || (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))) {
            dir.x += -1;
        } else {
            isDirectionMaintained[GameEntities::LEFT] = false;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))) {
            dir.y += 1;
        } else {
            isDirectionMaintained[GameEntities::DOWN] = false;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))) {
            dir.x += 1;
        } else {
            isDirectionMaintained[GameEntities::RIGHT] = false;
        }
        networkHandler.broadcast(MovePacket(playerId, dir.x, dir.y));
    } else {
        if (event.key.code == sf::Keyboard::Backspace) {
            if (getScene() == sceneName::LOGIN) {
                if (username.length() > 0) {
                    username.erase(username.size() - 1, 1);
                    setDrawableTextStr(sceneName::LOGIN, "usernameInput", username);
                    setDrawableTextStr(sceneName::MENU, "username", username);
                }
            } else if (getScene() == sceneName::CREATE || getScene() == sceneName::JOIN) {
                if (roomname.length() > 0) {
                    roomname.erase(roomname.size() - 1, 1);
                    setDrawableTextStr(getScene(), "roomnameInput", roomname);
                    setDrawableTextStr(sceneName::ROOM, "roomname", roomname);
                }
            }
        } else if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Space) {
            if (getScene() == sceneName::LOGIN) {
                if (isNameValid(username)) {
                    tryLogIn();
                }
            } else if (getScene() == sceneName::MENU) {
                if (actualButton == menuButton::B_CREATE) {
                    setScene(sceneName::CREATE);
                } else if (actualButton == menuButton::B_JOIN) {
                    setScene(sceneName::JOIN);
                }
            } else if (getScene() == sceneName::CREATE) {
                if (roomname.length() >= 3)
                    tryCreateRoom();
            } else if (getScene() == sceneName::JOIN) {
                if (roomname.length() >= 3)
                    tryJoinRoom();
            } else if (getScene() == sceneName::ROOM) {
                tryChangeUserStatus();
            }
        } else if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::Up) {
            if (getScene() == sceneName::MENU) {
                dynamic_cast<sf::Text *>(modDrawables.at(sceneName::MENU)[menuButtons[actualButton]].get())->setFillColor(sf::Color::White);
                actualButton = actualButton == menuButton::B_CREATE ? menuButton::B_JOIN : menuButton::B_CREATE;
                dynamic_cast<sf::Text *>(modDrawables.at(sceneName::MENU)[menuButtons[actualButton]].get())->setFillColor(sf::Color::Yellow);
            }
        } else if (event.key.code == sf::Keyboard::Escape) {
            if (getScene() == sceneName::ROOM) {
                disconnectRoom();
            }
            setScene(sceneName::MENU);
        }
    }
}