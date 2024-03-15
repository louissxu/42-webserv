#pragma once

#include <string>
// #include <unistd.h>
#include <stdlib.h>

#include "ServerManager.hpp"
#include "HTTPRequest.hpp"
#include "Client.hpp"

// #define LOG_LEVEL 4
#include "log.hpp"

class ServerManager;
// #define BUFFERSIZE 4000
class Cgi
{
private:
	std::vector<std::string> _envVec;
	std::vector<char *> _argv;
	std::vector<char *> _env;
	// char **_argv;
	// char **_env;
	// char **_env;

public:
	Cgi();
	~Cgi();

	void setArgv(HTTPRequest const &req);
	void setEnv(HTTPRequest &req);

	void CgiReadHandler(ServerManager &sm, Client *cl, struct kevent ev_list);
	bool CgiWriteHandler(ServerManager &sm, Client *cl, struct kevent ev_list);

	void launchCgi(HTTPRequest &req, Client *cl);
};