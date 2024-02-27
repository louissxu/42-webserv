#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest()
{
	method = GET;
	version = HTTP_1_1;
	uri = "/";
}

HTTPRequest::HTTPRequest(std::map<std::string, std::string> const &_headers, std::string const &_body, Method const &_method, std::string const &_uri, Version const &_version)
	: headers(_headers), body(_body), method(_method), uri(_uri), version(_version)
{
}

HTTPRequest::~HTTPRequest()
{
}

std::map<std::string, std::string> const &HTTPRequest::getHeaders() const
{
	return this->headers;
}

std::string const &HTTPRequest::getBody() const
{
	return this->body;
}
std::string const &HTTPRequest::getUri() const
{
	return this->uri;
}

Method const &HTTPRequest::getMethod() const
{
	return this->method;
}

Version const &HTTPRequest::getVersion() const
{
	return this->version;
}