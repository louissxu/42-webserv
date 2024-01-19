#pragma once
#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>

/*
	TODO add later:
	Server: AMAnix\r\n
	Date: Fri, 19 Jan 2024 05:55:55 UTC\r\n\r\n
*/

class HTTPResponse {
	private:
		std::string contentType;
		std::string contentLength;
		std::string connection;
};

#endif