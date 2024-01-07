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
  int new_fd = safe_dup(other._connection_fd);
  close(_connection_fd);
  _connection_fd = new_fd;
  return *this;
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

  // Currently immediately receive data and send a dummy message back
  // Need to actually parse the request, figure out what it wants
  // and send back the correct respoonse contents (I think?)
  receiveData();
  sendData();
}

void Connection::receiveData() {
  char buff[1024];
  
  int bytes_received = recv(_connection_fd, &buff, 1024, 0);
  (void)bytes_received;
  // Should use bytes_received to tell if i have all the chars and to build my string from my buff

  std::string input_string = buff;
  HTTPRequest http_request(input_string);

  _req = http_request;
  _req.print();
  std::cout << "bytes received: " << bytes_received << std::endl;
}

void Connection::sendData() {
  // Hard coded message to send for testing
  char sent_message[] = "HTTP/1.1 200 OK\n\n<html><body><h1>It works!</h1></body></html>";
  int sent_message_length = strlen(sent_message);
  int bytes_sent = send(_connection_fd, sent_message, sent_message_length, 0);
  std::cout << "message sent" << std::endl;
  std::cout << "sent_message_length: " << sent_message_length << std::endl;
  std::cout << "bytes sent: " << bytes_sent << std::endl;
}