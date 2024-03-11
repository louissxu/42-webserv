#pragma once
#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <netinet/in.h>
#include <unistd.h> // for dup
#include <sys/event.h> // for kevent

#include "IEventHandler.hpp"
#include "HTTPRequest.hpp"

class Connection : public IEventHandler {
  public:
    Connection();
    Connection(const Connection& other);
    Connection& operator=(const Connection& other);
    ~Connection();

    Connection(int sockfd);

    void receiveData();
    void sendData();

    int getConnectionFd();

    virtual std::vector<struct kevent> getEventsToRegister();
    virtual void handleEvent(struct kevent event);
    void handleRead();

  private:
    int connection_fd_;
    std::string received_string_;
    HTTPRequest req_;
};

#endif