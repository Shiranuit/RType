/*
** EPITECH PROJECT, 2020
** B-CPP-501-MPL-5-1-rtype-antoine.maillard
** File description:
** RTypeServer
*/

#include "server/Class/RTypeServer/RTypeServer.hpp"

RTypeServer::RTypeServer() : TCPServer(), gamesInProgress(32)
{
    for (int i = 0; i < 8; i++) {
        pool.create_thread([this]() {
            Game *value;
            while (!this->done) {
                while (this->gamesInProgress.pop(value)) {
                    value->update();
                    if (value->isGaming()) {
                        this->gamesInProgress.push(value);
                    } else {
                        value->close();
                        this->getGames().erase(value->getRoomName());
                    }
                }
            }
        });
    }
}

RTypeServer::~RTypeServer()
{
    done = true;
    pool.join_all();
}

void RTypeServer::work()
{
    handleConnection();
}

unsigned int RTypeServer::prepareNewClient()
{
    static unsigned int key = 0;

    key += 1;
    std::pair<std::map<unsigned int, std::unique_ptr<TCPClient>>::iterator, bool>
        inserted = clients.insert(std::make_pair(key, std::make_unique<TCPClient>(ioService)));
    if (!inserted.second) { return (0); }
    return (key);
}

bool RTypeServer::handleConnection()
{
    unsigned int client_id = prepareNewClient();

    if (!isClientConnected(client_id)) { return (false); }
    acceptor->async_accept(clients[client_id]->m_socket,
    [this, client_id](const boost::system::error_code &errc) {
        if (!errc) {
            clients[client_id]->setClient();
            this->handleClient(client_id);
            this->handleConnection();
        } else {
            std::cout << "Failed connection client" << std::endl;
        }
    });
    return (true);
}

bool RTypeServer::handleClient(const unsigned int client_id)
{
    if (!isClientConnected(client_id)) { return (false); }
    std::cout << clients[client_id]->getAddress() << ':' << clients[client_id]->getPort() << std::endl;
    clients[client_id]->m_socket.async_read_some(boost::asio::buffer(clients[client_id]->getPacket(), 1024),
    [this, client_id](const boost::system::error_code &errc, std::size_t bytes_transferred) {
        if (!errc) {
            clients[client_id]->getBuffer().clear();
            clients[client_id]->getBuffer().append(clients[client_id]->getPacket(), bytes_transferred);
            unsigned int expectedDataLen = clients[client_id]->getBuffer().readUInt(nullptr);
            if (expectedDataLen == bytes_transferred - 4) {
                handleRequests(client_id);
            } else {
                std::cout << "Error data train doesn't have the good size." << std::endl;
            }
            this->handleClient(client_id);
        } else {
            disconnectClient(client_id);
            for (std::map<std::string, std::unique_ptr<Game>>::iterator it = games.begin(); it != games.end(); ++it) {
                it->second->getLobby().disconnect(client_id);
            }
        }
    });
    return (true);
}

bool RTypeServer::sendData(const unsigned int client_id)
{
    if (!isClientConnected(client_id)) { return (false); }
    clients[client_id]->m_socket.async_write_some(
    boost::asio::buffer(clients[client_id]->getBuffer().flush(), clients[client_id]->getBuffer().getSize()),
    [this, client_id](const boost::system::error_code &ec, std::size_t bytes_transferred)
    {
        if (!ec) {
        } else {
            std::cout << "Data failed to be sent." << std::endl;
        }
    });
    return (true);
}

bool RTypeServer::sendData(const ByteBuffer &buff, const std::vector<unsigned int> &clients_id)
{
    for (size_t i = 0; i < clients_id.size(); i++) {
        if (!isClientConnected(clients_id[i])) break;
        clients[clients_id[i]]->m_socket.async_write_some(
        boost::asio::buffer(buff.flush(), buff.getSize()),
        [this, clients_id, i](const boost::system::error_code &ec, std::size_t bytes_transferred)
        {
            if (!ec) {
            } else {
                std::cout << "Data failed to be sent." << std::endl;
            }
        });
    }
    return (true);
}

bool RTypeServer::disconnectClient(const unsigned int client_id)
{
    if (!isClientConnected(client_id)) { return (false); }
    clients.erase(client_id);
    return (true);
}

bool RTypeServer::isClientConnected(const unsigned int client_id)
{
    return (clients.count(client_id) > 0);
}

const std::map<unsigned int, std::unique_ptr<TCPClient>> &RTypeServer::getClients() const
{
    return (clients);
}