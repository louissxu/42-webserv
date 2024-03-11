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
#include <sys/event.h> // for EV_SET
#include <fcntl.h> // for fcntl

#include <string>
#include <iostream>

#include <vector>
#include "Connection.hpp"
#include "IEventHandler.hpp"
#include "QueueManager.hpp"


class Server : public IEventHandler {
  public:
    Server(const Server& other);
    Server& operator=(const Server& other);
    virtual ~Server();

    Server(QueueManager& qm, std::string port);

    int getSockFd();
    void acceptNewConnection();
    std::vector<Connection>& getConnections();
    virtual std::vector<struct kevent> getEventsToRegister();
    virtual void handleEvent(struct kevent event);

  private:
    QueueManager& _qm;
    int _sockfd;
    std::string _port;
    std::string _ip;
    std::vector<Connection> _connections;
    
    Server();
};


#endif