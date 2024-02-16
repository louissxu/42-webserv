#pragma once

#include <poll.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/time.h>

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

#include <map>

#include "Server.hpp"
#include "Client.hpp"

class Server;

#define MAX_EVENTS 200 // random value
#define BUFFER_SIZE 3000
class ServerManager
{
public:
  ServerManager();
  ~ServerManager();

  void addServer(const Server &server);
  // void addConnection(Connection& connection);

  void runKQ();
  void createQ();


  void acceptClient(int indexListenSocket);
  Client *getClient(int fd);

  void updateEvent(int ident, short filter, u_short flags, u_int fflags, int data, void *udata);
  // void acceptNewConnections(int nev);
  void processConnectionIO(int nev);
  bool isListeningSocket(int socket_fd);
  void add_cgi_IO_to_ev_set();

private:
  std::vector<Server> _servers;
  std::map<int, Client *> _clients;

  int kq;
  bool accepting;
  struct kevent *ev_set;
  struct kevent ev_list[MAX_EVENTS];
  int ev_set_count;

  ServerManager(ServerManager &other);
  ServerManager &operator=(ServerManager &other);
};