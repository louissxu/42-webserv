#pragma once
#ifndef IEVENTHANDLER_HPP
#define IEVENTHANDLER_HPP

class IEventHandler {
  public:
    virtual ~IEventHandler() {};
    virtual void handleEvent() = 0;
};

#endif