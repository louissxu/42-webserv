#pragma once
#ifndef CONNECTION_HPP
#define CONNECTION_HPP

class Connection {
  public:
    Connection();
    Connection(const Connection& other);
    Connection& operator=(const Connection& other);
    ~Connection();

    Connection(int sockfd);

    void receiveData();

  private:
    int _connection_fd;
    HTTPRequest _req;
};

#endif