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
	std::string version;
	Status status;
	std::string reason;
	std::map<std::string, std::string> headers;
	std::string body;
	bool cgiStatus;
	Server _server;

public:
	HTTPResponse();
	HTTPResponse(std::string const &_version, Status const &_status, std::string const &_reason, std::map<std::string, std::string> const &_headers, std::string const &_body);
	HTTPResponse(HTTPResponse const &src);
	HTTPResponse &operator=(HTTPResponse const &src);

	//HTTPResponse(HTTPRequest const &request);
	HTTPResponse(HTTPRequest const &_req);
	HTTPResponse(HTTPRequest const &_req, Server &_myServer);

	// setters
	void setVersion(std::string const &_version);
	// void setReason(std::string const &_version);
	void setStatus(Status const &_status);
	void setReason(std::string const &_reason);
	void addHeader(std::string const &_key, std::string const &_value);
	void setBody(std::string const &_body);

	// getters
	std::string const &getVersion() const;
	Status const &getStatusCode() const;
	std::string getStatus();
	std::string const &getReason() const;
	std::map<std::string, std::string> const &getHeaders() const;
	std::string const &getBody() const;
	std::string getMethodString(enum e_HRM method) const;
	bool getMethodPermission(enum e_HRM method, Location &Location) const;

	//CGI
	bool const &getCgiStatus() const;
	void setCgiStatus(bool _status);

	void buildDefaultResponse();
	void setDefaultHeaders();
	void setDefaultBody();

	//Method-Route Verification.
	int methodPermittedAtRoute(HTTPRequest const &req);
	//bool methodPermittedAtRoute(HTTPRequest const &req);
	std::string stripFileName(std::string const &reqUri);

	void getErrorResource(int errCode); //retrieves our servers error file first.
	void makeDirectoryPage(const HTTPRequest &req);

	//Incoming change.
	// bool getMethodPermission(enum e_HRM method, Location *myLocation);
	// bool getMethodPermission(enum e_HRM method, Location &myLocation);

private:
	void buildRedirectResponse(std::string const &redirectPath);
	// void isRedirect(std::string const &uri);
	bool getResource(std::string const &path, int const &len);
	//void GETHandler(std::string const &uri);

	//Incoming change
	//void getErrorResource(int errCode); //retrieves our servers error file first.
	// //void GETHandler(std::string const &uri);
	void GETHandler(HTTPRequest const &_req);


	// int const &POSTHandler(HTTPRequest const &request);
	void DELETEHandler(HTTPRequest const &req);
};
