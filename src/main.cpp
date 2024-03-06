#include "Server.hpp"
#include "ServerManager.hpp"
// #include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

// Ref: https://beej.us/guide/bgnet/html/#structs

int main(void)
{
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

  return (0);
}