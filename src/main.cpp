#include "Webserver.hpp"

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
    std::cout << YELLOW << "PIKA!" << RESET << std::endl;
    sm.setStateFromParser(parser);
    std::cout << BLUE << "SQUIIIRTLE!" << RESET << std::endl;
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

