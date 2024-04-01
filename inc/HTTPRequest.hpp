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
	PATCH,
	NOTFOUND
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
	std::map<std::string, std::string> _headers;
	std::string _body;
	Method _method;
	std::string _uri;
	Version _version;
	std::string _qString;
	bool _isCGI;

public:
	HTTPRequest(std::map<std::string, std::string> const &headers,
				std::string const &body,
				Method const &method,
				std::string const &uri,
				Version const &version,
				std::string const &qString,
				bool isCGI);
	HTTPRequest();
	~HTTPRequest();

	// setters
	void setHeader(std::string const &key, std::string const &value);
	void setBody(std::string const &body);
	void setMethod(Method const &method);
	void setUri(std::string const &uri);
	void setVersion(Version const &version);
	void setIsCgi(bool const &isCgi);

	// getters
	std::map<std::string, std::string> getHeaders() const;
	std::string getHeader(std::string const &key) const;
	std::string getBody() const;
	std::string getUri() const;
	std::string getQString() const;
	Method getMethod() const;
	std::string getMethodString() const;
	Version getVersion() const;
	bool getCGIStatus() const;
};