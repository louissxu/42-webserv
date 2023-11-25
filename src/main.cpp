#include <iostream>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>

// Ref: https://beej.us/guide/bgnet/html/#structs

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  std::cout << "Hello world" << std::endl;

  int error_return;
  struct addrinfo hints;
  struct addrinfo *servinfo;

  memset(&hints, 0, sizeof hints);

  hints.ai_family = AF_INET; // Set to IPv4
  hints.ai_socktype = SOCK_STREAM; // Set to TCP
  // hints.ai_flags = AI_PASSIVE; // fill in my IP for me

  error_return = getaddrinfo(NULL, "1234", &hints, &servinfo);

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
  std::cout << "ai_addr->sin_addr->s_addr " << sai->sin_addr.s_addr << std::endl;

  char ipstr[INET6_ADDRSTRLEN];
  inet_ntop(servinfo->ai_family, &sai->sin_addr, ipstr, sizeof ipstr);
  std::cout << "ai_addr->sin_addr->s_addr ntop (printable) " << ipstr << std::endl;


  std::cout << "ai_addr->sin_zero " << sai->sin_zero << std::endl;


  // std::cout << "ai_canonname " << servinfo->ai_canonname << std::endl;

  std::cout << "end printing stuff" << std::endl;


  freeaddrinfo(servinfo);

  return (0);

  // int sockfd = socket(domain, type, protocol);
  // int sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);

  // int setsockopt(int sockfd, int level, int optname, const void* optval, socklen_t optlen);
  // int err_return = setsockopt()

  // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
  // int err_return = bind(sockfd, )



  return (0);
}