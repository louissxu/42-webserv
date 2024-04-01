#ifndef SERVER_MANAGER_HPP
# define SERVER_MANAGER_HPP

#include <poll.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>

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
#include "cgi.hpp"
#include "log.hpp"

#include "ConfigParser.hpp"
#include "Location.hpp"
#include "Utils.hpp"
#include "Cout.hpp"

class Utils;
class ConfigParser;
class Server;

#define NOMOREDATA 0
#define MOREDATA 1
#define ERRORDATA 2

#define MAX_EVENTS 500 // random value
#define BUFFER_SIZE 3000
#define BUFFERSIZE 10000
#define CLIENT_TIMEOUT 10 //time in seconds before a client times out.
#define LOCALHOST "127.0.0.1"
#define TIMEOUT_SEC 60
class ServerManager
{
	public:
  		ServerManager();
  		~ServerManager();
		
		// getters
		bool	getMinimimumRunState(void) const;
  		Client	*getClient(int fd);
  		Client	*getCgiClient(int fd, bool &isRead, bool &isWrite);
  		Client	*getCgiRead(int fd);
  		int		getCgiReadFd(Client *cl);
  		Client	*getCgiWrite(int fd);

		// kqueue
		void	addServer(const Server &server);
  		void	createQ();
  		void	acceptClient(int ListenSocket);
  		void	runKQ();

  		// io handlers
  		void	handleEvent(struct kevent const &ev);
  		int		handleReadEvent(Client *cl, struct kevent event);
  		void	handleWriteEvent(Client *cl);
  		void	handleEOF(Client *cl, struct kevent const &ev, bool &isRead, bool &isWrite);
		void	updateEvent(int ident, short filter, u_short flags, u_int fflags, int data, void *udata);
  		bool	isListeningSocket(int socket_fd);
  		void	closeConnection(Client *cl);
		void	errorEvent(Status code, HTTPRequest const &r, Server &s, Client *cl);
		
		// parser
  		HTTPRequest parseRequest(Client *cl, std::string const &message);

		//Experimental: for performing httpRequest->httpResponse within the ServerManager.
		Server		&getRelevantServer(HTTPRequest &request, std::vector<Server>& servers);
		std::string	stripWhiteSpace(std::string src);
		void		printAllServers();
		void		startServer(Server &mServer);

		//Configuration handling related:
		bool	portIsAvailable(std::string portNo);
		bool	isValidDirectiveName(const std::string &src);
		void	ns_addDirectives(ConfigParser &src);
		void	ns_addContexts(ConfigParser &src);
		void	setStateFromParser(ConfigParser &src);

		//Exceptions
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
  		std::map<int, Client *> _clients;
  		std::map<int, Client *> _cgiRead;
  		std::map<int, Client *> _cgiWrite;

		std::vector<Server> _servers;
		std::vector <std::string> _portsActive;

  		int _kq;
  		bool _accepting;
  		struct kevent *_ev_set;
		struct kevent _sigEvent;
  		struct kevent _ev_list[MAX_EVENTS];
  		int _ev_set_count;
		bool _serverMinimumRequirements;

  		ServerManager(ServerManager &other);
  		ServerManager &operator=(ServerManager &other);

		void deleteCgi(std::map<int, Client *> &fdmap, Client *cl, short filter);
  		void deleteCgi(std::map<int, Client *> &fdmap, int fd, short filter);
  		void checkCgi(HTTPRequest &req, Server const &s);
};
#endif
