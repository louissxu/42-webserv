#pragma once

#include <iostream>
#include <string>

#include "HTTPResponse.hpp"

class HTTPResponse;

class Message
{
private:
    std::string message;

public:
    Message();
    Message(const HTTPResponse &_resp);
    ~Message();

    // Getter
    const std::string &getMessage() const;

    // Serialization method
    void serialize(const HTTPResponse &_resp);
};


// class Message
// {
// private:
// 	std::string message;
// 	int bufferSend;

// public:
// 	Message();
// 	Message(HTTPResponse const &_resp);
// 	~Message();

// 	// getters
// 	std::string const &getMessage() const;
// 	int const &getBuffer() const;
	
// 	// setters
// 	void setBuffer(int const &size);

// 	void serialize(HTTPResponse const &_resp);
// };

// #include <map>
// #include <string>
// #include <sstream>

// #include <iostream>

// // enum class Request_Methods {
// // 	PUT,
// // 	POST,
// // 	GET
// // };

// class Message
// {
// public:
// 	Message();
// 	Message(char *request, int len);
// 	// Message(std::string const &request);
// 	virtual ~Message();
// 	Message(Message const &src);
// 	Message &operator=(Message const &src);

// 	void init();

// 	void parseRequest(std::string const &request, int requestLen);
// 	void parseRequest();

// 	// std::map of headers manipulation
// 	void addHeader(std::string line);
// 	void addHeader(std::string key, std::string value);

// 	// getters and setters
// 	std::string getHeaderValue(std::string const &key) const;
// 	std::string getBody() const;

// 	// // Method to serialize the message into a string
// 	// virtual std::string toString() const;

// 	// // Method to deserialize a string into a message
// 	// virtual void fromString(const std::string& data);

// protected:
// 	std::map<std::string, std::string> headers;
// 	std::string body;

// 	char *request;
// 	int requestLen;
// };