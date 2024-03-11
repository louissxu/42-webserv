#include <iostream>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include "HTTPRequest.hpp"
#include "Server.hpp"
#include "QueueManager.hpp"

#include <vector>

// Ref: https://beej.us/guide/bgnet/html/#structs

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  std::cout << "Starting" << std::endl;

  std::string port1 = "2345";

  Server server1 = Server(port1);

  std::cout << "adding socket manager" << std::endl;
  QueueManager qm;

  qm.registerEvents(server1.getEventsToRegister());

  qm.runEventLoop();

  std::cout << "all done!" << std::endl;

  return (0);

  // a = Server(port1);
  // Server b(port1);

  // std::vector<Server> vec_of_servers;
  // vec_of_servers.push_back(a);
  // vec_of_servers.push_back(c);
  // vec_of_servers.push_back(d);

  return(0);

  std::cout << "Made 3 (2?) servers" << std::endl;











  int error_return;
  struct addrinfo hints;
  struct addrinfo *servinfo;

  memset(&hints, 0, sizeof hints);

  hints.ai_family = AF_INET; // Set to IPv4
  hints.ai_socktype = SOCK_STREAM; // Set to TCP
  // hints.ai_flags = AI_PASSIVE; // fill in my IP for me

  error_return = getaddrinfo(NULL, "1234", &hints, &servinfo);
  // check return value

  std::cout << "ai family is: " << servinfo->ai_family << std::endl;

  std::cout << "getaddrinfo return value is: " << error_return << std::endl;

  std::cout << "Printing addrinfo" << std::endl;
  std::cout << "ai_flags " << servinfo->ai_flags << std::endl;
  std::cout << "ai_family " << servinfo->ai_family << std::endl;
  std::cout << "ai_socktype " << servinfo->ai_socktype << std::endl;
  std::cout << "ai_protocol " << servinfo->ai_protocol << std::endl;
  std::cout << "ai_addrlen " << servinfo->ai_addrlen << std::endl;

  std::cout << "ai_addr->sa_family " << servinfo->ai_addr->sa_family << std::endl;
  std::cout << "ai_addr->sa_data " << servinfo->ai_addr->sa_data << std::endl;

  struct sockaddr_in *sai;
  sai = reinterpret_cast<struct sockaddr_in *>(servinfo->ai_addr);

  std::cout << "ai_addr->sin_family " << sai->sin_family << std::endl;
  std::cout << "ai_addr->sin_port " << ntohs(sai->sin_port) << std::endl;
  std::cout << "ai_addr->sin_addr->sin_addr " << sai->sin_addr.s_addr << std::endl;

  char ipstr[INET6_ADDRSTRLEN];
  inet_ntop(servinfo->ai_family, &sai->sin_addr, ipstr, sizeof ipstr);
  std::cout << "ai_addr->sin_addr->sin_addr ntop (printable) " << ipstr << std::endl;


  std::cout << "ai_addr->sin_zero " << sai->sin_zero << std::endl;


  // std::cout << "ai_canonname " << servinfo->ai_canonname << std::endl;

  std::cout << "end printing stuff" << std::endl;


  // int sockfd = socket(PF_INET, SOCK_STREAM, getprotobyname("TCP")); // hard coded values. Note PF_INET rather than AF_INET. strictly speakign htis is correct (protocol family vs address family; even though the actual values are the same and therefore some people use AF_INET here)
  // int sockfd = socket(PF_INET, SOCK_STREAM, 0); // hard coded values, but allow protocol to be set by given type
  int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol); // use values from getaddrinfo() call
  // check return value > 0 (-1 is error)

  std::cout << "sock fd is: " << sockfd << std::endl;

  int yes = 1;
  setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes);


  error_return = bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
  // check return value (-1 is error)

  error_return = listen(sockfd, 20);
  // check return value (-1 is error)


  struct sockaddr_storage their_addr;
  socklen_t addr_size;
  int newfd = accept(sockfd, (struct sockaddr * )&their_addr, &addr_size);

  char buff[1024];

  int bytes_received = recv(newfd, &buff, 1024, 0);

  std::string input_string = buff;
  HTTPRequest http_request(input_string);
  http_request.print();

  // std::cout << "received data: " << buff << std::endl;
  std::cout << "bytes received: " << bytes_received << std::endl;

  char sent_message[] = "HTTP/1.1 200 OK\n\n<html><body><h1>It works!</h1></body></html>";
  int sent_message_length = strlen(sent_message);
  int bytes_sent = send(newfd, sent_message, sent_message_length, 0);
  std::cout << "message sent" << std::endl;
  std::cout << "sent_message_length: " << sent_message_length << std::endl;
  std::cout << "bytes sent: " << bytes_sent << std::endl;


  freeaddrinfo(servinfo);

  std::cout << "all done" << std::endl;
  error_return = shutdown(newfd, 2);


  return (0);

  // int sockfd = socket(domain, type, protocol);
  // int sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);

  // int setsockopt(int sockfd, int level, int optname, const void* optval, socklen_t optlen);
  // int err_return = setsockopt()

  // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
  // int err_return = bind(sockfd, )



  return (0);
}

// Socket manager
  // has fds
  // when fd is updated
  // create http request
  // do thing, and reply with http response