#pragma once
#ifndef SERVER_HPP
# define SERVER_HPP

# include <sys/socket.h>
# include <sys/types.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <stdio.h>
# include <fcntl.h>
# include <string>
# include <iostream>
# include <vector>
# include <map>

# include "Connection.hpp"
# include "Definitions.hpp"
# include "Location.hpp"

class Connection;

# define MAX_CONTENT_LENGTH 30000000

/*
In the configuration file, you should be able to:
• Choose the port and host of each ’server’.

• Setup the server_names or not.

• The first server for a host:port will be the default for this host:port (that means
it will answer to all the requests that don’t belong to an other server).

• Setup default error pages.

• Limit client body size.

• Setup routes with one or multiple of the following rules/configuration (routes wont
be using regexp):

◦ Define a list of accepted HTTP methods for the route.

◦ Define a HTTP redirection.

◦ Define a directory or a file from where the file should be searched (for example,
if url /kapouet is rooted to /tmp/www, url /kapouet/pouic/toto/pouet is
/tmp/www/pouic/toto/pouet).

◦ Turn on or off directory listing.

◦ Set a default file to answer if the request is a directory
*/

/*
 *	One key change is we do not use parameterised constructor of server anymore,
 *	we break it into two parts, initing the server and starting it.
 *	Server(std::string port);
 *
 *	std::string _port; // now called _listen.
 *	std::string _ip; // now called _host.
 *	std::vector<Connection> *_connections; is now: std::vector<Connection>     _connections;
*/

class Server {
  public:
    Server();
    Server(size_t serverId);
    Server(const Server& other);
    Server& operator=(const Server& other);
    ~Server();

    // init_servers()
    int getSockFd();
    void acceptNewConnection();
    std::vector<Connection>& getConnections();
    void setListen(std::string newListen);
    void startServer();
    void initialiseErrorPages();
    void acceptNewLocation(Location newLocation);
    //void addLocation(Location & src);
    //setter for multiple attributes prior to server start.
    void addDirective(const std::string& name, const std::string& value);

  private:
    size_t                      _id;
    std::string                 _listen;
    std::string                 _host;
    std::string                 _server_name;
    std::string                 _root;
    std::string                 _index;
    int                         _sockfd;
    size_t                      _client_max_body_size;
    bool                        _autoindex;
    std::map<int, std::string>  _err_pages;
    std::vector<Connection>     _connections;
    std::vector<Location> 		_locations;
};

#endif
