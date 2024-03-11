#include "Server.hpp"

// Ref: https://stackoverflow.com/questions/41104320/c-copy-constructor-of-object-owning-a-posix-file-descriptor
// Ref: https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
// Ref: https://stackoverflow.com/questions/5481539/what-does-t-double-ampersand-mean-in-c11

// Also
// Ref: https://stackoverflow.com/questions/56369138/moving-an-object-with-a-file-descriptor
// Ref: https://stackoverflow.com/questions/4172722/what-is-the-rule-of-three#:~:text=The%20rule%20of%203%2F5,functions%20when%20creating%20your%20class.

// Ref: Lost of kqueue inspiration from here https://habr.com/en/articles/600123/
// Ref: And here: https://nima101.github.io/kqueue_server

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

  // Ref: Make socket non blocking https://stackoverflow.com/questions/1543466/how-do-i-change-a-tcp-socket-to-be-non-blocking
  error_return = fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK);
  if (error_return != 0) {
    perror("Server: fcntl");
    throw std::runtime_error("Server: fcntl: failed to set socket to non-blocking");
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
  _port = port;
  _ip = ipstr;

  std::cout << "parameterised constructor ran. " << _ip << ":" << _port << " fd: " << _sockfd << std::endl;

  freeaddrinfo(servinfo);
}

static int safe_dup(int fd) {
  if (fd == -1) {
    return -1;
  }

  int copy = dup(fd);
  if (copy < 0) {
    throw std::runtime_error(strerror(errno));
  }
  return copy;
}

Server::Server(const Server& other) {
  _sockfd = safe_dup(other._sockfd);
  _ip = other._ip;
  _port = other._port;
  std::cout << "copy constructor ran. " << _ip << ":" << _port << " fd: " << _sockfd << std::endl;
}

Server& Server::operator=(const Server& other) {
  int new_fd = safe_dup(other._sockfd);
  if (_sockfd != -1) {
    close(_sockfd);
  }
  _sockfd = new_fd;
  _ip = other._ip;
  _port = other._port;
  std::cout << "assignment constructor ran. " << _ip << ":" << _port << " fd: " << _sockfd << std::endl;
  return *this;
}

Server::~Server() {
  // TODO. Do teardown stuff
  std::cout << "destructor ran. " << _ip << ":" << _port << " fd: " << _sockfd << std::endl;
  if (_sockfd != -1) {
    shutdown(_sockfd, 2); // WHICH ONE?!
    // close(_sockfd);
  }
}

int Server::getSockFd() {
  return _sockfd;
}

Server::Server() {
  _sockfd = -1;
  _ip = "";
  _port = "";
  std::cout << "default constructor ran. " << _ip << ":" << _port << " fd: " << _sockfd << std::endl;
}

void Server::acceptNewConnection() {
  Connection newConnection = Connection(_sockfd);
  _connections.push_back(newConnection);
  std::cout << "connection accepted" << std::endl;
  std::cout << "the newConnection's sockFD was " << newConnection.getConnectionFd() << std::endl;
  std::cout << "the connection in the vector's Fd is: " << _connections.back().getConnectionFd() << std::endl;
}

std::vector<Connection>& Server::getConnections() {
  return _connections;
}

// void Server::handleReadEvent() {
//   std::cout << "handling read event" << std::endl;
// }

std::vector<struct kevent> Server::getEventsToRegister() {
  struct kevent readEvent;
  struct kevent writeEvent;

  //TODO DO I need this EV_CLEAR?
  EV_SET(&readEvent, _sockfd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, this);
  EV_SET(&writeEvent, _sockfd, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, this);
  
  std::vector<struct kevent> events;
  events.push_back(readEvent);
  events.push_back(writeEvent);

  return events;
}

void Server::handleEvent() {
  std::cout << "I have handled the event" << std::endl;
  std::cout << "The socket is  " << _sockfd << std::endl;
  acceptNewConnection();
}