#include "Server.hpp"

Server::Server() {

}

Server::Server(Server& other) {
  //TODO
  (void)other;
}

Server::Server(std::string port) {
  struct addrinfo hints;
  memset(&hints, 0, sizeof hints);

  hints.ai_family = AF_INET; // set to IPv4
  hints.ai_socktype = SOCK_STREAM; // set to TCP
  // hints.ai_flags = AI_PASSIVE; // fill in my ip for me

  struct addrinfo *servinfo;
  int error_return;
  error_return = getaddrinfo(NULL, port.c_str(), &hints, &servinfo);

  if (error_return != 0) {
    // gai_strerror(error_return) ?? something with this error value // TODO: May be forbidden function
    throw std::runtime_error("Server: getaddrinfo: failed");
  }

  // debug print the ip and port
  struct sockaddr_in *sai;
  sai = reinterpret_cast<struct sockaddr_in *>(servinfo->ai_addr);
  char ipstr[INET6_ADDRSTRLEN];
  inet_ntop(servinfo->ai_family, &sai->sin_addr, ipstr, sizeof ipstr);
  std::cout << "Server: Starting on " << ipstr << ":" << ntohs(sai->sin_port) << std::endl;

  int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
  if (sockfd < 0) {
    perror("Server: socket");
    throw std::runtime_error("Server: socket: failed");
  }

  // set to allow port reuse? or something
  int yes = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

  error_return = bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
  if (error_return != 0) {
    perror("Server: bind");
    throw std::runtime_error("Server: bind: failed");
  }

  error_return = listen(sockfd, 20); // start listening and set maximum number of pending connections to 20 (make this tuneable?)
  if (error_return != 0) {
    perror("Server: listen");
    throw std::runtime_error("Server: listen: failed");
  }

  _sockfd = sockfd;

  freeaddrinfo(servinfo);
}

Server& Server::operator=(Server& other) {
  (void)other;
  return *this;
}

Server::~Server() {
  // TODO. Do teardown stuff
  shutdown(_sockfd, 2);
}

int Server::getSockFd() {
  return _sockfd;
}