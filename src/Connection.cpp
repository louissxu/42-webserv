#include "Connection.hpp"

static int safe_dup(int fd)
{
  if (fd == -1)
  {
    return -1;
  }

  int copy = dup(fd);
  if (copy < 0)
  {
    throw std::runtime_error(strerror(errno));
  }
  return copy;
}

Connection::Connection():
received_string_("")
{
  connection_fd_ = -1;
}

Connection::Connection(const Connection &other):
received_string_(other.received_string_)
{
  connection_fd_ = safe_dup(other.connection_fd_);
}

Connection &Connection::operator=(const Connection &other)
{
  int new_fd = safe_dup(other.connection_fd_);
  received_string_ = other.received_string_;
  close(connection_fd_);
  connection_fd_ = new_fd;
  return *this;
}

Connection::~Connection()
{
  if (connection_fd_ != -1)
  {
    close(connection_fd_);
  }
}

Connection::Connection(int sockfd):
received_string_("")
{
  struct sockaddr_storage their_addr;
  socklen_t addr_size;
  int newfd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);

  if (newfd < 0)
  {
    throw std::runtime_error("Connection: accept: failed");
  }

  connection_fd_ = newfd;

  // Currently immediately receive data and send a dummy message back
  // Need to actually parse the request, figure out what it wants
  // and send back the correct respoonse contents (I think?)
  // receiveData();
  // sendData();
}

void Connection::receiveData()
{
  char buff[1024];

  int bytes_received = recv(connection_fd_, &buff, 1024, 0);
  (void)bytes_received;
  // Should use bytes_received to tell if i have all the chars and to build my string from my buff

  std::string input_string = buff;
  HTTPRequest http_request(input_string);

  req_ = http_request;
  req_.print();
  std::cout << "bytes received: " << bytes_received << std::endl;
}

void Connection::sendData()
{
  // Hard coded message to send for testing
  char sent_message[] = "HTTP/1.1 200 OK\n\n<html><body><h1>It works!</h1></body></html>";
  int sent_message_length = strlen(sent_message);
  int bytes_sent = send(connection_fd_, sent_message, sent_message_length, 0);
  std::cout << "message sent" << std::endl;
  std::cout << "sent_message_length: " << sent_message_length << std::endl;
  std::cout << "bytes sent: " << bytes_sent << std::endl;
}

int Connection::getConnectionFd()
{
  return connection_fd_;
}

std::vector<struct kevent> Connection::getEventsToRegister()
{
  struct kevent read_event;
  struct kevent write_event;

  // TODO DO I need this EV_CLEAR?
  EV_SET(&read_event, connection_fd_, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, this);
  EV_SET(&write_event, connection_fd_, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, this);

  std::vector<struct kevent> events;
  events.push_back(read_event);
  events.push_back(write_event);

  return events;
}

void Connection::handleEvent(struct kevent event)
{
  std::cout << "Connection event received" << std::endl;
  if (event.filter == EVFILT_READ)
  {
    handleRead();
  }
}

void Connection::handleRead()
{
  std::cout << "connection is reading data" << std::endl;

  while (true) {
    char buff[51];
    memset(buff, '\0', 51);
    int return_value = recv(connection_fd_, &buff, 50, 0);

    if (return_value < 0 && errno == EAGAIN) {
      errno = 0;
      break;
    }

    if (return_value < 0) {
      perror("Connection: recv");
      throw std::runtime_error("Connection: recv: failed");
    }
    incoming_message_stream_ << buff;
    received_string_.append(buff); // for debugging
  }

  int i = 0;
  while (true) {
    char buff[1001]; // TODO Make this handle lines longer than 1000 better
    memset(buff, '\0', 1001);
    incoming_message_stream_.getline(buff, 1000);
    std::cout << "line " << i << ": " << buff << std::endl;
    i++;

    req_.parseLine(buff);

    if (incoming_message_stream_.eof()) {
      break;
    }
  }

  std::cout << "full message is " << received_string_.length() << " long and is: " << received_string_ << std::endl;

  // std::string input_string = buff;
  // HTTPRequest http_request(input_string);

  // req_ = http_request;
  // req_.print();
}