#pragma once
#ifndef SOCKETMANAGER_HPP
#define SOCKETMANAGER_HPP

class SocketManager {
  public:
    SocketManager();
    SocketManager(SocketManager& other);
    SocketManager& operator=(SocketManager& other);
    ~SocketManager();

    void addServer(Server server);
    void checkFDs();

  private:
    struct pollfd *pfds
    size_t  pfd_size;
    std::vector<Server&> server_ref;
    std::vector<bool> is_listen;
}

#endif