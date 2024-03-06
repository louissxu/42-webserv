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

enum Status
{
	OK = 200,
	CREATED = 201,
	ACCEPTED = 202,
	NO_CONTENT = 203,
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	REQUEST_TIMEOUT = 408,
	INTERNAL_SERVER_ERROR = 500,
	BAD_GATEWAY = 502,
	SERVICE_UNAVAILABLE = 503,
};

class HTTPResponse
{
private:
	std::string version;
	Status status;
	std::string reason;
	std::map<std::string, std::string> headers;
	std::string body;

public:
	HTTPResponse();
	HTTPResponse(std::string const &_version, Status const &_status, std::string const &_reason, std::map<std::string, std::string> const &_headers, std::string const &_body);
	HTTPResponse(HTTPResponse const &src);
	HTTPResponse &operator=(HTTPResponse const &src);

	HTTPResponse(HTTPRequest const &request);

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
	std::string getStatus() const;
	std::string const &getReason() const;
	std::map<std::string, std::string> const &getHeaders() const;
	std::string const &getBody() const;

	void buildDefaultResponse();
	void setDefaultHeaders();
	void setDefaultBody();

private:
	bool getResourse(std::string const &path, int const &len);
	void getDefaultResourse();

	void GETHandler(std::string const &uri);
	int const &POSTHandler(HTTPRequest const &request);
	void DELETEHandler();
};

// #include "HTTPRequest.hpp"

// class HTTPResponse
// {
// 	private:
// 		std::string response;
// 		std::string body;
// 		statusCodes status;

// 		int	bufferSend;

// 	private:
// 		std::string filePath;
// 		// std::string base;
// 		// int
// 	public:
// 		HTTPResponse();
// 		HTTPResponse(std::string const &uri);

// 		void setFilePath(std::string const &uri);

// 		void init();
// 		// static HTTPResponse serialize(HTTPRequest const &request);
// };