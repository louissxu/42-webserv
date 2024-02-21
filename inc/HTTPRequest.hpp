#pragma once

#include <map>
#include <sstream>

enum Method
{
	GET,
	HEAD,
	POST,
	PUT,
	DELETE,
	TRACE,
	OPTIONS,
	CONNECT,
	PATCH
};

enum Version
{
	HTTP_1_0,
	HTTP_1_1,
	HTTP_2_0
};

class HTTPRequest
{
private:
	std::map<std::string, std::string> headers;
	std::string body;
	Method method;
	std::string uri;
	Version version;

public:
	HTTPRequest(std::map<std::string,std::string> const &_headers,
				std::string const &_body,
				Method const &_method,
				std::string const &_uri,
				Version const &_version);
	HTTPRequest();
	~HTTPRequest();

	std::map<std::string, std::string> const &getHeaders() const;
	std::string const &getBody() const;
	std::string const &getUri() const;
	Method const &getMethod() const;
	Version const &getVersion() const;
};