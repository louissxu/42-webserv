#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h> // for perror

#include <string>
#include <iostream>

class Server {
  public:
    Server();
    Server(Server& other);
    Server(std::string port);
    Server& operator=(Server& other);
    ~Server();

    int getSockFd();

  private:
    int _sockfd;
};


#endif