#include "HTTPResponse.hpp"

HTTPResponse::HTTPResponse()
{
	buildDefaultResponse();
}

HTTPResponse::HTTPResponse(std::string const &_version, Status const &_status, std::string const &_reason, std::map<std::string, std::string> const &_headers, std::string const &_body)
	: version(_version), status(_status), reason(_reason), headers(_headers), body(_body)
{
}

HTTPResponse::HTTPResponse(HTTPResponse const &src)
{
	*this = src;
}

HTTPResponse &HTTPResponse::operator=(HTTPResponse const &src)
{
	this->status = src.getStatusCode();
	this->reason = src.getReason();
	this->headers = src.getHeaders();
	this->body = src.getBody();
	return *this;
}

std::string const &HTTPResponse::getVersion() const
{
	return this->version;
}

Status const &HTTPResponse::getStatusCode() const
{
	return this->status;
}

std::string HTTPResponse::getStatus() const
{
	switch (status)
	{
	case OK:
		return "OK";
	case CREATED:
		return "CREATED";
	case ACCEPTED:
		return "ACCEPTED";
	case NO_CONTENT:
		return "NO_CONTENT";
	case BAD_REQUEST:
		return "BAD_REQUEST";
	case FORBIDDEN:
		return "FORBIDDEN";
	case NOT_FOUND:
		return "NOT_FOUND";
	case REQUEST_TIMEOUT:
		return "REQUEST_TIMEOUT";
	case INTERNAL_SERVER_ERROR:
		return "INTERNAL_SERVER_ERROR";
	case BAD_GATEWAY:
		return "BAD_GATEWAY";
	case SERVICE_UNAVAILABLE:
		return "SERVICE_UNAVAILABLE";
		// TODO throw error when code is not valid
		// default:
		// 	throw;
		// 	break;
	}
}

void HTTPResponse::setVersion(std::string const &_version)
{
	this->version = _version;
}

void HTTPResponse::setStatus(Status const &_status)
{
	this->status = _status;
}

void HTTPResponse::setReason(std::string const &_reason)
{
	this->reason = _reason;
}

void HTTPResponse::addHeader(std::string const &_key, std::string const &_value)
{
	// TODO check if key is already in the map, or is the key "Cookie".
	headers.insert(std::pair<std::string, std::string>(_key, _value));
}

void HTTPResponse::setBody(std::string const &_body)
{
	this->body = _body;
}

// Status const &HTTPResponse::getStatus() const
// {
// 	return this->status;
// }

std::string const &HTTPResponse::getReason() const
{
	return this->reason;
}

std::map<std::string, std::string> const &HTTPResponse::getHeaders() const
{
	return this->headers;
}

std::string const &HTTPResponse::getBody() const
{
	return this->body;
}

void HTTPResponse::buildDefaultResponse()
{
	this->version = "HTTP/1.1";
	this->status = OK;
	this->reason = "OK";
	setDefaultBody();
	setDefaultHeaders();
}

void HTTPResponse::setDefaultHeaders()
{
	headers.insert(std::pair<std::string, std::string>("Content-Length", std::to_string(body.size())));
	headers.insert(std::pair<std::string, std::string>("Content-Type", "en-us"));
	headers.insert(std::pair<std::string, std::string>("Connection", "Keep-Alive"));
	headers.insert(std::pair<std::string, std::string>("Server", "mehdi's_webserv"));
}

void HTTPResponse::setDefaultBody()
{
	body = "<html><head><title>Test Title</title></head><body>Hello World!<br /></body></html>";
}
