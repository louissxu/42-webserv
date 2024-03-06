#pragma once

#include <iostream>
#include <string>

#include "HTTPResponse.hpp"

class HTTPResponse;

class Message
{
private:
    std::string message;
    int bufferSent;

public:
    Message();
    Message(const std::string &_message);
    Message(const HTTPResponse &_resp);
    Message(Message const &src);
    ~Message();
    Message &operator=(Message const &src);

    // Getter
    const std::string &getMessage() const;
    int size() const;
    const int &getBufferSent() const;

    // setter
    void setMessage(std::string const &_message);
    void setBufferSent(int buffer);

    // Serialization method
    void serialize(const HTTPResponse &_resp);
};