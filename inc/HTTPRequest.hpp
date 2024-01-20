#pragma once
#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <iostream>
#include <sstream>

#include "message.hpp"
class HTTPRequest: public Message {
  public:
    HTTPRequest();
    HTTPRequest(std::string request);
    HTTPRequest(HTTPRequest& other);
    HTTPRequest& operator=(HTTPRequest& other);
    virtual ~HTTPRequest();

    void print();

  private:
    void parseString(std::string str);
};

#endif