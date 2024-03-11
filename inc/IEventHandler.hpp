#pragma once
#ifndef IEVENTHANDLER_HPP
#define IEVENTHANDLER_HPP

#include <vector>

class IEventHandler {
  public:
    virtual ~IEventHandler() {};
    virtual std::vector<struct kevent> getEventsToRegister() = 0;
    virtual void handleEvent(struct kevent event) = 0;
};

#endif