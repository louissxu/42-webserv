#pragma once
#ifndef ServerManager_HPP
#define ServerManager_HPP

#include <poll.h>

#include "Server.hpp"
#include "Cout.hpp"
#include <vector>

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/event.h>
#include <sys/time.h>
#include <set>

#include "HTTPResponse.hpp"
#include "ConfigParser.hpp"
#include "Location.hpp"
#include "Utils.hpp"

class Utils;
class ConfigParser;

#define MAX_EVENTS 20 // random value
#define BUFFER_SIZE 1024
class ServerManager {
  public:
    ServerManager();
    ~ServerManager();

    void addServer(const Server& server);
    // void addConnection(Connection& connection);

    void runPoll();
    void runKQ();
    void createQ();
    void acceptNewConnections( int nev );
    void processConnectionIO(int nev );
    void setStateFromParser(ConfigParser &src);

    std::string getFirst(const std::string& str);
    std::string getSecond(const std::string& str);
    bool isValidDirectiveName(const std::string &src);
    void ns_addDirectives(ConfigParser &src);
    void p_c(ConfigParser &src);


		class ErrorException : public std::exception
		{
			private:
				std::string _message;
			public:
				ErrorException(std::string message) throw()
				{
					_message = "SERVER CONFIG ERROR: " + message;
				}
				virtual const char* what() const throw()
				{
					return (_message.c_str());
				}
				virtual ~ErrorException() throw() {}
		};

  private:
    std::vector<Server> _servers;
    int kq;
    bool accepting;
    struct kevent *ev_set;
    struct kevent ev_list[MAX_EVENTS];
    // void extendPfdArray(int amount = 10);
    //ConfigParser copied stuff:
    std::vector< std::pair <std::string, std::string> > _directives;
		std::vector < ConfigParser >  _contexts;

    ServerManager(ServerManager& other);
    ServerManager& operator=(ServerManager& other);

};



#endif