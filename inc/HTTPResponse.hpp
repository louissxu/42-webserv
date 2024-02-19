#pragma once

#include <string>
#include <exception>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sstream>

// #include "HTTPRequest.hpp"

// enum statusCodes
// {
// 	OK,
// 	NOTFOUND,
// 	SERVERERROR
// };

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