#include "ServerManager.hpp"

ServerManager::ServerManager() {
  // _pfds = new struct pollfd[10];
  // _pfds_count = 0;
  // _pfds_array_size = 10;
  _kq = kqueue();

}

// ServerManager::ServerManager(ServerManager& other) {

// }

// ServerManager& ServerManager::operator=(ServerManager& other) {

// }

ServerManager::~ServerManager() {
  // delete[] _pfds;
  // _pfds_count = 0;
  // _pfds_array_size = 0;
  // TODO close kqueue
  std::cout << "ServerManager destructor" << std::endl;
}

// void ServerManager::extendPfdArray(int amount) {
//   struct pollfd *new_pfds = new struct pollfd[_pfds_array_size + amount];
//   for (size_t i = 0; i < _pfds_array_size; i++) {
//     new_pfds[i] = _pfds[i];
//   }
//   delete[] _pfds;
//   _pfds = new_pfds;
//   _pfds_array_size = _pfds_array_size + amount;
// }

void ServerManager::addServer(Server& server) {
  struct kevent events[2];
  // struct context data_for_kqueue = {};
  // data_for_kqueue.fd = server.getSockFd();
  // data_for_kqueue.handler = &server.handleEvent;

  EV_SET(&events[0], server.getSockFd(), EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, &server);
  EV_SET(&events[1], server.getSockFd(), EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, &server);

  int error_return = kevent(_kq, events, 2, NULL, 0, NULL);
  if (error_return != 0) {
    perror("ServerManager: kevent");
    throw std::runtime_error("ServerManager: kevent: failed adding server");
  }

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

void ServerManager::runKQueueEventLoop() {
  struct timespec *timeout = NULL; // wait indefinitely
  struct kevent events[100]; // TODO change this to be not manually set

  std::cout << "starting kqueue loop" << std::endl;
  while (true) {
    int n = kevent(_kq, NULL, 0, events, 100, timeout);
    if (n <= 0) {
      perror("ServerManager: kevent");
      throw std::runtime_error("ServerManager: kevent: failed in loop");
    }

    for (int i = 0; i < n; i++) {
      IEventHandler *obj = static_cast<IEventHandler *>(events[i].udata);
      obj->handleEvent();
      // events[i].udata->handleEvent()

      // struct context *o = events[i].udata;
      // o->handler(o);
    }
  }
}

void ServerManager::runPoll() {
  size_t pfd_count = _servers.size(); // + connections
  struct pollfd *pfds = new struct pollfd[pfd_count];
  for (size_t i = 0; i < _servers.size(); i++) {
    pfds[i].fd = _servers[i].getSockFd();
    pfds[i].events = POLLIN;
  }
  
  std::cout << "Hit RETURN or wait 10 seconds for timeout" << std::endl;
  std::cout << "If you try to connect to 127.0.0.1:2345 while this is running you will see a hello world message" << std::endl;;
  size_t num_events = poll(pfds, pfd_count, 10000);

  std::cout << "number of ready events is: " << num_events << std::endl;

  for (size_t i = 0; i < _servers.size(); i++) {
    if (pfds[i].revents & POLLIN) {
      _servers[i].acceptNewConnection();
    }
  }

  delete[] pfds;
}