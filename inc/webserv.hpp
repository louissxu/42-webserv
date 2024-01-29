#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <iostream>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include "HTTPRequest.hpp"
#include "Server.hpp"
#include "ServerManager.hpp"

#include <vector>


#define MAX_CONTENT_LENGTH 30000000


#endif
