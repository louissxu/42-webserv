#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest()
{
	isCgi = false;
}

HTTPRequest::HTTPRequest(std::string _method, std::string _version, std::string _uri, std::map<std::string, std::string> _headers, std::string _body) : method(_method), version(_version), uri(_uri), headers(_headers), body(_body)
{
	this->setIsCgi();
}

// HTTPRequest::HTTPRequest(char *request, int len) {}

// HTTPRequest::HTTPRequest(std::string _method, std::string _uri) {}

HTTPRequest::HTTPRequest(HTTPRequest const &other)
{
	*this = other;
}

HTTPRequest &HTTPRequest::operator=(HTTPRequest const &other)
{
	this->method = other.method;
	this->uri = other.uri;
	return *this;
}

HTTPRequest::~HTTPRequest()
{
}

void HTTPRequest::setMethod(std::string const &_method)
{
	this->method = _method;
}

void HTTPRequest::setUri(std::string const &_uri)
{
	this->uri = _uri;
}

HTTPRequest HTTPRequest::deserialize(char *requestMessage, int requestLen)
{
	(void)requestLen;
	std::string key, value; // header, value
	std::string method;
	std::string version;
	std::string uri;
	std::map<std::string, std::string> headers;
	std::string body;

	std::stringstream ss(requestMessage);
	std::string line;

	std::getline(ss, line, '\n');
	std::stringstream line_stream(line);
	std::getline(line_stream, method, ' ');
	std::getline(line_stream, uri, ' ');
	std::getline(line_stream, version, '\r');

	// Parse headers
	while (std::getline(ss, line) && !line.empty())
	{
		size_t colonPos = line.find(':');
		if (colonPos != std::string::npos)
		{
			std::string key = line.substr(0, colonPos);
			std::string value = line.substr(colonPos + 2); // Skip ': ' after colon
			headers[key] = value;
		}
	}
	// Get the rest as the body
	body = ss.str();
	// Remove headers from the body
	body.erase(0, ss.tellg());
	return HTTPRequest(method, version, uri, headers, body);
}
// #include <iostream>
void HTTPRequest::setIsCgi()
{
	isCgi = false;
	if (uri.compare(0, 7, "/cgiBin") == 0)
		isCgi = true;
}
