#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h> // for perror
#include <unistd.h> // for dup

#include <string>
#include <iostream>

#include <vector>
#include "Connection.hpp"

class Server {
  public:
    Server();
    Server(const Server& other);
    Server& operator=(const Server& other);
    ~Server();

    Server(std::string port);

    int getSockFd();
    void acceptNewConnection();
    std::vector<Connection>& getConnections();

  private:
    int _sockfd;
    std::string _port;
    std::string _ip;
    std::vector<Connection> _connections;
};


#endif