#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h> // for perror
#include <fcntl.h>
#include <string>
#include <iostream>
#include <vector>

#include "Connection.hpp"

class Connection;

class Server
{
public:
  Server();
  Server(const Server &other);
  Server &operator=(const Server &other);
  ~Server();

  Server(std::string port);

  int getSockFd();
  void acceptNewConnection();
  std::vector<Connection> &getConnections();

private:
  int _sockfd;
  std::string _port;
  std::string _ip;
  std::vector<Connection> *_connections;
};