/*
** EPITECH PROJECT, 2020
** B-CPP-501-MPL-5-1-rtype-antoine.maillard
** File description:
** GameServer
*/

#include "server/Class/GameServer/GameServer.hpp"

GameServer::GameServer(): UDPServer()
{
}

GameServer::~GameServer()
{
}

void GameServer::work()
{
    std::cout<<"test"<<std::endl;
}