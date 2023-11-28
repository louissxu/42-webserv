#include "SocketManager.hpp"

SocketManager::SocketManager() {
  _pfds = new struct pollfd[10];
  _pfds_count = 0;
  _pfds_array_size = 10;

}

// SocketManager::SocketManager(SocketManager& other) {

// }

// SocketManager& SocketManager::operator=(SocketManager& other) {

// }

SocketManager::~SocketManager() {
  delete[] _pfds;
  // _pfds_count = 0;
  // _pfds_array_size = 0;
}

void SocketManager::extendPfdArray(int amount) {
  struct pollfd *new_pfds = new struct pollfd[_pfds_array_size + amount];
  for (size_t i = 0; i < _pfds_array_size; i++) {
    new_pfds[i] = _pfds[i];
  }
  delete[] _pfds;
  _pfds = new_pfds;
  _pfds_array_size = _pfds_array_size + amount;
}

void SocketManager::addServer(Server& server) {
  if (_pfds_count >= _pfds_array_size) {
    extendPfdArray(10);
  }
  _pfds[_pfds_count].fd = server.getSockFd();
  _pfds[_pfds_count].events = POLLIN;
  _servers[_pfds_count] = server;
  _type[_pfds_count] = 1;
  _pfds_count += 1;
}
