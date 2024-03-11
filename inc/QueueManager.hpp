#pragma once
#ifndef QueueManager_HPP
#define QueueManager_HPP

#include <poll.h>
#include <sys/event.h>
#include <vector>
#include <unistd.h>

#include <string>
#include <iostream>

#include "IEventHandler.hpp"

class QueueManager {
  public:
    QueueManager();
    QueueManager(const QueueManager& other);
    QueueManager& operator=(QueueManager& other);
    ~QueueManager();

    void registerEvents(std::vector<struct kevent> events);
    void runEventLoop();

  private:
    int _kq;
};

#endif