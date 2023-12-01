#include "Connection.hpp"

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

Connection::Connection() {
  _connection_fd = -1;
}

Connection::Connection(const Connection& other) {
  _connection_fd = safe_dup(other._connection_fd);
}

Connection& Connection::operator=(const Connection& other) {
  new_fd = safe_dup(other._connection_fd);
  close(_connection_fd);
  _connection_fd = new_fd;
  
}

Connection::~Connection() {
  if (_connection_fd != -1) {
    close(_connection_fd);
  }
}

Connection::Connection(int sockfd) {
  struct sockaddr_storage their_addr;
  socklen_t addr_size;
  int newfd = accept(sockfd, (struct sockaddr * )&their_addr, &addr_size);

  if (newfd < 0) {
    throw std::runtime_error("Connection: accept: failed");
  }

  _connection_fd = newfd;
}

void Connection::receiveData() {
  char buff[1024];
  
  int bytes_received = recv(_connection_fd, &buff, 1024, 0);

  HTTPRequest http_request(input_string);
  _req = http_request;
  _req.print();
}