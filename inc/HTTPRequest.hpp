#pragma once

#include <map>
#include <sstream>
// #include <

// #include "message.hpp"
class HTTPRequest
{
public:
	HTTPRequest();
	HTTPRequest(std::string method, std::string version, std::string uri, std::map<std::string, std::string> headers, std::string body);
	// HTTPRequest(char *request, int len);
	// HTTPRequest(std::string method, std::string uri);
	HTTPRequest(HTTPRequest const &other);
	HTTPRequest &operator=(HTTPRequest const &other);
	~HTTPRequest();

	std::string const &getMethod() const;
	std::string const &getUri() const;
	void setMethod(std::string const &_method);
	void setUri(std::string const &_uri);

	void setIsCgi();
	// void setFile(std::string const &uri);

	static HTTPRequest deserialize(char *requestMessage, int requestLen);

private:
	std::string method;
	std::string version;
	std::string uri;
	std::map<std::string, std::string> headers;
	std::string body;
private:
	bool isCgi;
	// std::string File;
};