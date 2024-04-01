#include "HTTPRequest.hpp"

/*------------------------------------------*\
|               CONSTRUCTORS                 |
\*------------------------------------------*/

HTTPRequest::HTTPRequest()
{
	_method = NOTFOUND;
	_version = HTTP_1_1;
	_uri = "/";
}

HTTPRequest::HTTPRequest(std::map<std::string, std::string> const &_headers, std::string const &_body, Method const &_method, std::string const &_uri, Version const &_version, std::string const &qString, bool _isCGI)
	: _headers(_headers), _body(_body), _method(_method), _uri(_uri), _version(_version), _qString(qString), _isCGI(_isCGI)
{
	// (void)isCGI;
}

HTTPRequest::~HTTPRequest() {}

/*------------------------------------------*\
|                 GETTERS                    |
\*------------------------------------------*/

std::map<std::string, std::string> HTTPRequest::getHeaders() const
{
	return this->_headers;
}

std::string HTTPRequest::getHeader(std::string const &key) const
{
	static std::string emptystring;
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it != _headers.end())
		return it->second;
	return emptystring;
}

std::string HTTPRequest::getBody() const
{
	return this->_body;
}
std::string HTTPRequest::getUri() const
{
	return this->_uri;
}

Method HTTPRequest::getMethod() const
{
	return this->_method;
}

std::string HTTPRequest::getMethodString() const
{
	switch (this->_method)
	{
	case POST:
		return "POST";
		break;
	case GET:
		return "GET";
		break;
	case DELETE:
		return "DELETE";
		break;
	default:
		return std::string();
		break;
	}
}

Version HTTPRequest::getVersion() const
{
	return this->_version;
}

bool HTTPRequest::getCGIStatus() const
{
	return this->_isCGI;
}

std::string HTTPRequest::getQString() const
{
	return this->_qString;
}

/*------------------------------------------*\
|                 SETTERS                    |
\*------------------------------------------*/

void HTTPRequest::setHeader(std::string const &key, std::string const &value)
{
	std::map<std::string, std::string>::iterator it = _headers.find(key);
	if (it == _headers.end())
		_headers.insert(std::pair<std::string, std::string>(key, value));
	else
		it->second = value;
}

void HTTPRequest::setBody(std::string const &body)
{
	this->_body = body;
}

void HTTPRequest::setMethod(Method const &method)
{
	this->_method = method;
}

void HTTPRequest::setUri(std::string const &uri)
{
	this->_uri = uri;
}

void HTTPRequest::setVersion(Version const &version)
{
	this->_version = version;
}

void HTTPRequest::setIsCgi(bool const &isCgi)
{
	this->_isCGI = isCgi;
}

