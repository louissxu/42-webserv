#include "Webserver.hpp"

int main(int argc, char **argv) {
  ServerManager sm;
  try
  {
    
    ConfigParser parser;

    if (argc == 1)
    { 
      std::string defaultConfigParser = "config/default.config";
      DEBUG("\t\tDefault ConfigParser file used: see %s.", defaultConfigParser.c_str());
      parser.setContent(defaultConfigParser);
    }
    else
    {
      parser.setContent(argv[1]);
    }
    parser.setStateFromContent(0, false);
    sm.setStateFromParser(parser);
    if (sm.getMinimimumRunState() == false)
    {
      WARN("Failed to initialise server: minimum requirements not met.");
      return (1);
    }

    DEBUG("\t\tSite ready...");
    sm.runKQ();
    DEBUG("\t\tAll done!");

  }
  catch (std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    return (1);
  }
  std::cout << "\nDONE WITH THE WEB!\n";
  return(0);
}
