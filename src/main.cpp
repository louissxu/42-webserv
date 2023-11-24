#include <iostream>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

// Ref: https://beej.us/guide/bgnet/html/#structs

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  std::cout << "Hello world" << std::endl;

  int error_return;
  struct address_info;
  error_return = getaddrinfo();
  std::cout << "ai family is: " << address_info.ai_family << std::endl;

  return (0);

  // int sockfd = socket(domain, type, protocol);
  int sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);

  // int setsockopt(int sockfd, int level, int optname, const void* optval, socklen_t optlen);
  // int err_return = setsockopt()

  // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
  // int err_return = bind(sockfd, )



  return (0);
}