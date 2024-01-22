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

int main(int argc, char **argv) {
  
  // try
  // {
  //   ServerManager sm;
  //   if (argc == 1)
  //   { 
  //     std::string defaultConfigParser = "config/default.conf";
  //     std::cout << "(Default ConfigParser file used): see <" << defaultConfigParser << ">. " << std::endl;
  //     sm.getParser().setContent(defaultConfigParser);
  //   }
  //   else
  //   {
  //           sm.getParser().setContent(argv[1]);
  //   }

  //   sm.getParser().setStateFromContent(0, false);	
  //   sm.getParser().printContexts();
  // }
  // catch (std::exception &e)
  // {
  //   std::cerr << e.what() << std::endl;
  //   return (1);
  // }
  
  // return (0);

  //OG
  (void)argc;
  (void)argv;
  
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