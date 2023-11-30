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

class Server {
  public:
    Server(const Server& other);
    Server& operator=(const Server& other);
    Server(std::string port);
    ~Server();
    Server();

    int getSockFd();

  private:
    int _sockfd;
    std::string _port;
    std::string _ip;
    
};


#endif