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
  try
  {
    ServerManager sm;
    ConfigParser parser;

    if (argc == 1)
    { 
      std::string defaultConfigParser = "config/default.conf";
      std::cout << "(Default ConfigParser file used): see <" << defaultConfigParser << ">. " << std::endl;
      parser.setContent(defaultConfigParser);
    }
    else
    {
            parser.setContent(argv[1]);
    }

    parser.setStateFromContent(0, false);
    parser.printContexts();
    sm.setStateFromParser(parser);	

    //testing / temp.
    std::string port1 = "2345";
    std::string port2 = "3456";
    Server a = Server(port1);
    Server b = Server(port2);
    sm.addServer(a);
    sm.addServer(b);
    sm.runKQ();
    std::cout << "all done!" << std::endl;
  }
  catch (std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    return (1);
  }
  return(0);
}