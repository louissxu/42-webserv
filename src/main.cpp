#include "Server.hpp"
#include "ServerManager.hpp"
// #include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

int main()
{
	std::cout << "MERGE STEP ONE" << std::endl;
	return (0);
}




// MEHDI'S MAIN:
// Ref: https://beej.us/guide/bgnet/html/#structs
/*
int main(void) {
  std::string port1 = "2345";
  // std::string port2 = "3456";

  Server a = Server(port1);
  // Server b = Server(port2);

  std::cout << "adding socket manager" << std::endl;
  ServerManager sm;

  sm.addServer(a);
  // sm.addServer(b);

  sm.runKQ();

  // sm.runPoll();
  std::cout << "all done!" << std::endl;

  return(0);
}
*/

/*
// ISAAC'S MAIN
#include "Webserver.hpp"

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
    sm.setStateFromParser(parser);	
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
*/
