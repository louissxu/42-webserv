#pragma once
#ifndef ServerManager_HPP
#define ServerManager_HPP

#include <poll.h>
#include <sys/event.h>

#include "Server.hpp"

#include <vector>

class ServerManager {
  public:
    ServerManager();
    ~ServerManager();

    void addServer(Server& server);
    // void addConnection(Connection& connection);

    void runPoll();
    void runKQueueEventLoop();

  private:
    // struct pollfd *_pfds;
    // nfds_t _pfds_count;
    // nfds_t _pfds_array_size;
    std::vector<Server> _servers;

    int _kq;

    // void extendPfdArray(int amount = 10);



    ServerManager(ServerManager& other);
    ServerManager& operator=(ServerManager& other);
};

#endif