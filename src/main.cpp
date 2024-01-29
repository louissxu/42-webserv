#include "Webserv.hpp"

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

    //parser.printContexts();

        // Add test directives
    //parser.addTestDirective("TestKey1", "TestValue1");
    //parser.addTestDirective("TestKey2", "TestValue2");

    //parser.printContexts();
    
    //parser.printContexts();
    sm.setStateFromParser(parser);	

    //testing / temp.
    // std::string port1 = "2345";
    // std::string port2 = "3456";
    // Server a = Server(port1);
    // Server b = Server(port2);
    // sm.addServer(a);
    // sm.addServer(b);
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