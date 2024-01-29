#pragma once
#ifndef ServerManager_HPP
#define ServerManager_HPP

#include <poll.h>

#include "Server.hpp"
#include "Cout.hpp"
#include <vector>

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/event.h>
#include <sys/time.h>

#include "HTTPResponse.hpp"

#define MAX_EVENTS 200 // random value
#define BUFFER_SIZE 3000
class ServerManager {
  public:
    ServerManager();
    ~ServerManager();

    void addServer(const Server& server);
    // void addConnection(Connection& connection);

    void runPoll();

    void runKQ();
    void createQ();
    void acceptNewConnections( int nev );
    void processConnectionIO(int nev );
    bool isListeningSocket(int socket_fd);
    void add_cgi_IO_to_ev_set();


  private:
    std::vector<Server> _servers;
    int kq;
    bool accepting;
    struct kevent *ev_set;
    struct kevent ev_list[MAX_EVENTS];
    int ev_set_count;
    // void extendPfdArray(int amount = 10);

    ServerManager(ServerManager& other);
    ServerManager& operator=(ServerManager& other);
};

#endif