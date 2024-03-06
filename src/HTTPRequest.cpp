#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest()
{
	method = GET;
	version = HTTP_1_1;
	uri = "/";
}

HTTPRequest::HTTPRequest(std::map<std::string, std::string> const &_headers, std::string const &_body, Method const &_method, std::string const &_uri, Version const &_version, bool _isCGI)
	: headers(_headers), body(_body), method(_method), uri(_uri), version(_version), isCGI(_isCGI)
{
	// (void)isCGI;
}

HTTPRequest::~HTTPRequest()
{
}

// !Might have to append value in some cases research further
void HTTPRequest::setHeader(std::string const &key, std::string const &value)
{
	std::map<std::string, std::string>::iterator it = headers.find(key);
	if (it == headers.end())
		headers.insert(std::pair<std::string, std::string>(key, value));
	else
		it->second = value;
}

std::map<std::string, std::string> const &HTTPRequest::getHeaders() const
{
	return this->headers;
}

std::string const &HTTPRequest::getHeader(std::string const &key) const
{
	static std::string emptystring;
	std::map<std::string, std::string>::const_iterator it = headers.find(key);
	if (it != headers.end())
		return it->second;
	return emptystring;
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

std::string HTTPRequest::getMethodString() const
{
	switch (this->method)
	{
	case POST:
		return "POST";
		break;
	case GET:
		return "GET";
		break;
	case HEAD:
		return "HEAD";
		break;
	case PUT:
		return "PUT";
		break;
	case DELETE:
		return "DELETE";
	case TRACE:
		return "TRACE";
		break;
	case OPTIONS:
		return "OPTIONS";
		break;
	case CONNECT:
		return "CONNECT";
		break;
	case PATCH:
		return "PATCH";
		break;
	default:
		return std::string();
		break;
	}
}

Version const &HTTPRequest::getVersion() const
{
	return this->version;
}

bool const &HTTPRequest::getCGIStatus() const
{
	return this->isCGI;
}