#pragma once
#ifndef ServerManager_HPP
#define ServerManager_HPP

#include <poll.h>

#include "Server.hpp"

#include <vector>

class ServerManager {
  public:
    ServerManager();
    ~ServerManager();

    void addServer(const Server& server);
    // void addConnection(Connection& connection);

    void runPoll();

  private:
    // struct pollfd *_pfds;
    // nfds_t _pfds_count;
    // nfds_t _pfds_array_size;
    std::vector<Server> _servers;

    // void extendPfdArray(int amount = 10);



    ServerManager(ServerManager& other);
    ServerManager& operator=(ServerManager& other);
};

#endif