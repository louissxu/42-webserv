#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h> // for perror
#include <unistd.h> // for dup
#include <fcntl.h>
#include <string>
#include <iostream>
#include <vector>
#include "Connection.hpp"

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
//set a default port?

class Server {
  public:
    Server();
    Server(const Server& other);
    Server& operator=(const Server& other);
    ~Server();
    Server(std::string port);

    // init_servers()

    int getSockFd();
    void acceptNewConnection();
    std::vector<Connection>& getConnections();

  private:
    int _sockfd;
    std::string _listen;
    std::string _host;
    std::string _name;
    std::vector<Connection> _connections;
};


#endif