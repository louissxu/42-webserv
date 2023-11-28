#pragma once
#ifndef SOCKETMANAGER_HPP
#define SOCKETMANAGER_HPP

#include <poll.h>

#include "Server.hpp"
#include "Connection.hpp"

#include <vector>

class SocketManager {
  public:
    SocketManager();
    ~SocketManager();

    void addServer(Server& server);
    void addConnection(Connection& connection);

    void checkPoll();

  private:
    struct pollfd *_pfds;
    nfds_t _pfds_count;
    nfds_t _pfds_array_size;
    std::vector<Server> _servers;
    std::vector<Connection> _connections;
    std::vector<int> _type; // 1 for server listen fd;
    // std::vector<bool> is_listen;

    void extendPfdArray(int amount = 10);



    SocketManager(SocketManager& other);
    SocketManager& operator=(SocketManager& other);
};

#endif