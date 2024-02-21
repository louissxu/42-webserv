#pragma once

#include <iostream>
#include <string>

#include "HTTPResponse.hpp"

class HTTPResponse;

class Message
{
private:
    std::string message;
	int			bufferSent;

public:
    Message();
    Message(const HTTPResponse &_resp);
    ~Message();

    // Getter
    const std::string &getMessage() const;
    int getMessageSize() const;
	const int &getBufferSent() const;

	// setter
	void setBufferSent(int buffer);

    // Serialization method
    void serialize(const HTTPResponse &_resp);
};