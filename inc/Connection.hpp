#pragma once
#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <netinet/in.h>
#include <unistd.h> // for dup

#include "HTTPRequest.hpp"

class Connection {
  public:
    Connection();
    Connection(const Connection& other);
    Connection& operator=(const Connection& other);
    ~Connection();

    Connection(int sockfd);

    void receiveData();
    void sendData();

  private:
    int _connection_fd;
    HTTPRequest _req;
};

#endif