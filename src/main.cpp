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

// Ref: https://beej.us/guide/bgnet/html/#structs

int main(void) {
  std::string port1 = "2345";
  std::string port2 = "3456";

  Server a = Server(port1);
  Server b = Server(port2);

  std::cout << "adding socket manager" << std::endl;
  ServerManager sm;

  sm.addServer(a);
  sm.addServer(b);

  sm.runKQ();

  // sm.runPoll();
  std::cout << "all done!" << std::endl;

  return(0);
}