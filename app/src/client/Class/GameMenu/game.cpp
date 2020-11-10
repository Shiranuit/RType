/*
** EPITECH PROJECT, 2020
** B-CPP-501-MPL-5-1-rtype-antoine.maillard
** File description:
** join
*/

#include "client/Class/GameMenu/GameMenu.hpp"

void GameMenu::tryStartGame()
{
    buffer.clear();

    buffer.writeUInt(sizeof(int) + sizeof(unsigned int) + roomname.length() + 1);
    buffer.writeInt(req::Type::StartGame);
    buffer.writeCharBuffer(roomname.c_str());

    std::cout << "TryStartGame: sending " << buffer.getSize() << " bytes." << std::endl;
    client->sendData(buffer.flush(), buffer.getSize());
}

void GameMenu::initFixedGameDrawables()
{
    fixedDrawables.at(sceneName::JOIN).push_back(std::make_unique<sf::Sprite>());
    dynamic_cast<sf::Sprite *>(LAST_FIXED_SPRITE(sceneName::JOIN).get())->setTexture(*loadedTextures["background"]);
    dynamic_cast<sf::Sprite *>(LAST_FIXED_SPRITE(sceneName::JOIN).get())->setOrigin(sf::Vector2f(960, 480));
    dynamic_cast<sf::Sprite *>(LAST_FIXED_SPRITE(sceneName::JOIN).get())->setPosition(sf::Vector2f(800, 400));

    fixedDrawables.at(sceneName::JOIN).push_back(std::make_unique<sf::Text>());
    mainFont.loadFromFile("./app/assets/fonts/pixelart.ttf");
    dynamic_cast<sf::Text *>(LAST_FIXED_SPRITE(sceneName::JOIN).get())->setFont(mainFont);
    dynamic_cast<sf::Text *>(LAST_FIXED_SPRITE(sceneName::JOIN).get())->setString("Room name to join:");
    dynamic_cast<sf::Text *>(LAST_FIXED_SPRITE(sceneName::JOIN).get())->setCharacterSize(40);
    dynamic_cast<sf::Text *>(LAST_FIXED_SPRITE(sceneName::JOIN).get())->setFillColor(sf::Color::White);
    dynamic_cast<sf::Text *>(LAST_FIXED_SPRITE(sceneName::JOIN).get())->setStyle(sf::Text::Bold);
    dynamic_cast<sf::Text *>(LAST_FIXED_SPRITE(sceneName::JOIN).get())->setPosition(sf::Vector2f(520, 280));

    fixedDrawables.at(sceneName::JOIN).push_back(std::make_unique<sf::Sprite>());
    sf::Texture backInput;
    backInput.create(600, 80);
    dynamic_cast<sf::Sprite *>(LAST_FIXED_SPRITE(sceneName::JOIN).get())->setTexture(backInput);
    dynamic_cast<sf::Sprite *>(LAST_FIXED_SPRITE(sceneName::JOIN).get())->setColor(sf::Color::Black);
    dynamic_cast<sf::Sprite *>(LAST_FIXED_SPRITE(sceneName::JOIN).get())->setOrigin(sf::Vector2f(300, 40));
    dynamic_cast<sf::Sprite *>(LAST_FIXED_SPRITE(sceneName::JOIN).get())->setPosition(sf::Vector2f(800, 400));

    fixedDrawables.at(sceneName::JOIN).push_back(std::make_unique<sf::Text>());
    dynamic_cast<sf::Text *>(LAST_FIXED_SPRITE(sceneName::JOIN).get())->setFont(mainFont);
    dynamic_cast<sf::Text *>(LAST_FIXED_SPRITE(sceneName::JOIN).get())->setString("Only alphanumeric room name is authorized.");
    dynamic_cast<sf::Text *>(LAST_FIXED_SPRITE(sceneName::JOIN).get())->setCharacterSize(30);
    dynamic_cast<sf::Text *>(LAST_FIXED_SPRITE(sceneName::JOIN).get())->setFillColor(sf::Color::Red);
    dynamic_cast<sf::Text *>(LAST_FIXED_SPRITE(sceneName::JOIN).get())->setStyle(sf::Text::Bold);
    dynamic_cast<sf::Text *>(LAST_FIXED_SPRITE(sceneName::JOIN).get())->setPosition(sf::Vector2f(350, 480));
}

void GameMenu::initModGameDrawables()
{
    modDrawables.at(sceneName::JOIN).insert(std::pair<std::string, std::unique_ptr<sf::Drawable>>("roomnameInput", std::make_unique<sf::Text>()));
    dynamic_cast<sf::Text *>(modDrawables.at(sceneName::JOIN)["roomnameInput"].get())->setFont(mainFont);
    dynamic_cast<sf::Text *>(modDrawables.at(sceneName::JOIN)["roomnameInput"].get())->setString("...");
    dynamic_cast<sf::Text *>(modDrawables.at(sceneName::JOIN)["roomnameInput"].get())->setCharacterSize(35);
    dynamic_cast<sf::Text *>(modDrawables.at(sceneName::JOIN)["roomnameInput"].get())->setFillColor(sf::Color::White);
    dynamic_cast<sf::Text *>(modDrawables.at(sceneName::JOIN)["roomnameInput"].get())->setStyle(sf::Text::Bold);
    dynamic_cast<sf::Text *>(modDrawables.at(sceneName::JOIN)["roomnameInput"].get())->setPosition(sf::Vector2f(520, 375));
}