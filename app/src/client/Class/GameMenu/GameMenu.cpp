/*
** EPITECH PROJECT, 2020
** B-CPP-501-MPL-5-1-rtype-antoine.maillard
** File description:
** GameMenu
*/

#include "client/Class/GameMenu/GameMenu.hpp"

GameMenu::GameMenu(): scene(MENU),
client(std::make_unique<TCPClient>()),
window(std::make_unique<sf::RenderWindow>(sf::VideoMode(1600, 800), "R-Type Menu", sf::Style::Close | sf::Style::Titlebar))
{
}

GameMenu::~GameMenu()
{
}

int GameMenu::run()
{
    client->connectTo(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("192.168.1.71"), 3000));
    client->run();

    while (window->isOpen()) {
        handleEvents();
    }
    return (0);
}

void GameMenu::handleEvents()
{
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window->close();
    }
}