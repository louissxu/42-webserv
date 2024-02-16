#pragma once

#include <poll.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/time.h>

#include "Server.hpp"

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
  void acceptNewConnections(int nev);
  void processConnectionIO(int nev);
  bool isListeningSocket(int socket_fd);
  void add_cgi_IO_to_ev_set();

private:
  std::vector<Server> _servers;
  int kq;
  bool accepting;
  struct kevent *ev_set;
  struct kevent ev_list[MAX_EVENTS];
  int ev_set_count;

  ServerManager(ServerManager &other);
  ServerManager &operator=(ServerManager &other);
};