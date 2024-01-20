#pragma once
#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>

#include "message.hpp"

class HTTPResponse: public Message {
  public:
    HTTPResponse();
    HTTPResponse(HTTPResponse& other);
    HTTPResponse& operator=(HTTPResponse& other);
    virtual ~HTTPResponse();

    // void print();

  private:
    void generateResponse();
    std::string getFileName( std::string uri ) const;
    // std::string Response();
    // std::string getFileContents();
    std::string _response;
};

#endif