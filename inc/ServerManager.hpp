#pragma once
#ifndef SERVER_MANAGER_HPP
# define SERVER_MANAGER_HPP

#include <poll.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <map>
#include <vector>
#include <set>

#include "Server.hpp"
#include "Client.hpp"
#include "HTTPResponse.hpp"
#include "HTTPRequest.hpp"
#include "log.hpp"

#include "ConfigParser.hpp"
#include "Location.hpp"
#include "Utils.hpp"

class Utils;
class ConfigParser;
class Server;

#define MAX_EVENTS 200 // random value
#define BUFFER_SIZE 3000

class ServerManager
{
	private:
  		
  		int kq;
  		bool accepting;
  		struct kevent *ev_set;
  		struct kevent ev_list[MAX_EVENTS];
  		int ev_set_count;
  		std::string defaultPath;		
		
  		std::map<int, Client *> _clients;
  		std::map<int, Client *> _cgiRead;
  		std::map<int, Client *> _cgiWrite;

		std::vector<Server> _servers;
  		std::vector< std::pair <std::string, std::string> > _directives;
		std::vector < ConfigParser >  _contexts;
	
		void launchCgi(HTTPRequest const &request, Client *cl);
    	
		ServerManager(ServerManager& other);
    	ServerManager& operator=(ServerManager& other);

	public:
		//Parts included in Mehdi's cpp:
  		ServerManager();
  		~ServerManager();

  		void addServer(const Server &server);
  		// void addConnection(Connection& connection);
  		void runKQ();
  		void createQ();

  		void acceptClient(int ListenSocket);
  		Client *getClient(int fd);
  		Client *getCgiClient(int fd, bool &isRead, bool &isWrite);
  		Client *getCgiRead(int fd);
  		int getCgiReadFd(Client *cl);
  		Client *getCgiWrite(int fd);
  		int handleReadEvent(Client *cl, int dataLen);
  		bool handleWriteEvent(Client *cl, int dataLen);

  		void CgiReadHandler(Client *cl, struct kevent ev_list);
  		bool CgiWriteHandler(Client *cl, struct kevent ev_list);
  		void handleEOF(Client *cl, int fd, bool &isRead, bool &isWrite);

		bool isValidDirectiveName(const std::string &src);
		void ns_addDirectives(ConfigParser &src);
		void p_c(ConfigParser &src);
		void setStateFromParser(ConfigParser &src);

  		// void processRequest(Client *cl, HTTPRequest request);
  		HTTPRequest *parseRequest(Client *cl, std::string const &message);

  		std::string getFileContents(std::string uri);
  		void updateEvent(int ident, short filter, u_short flags, u_int fflags, int data, void *udata);
  		void closeConnection(Client *cl);
  		bool isListeningSocket(int socket_fd);


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

};
#endif
