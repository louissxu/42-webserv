#pragma once

#include <string>
// #include <unistd.h>
#include <stdlib.h>

#include "HTTPRequest.hpp"

#define LOG_LEVEL 3
#include "log.hpp"

class Cgi
{
	private:
		std::vector<std::string> _env; 
		char **_argv;
		// char **_env;

	public:
		Cgi();
		~Cgi();

		void setArgv(HTTPRequest const &_req);
		void setEnv(HTTPRequest const &_req);
};