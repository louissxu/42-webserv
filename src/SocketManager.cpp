#include "SocketManager.hpp"

SocketManager::SocketManager() {
  // _pfds = new struct pollfd[10];
  // _pfds_count = 0;
  // _pfds_array_size = 10;

}

// SocketManager::SocketManager(SocketManager& other) {

// }

// SocketManager& SocketManager::operator=(SocketManager& other) {

// }

SocketManager::~SocketManager() {
  // delete[] _pfds;
  // _pfds_count = 0;
  // _pfds_array_size = 0;
  std::cout << "SocketManager destructor" << std::endl;
}

// void SocketManager::extendPfdArray(int amount) {
//   struct pollfd *new_pfds = new struct pollfd[_pfds_array_size + amount];
//   for (size_t i = 0; i < _pfds_array_size; i++) {
//     new_pfds[i] = _pfds[i];
//   }
//   delete[] _pfds;
//   _pfds = new_pfds;
//   _pfds_array_size = _pfds_array_size + amount;
// }

void SocketManager::addServer(const Server& server) {
  _servers.push_back(server);
  
  return;

  // if (_pfds_count >= _pfds_array_size) {
  //   extendPfdArray(10);
  // }
  // _pfds[_pfds_count].fd = server.getSockFd();
  // _pfds[_pfds_count].events = POLLIN;
  // _servers[_pfds_count] = server;
  // _type[_pfds_count] = 1;
  // _pfds_count += 1;
}

void SocketManager::runPoll() {
  size_t pfd_count = _servers.size(); // + connections
  struct pollfd *pfds = new struct pollfd[pfd_count];
  for (size_t i = 0; i < _servers.size(); i++) {
    pfds[i].fd = _servers[i].getSockFd();
    pfds[i].events = POLLIN;
  }
  
  std::cout << "Hit RETURN or wait 2.5 seconds for timeout" << std::endl;
  size_t num_events = poll(pfds, pfd_count, 2500);

  std::cout << "number of ready events is: " << num_events << std::endl;

  for (size_t i = 0; i < _servers.size(); i++) {
    if (pfds[i].revents & POLLIN) {
      _servers[i].acceptNewConnection();
    }
  }

  delete[] pfds;
}