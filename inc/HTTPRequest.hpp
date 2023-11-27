#pragma once
#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <iostream>
#include <sstream>

class HTTPRequest {
  public:
    HTTPRequest();
    HTTPRequest(std::string request);
    HTTPRequest(HTTPRequest& other);
    HTTPRequest& operator=(HTTPRequest& other);
    ~HTTPRequest();

    void print();

  private:
    void parseString(std::string str);

    std::string _request_method_name;
    std::string _request_uri;
    std::string _HTTP_version;
};

#endif