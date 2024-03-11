#pragma once
#ifndef QueueManager_HPP
#define QueueManager_HPP

#include <poll.h>
#include <sys/event.h>

#include "Server.hpp"

#include <vector>

class QueueManager {
  public:
    QueueManager();
    ~QueueManager();

    void registerEvents(std::vector<struct kevent> events);
    void runEventLoop();

  private:
    int _kq;

    QueueManager(QueueManager& other);
    QueueManager& operator=(QueueManager& other);
};

#endif