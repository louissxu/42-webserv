#pragma once
#ifndef SOCKETMANAGER_HPP
#define SOCKETMANAGER_HPP

#include <poll.h>

#include "Server.hpp"

#include <vector>

class SocketManager {
  public:
    SocketManager();
    ~SocketManager();

    void addServer(const Server& server);
    // void addConnection(Connection& connection);

    void runPoll();

  private:
    // struct pollfd *_pfds;
    // nfds_t _pfds_count;
    // nfds_t _pfds_array_size;
    std::vector<Server> _servers;

    // void extendPfdArray(int amount = 10);



    SocketManager(SocketManager& other);
    SocketManager& operator=(SocketManager& other);
};

#endif