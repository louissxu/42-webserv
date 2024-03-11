#include "QueueManager.hpp"

QueueManager::QueueManager()
{
  _kq = kqueue();
}

// QueueManager::QueueManager(QueueManager& other) {

// }

// QueueManager& QueueManager::operator=(QueueManager& other) {

// }

QueueManager::~QueueManager()
{
  // TODO close kqueue
  close(_kq);
  std::cout << "QueueManager destructor" << std::endl;
}

void QueueManager::registerEvents(std::vector<struct kevent> events) {
  struct kevent events_array[events.size()];

  for (size_t i = 0; i < events.size(); i++) {
    events_array[i] = events[i];
  }

  int error_return = kevent(_kq, events_array, events.size(), NULL, 0, NULL);
  if (error_return != 0)
  {
    perror("QueueManager: kevent");
    throw std::runtime_error("QueueManager: kevent: failed registering events");
  }
}

void QueueManager::runEventLoop()
{
  struct timespec *timeout = NULL; // wait indefinitely
  struct kevent events[100];       // TODO change this to be not manually set

  std::cout << "starting kqueue loop" << std::endl;
  while (true)
  {
    int n = kevent(_kq, NULL, 0, events, 100, timeout);
    if (n <= 0)
    {
      perror("QueueManager: kevent");
      throw std::runtime_error("QueueManager: kevent: failed in loop");
    }

    for (int i = 0; i < n; i++)
    {
      IEventHandler *obj = static_cast<IEventHandler *>(events[i].udata);
      obj->handleEvent();
    }
  }
}
