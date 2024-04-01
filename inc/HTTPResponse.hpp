#pragma once

#include <string>
#include <exception>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <exception>
#include <map>
#include <sys/stat.h>
#include <dirent.h>

#include "HTTPRequest.hpp"
#include "log.hpp"
#include "Server.hpp"
#include "MIME.hpp"
#include "Utils.hpp"
#include "Definitions.hpp"

class Location;
class Server;

enum Status
{
	OK = 200,
	CREATED = 201,
	ACCEPTED = 202,
	NON_AUTHORITATIVE_INFORMATION = 203,
	NO_CONTENT = 204,
	MOVED_PERMANENTLY = 301,
	FOUND = 302,
	BAD_REQUEST = 400,
	UNAUTHORIZED = 401,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	REQUEST_TIMEOUT = 408,
	CONTENT_TOO_LARGE = 413,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	BAD_GATEWAY = 502,
	SERVICE_UNAVAILABLE = 503,
	HTTP_VERSION_NOT_SUPPORTED = 505,
};

class HTTPResponse
{
private:
	std::string _version;
	Status _status;
	std::string _reason;
	std::map<std::string, std::string> _headers;
	std::string _body;
	bool _cgiStatus;
	Server _server;
	std::string _path;

public:
	HTTPResponse();
	HTTPResponse(std::string const &version, Status const &status, std::string const &reason, std::map<std::string, std::string> const &_headers, std::string const &body);
	HTTPResponse(HTTPResponse const &src);
	HTTPResponse(HTTPRequest const &req, Server &myServer);
	HTTPResponse &operator=(HTTPResponse const &src);

	// setters
	void setVersion(std::string const &version);
	void setStatus(Status const &status);
	void setReason(std::string const &reason);
	void addHeader(std::string const &key, std::string const &value);
	void setBody(std::string const &body);

	// getters
	std::string getVersion() const;
	Status getStatusCode() const;
	std::string getStatus();
	std::string getReason() const;
	std::map<std::string, std::string> getHeaders() const;
	std::string getBody() const;
	std::string getMethodString(enum e_HRM method) const;
	bool getMethodPermission(enum e_HRM method, Location &Location) const;
	bool isAutoIndexOn(HTTPRequest const &req);

	//CGI
	bool getCgiStatus() const;
	void setCgiStatus(bool _status);

	void buildDefaultResponse();
	void setDefaultHeaders();
	void setDefaultBody();

	//Method-Route Verification.
	int methodPermittedAtRoute(HTTPRequest const &req);
	std::string stripFileName(std::string const &reqUri);

	void getErrorResource(int errCode); //retrieves our servers error file first.
	void makeDirectoryPage(std::string path);

	//Incoming change.
	std::string createFullPath(HTTPRequest const &_req);
	bool isDirectory(std::string const &uri);

private:
	void buildRedirectResponse(std::string const &redirectPath);
	bool getResource(std::string const &path, int const &len);

	//Incoming change
	void GETHandler(HTTPRequest const &_req);
	void DELETEHandler(HTTPRequest const &req);
};
