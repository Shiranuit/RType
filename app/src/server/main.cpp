/*
** EPITECH PROJECT, 2020
** Untitled (Workspace)
** File description:
** main
*/

#include "server/Class/RTypeServer/RTypeServer.hpp"
#include "lib/Network/NetworkHandler.hpp"

int main(int ac, char *av[]) {
    RTypeServer server;

    server.configure(4000);

    server.run();
    return (0);
}