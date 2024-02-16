#include "ServerManager.hpp"
#include "Cout.hpp"

ServerManager::ServerManager() {}

ServerManager::~ServerManager() {}

void ServerManager::addServer(const Server &server)
{
  _servers.push_back(server);
  return;
}

/*
*purpose
add all the listening sockets(servers) to the empty queue (kq);
*/
void ServerManager::createQ()
{
  ev_set = new struct kevent[_servers.size()];
  kq = kqueue();
  if (kq == -1)
  {
    perror("kqueue");
    exit(EXIT_FAILURE);
  }

  for (size_t i = 0; i < _servers.size(); i++)
  {
    EV_SET(&ev_set[i], _servers[i].getSockFd(), EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &ev_set[i], 1, NULL, 0, NULL) == -1)
    {
      perror("kevent");
      exit(EXIT_FAILURE);
    }
  }
}

void ServerManager::acceptClient(int indexListenSocket)
{
  int clientFD;
  struct sockaddr_in client_address;
  long client_address_size = sizeof(client_address);
  clientFD = accept(_servers[indexListenSocket].getSockFd(), (struct sockaddr *)&client_address, (socklen_t *)&client_address_size);
  if (clientFD < 0)
  {
    perror("accept");
  }
  // std::cout << "accepted: " << conn_fd << std::endl;
  // Add the new socket to kqueue
  updateEvent(clientFD, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
  updateEvent(clientFD, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
  // EV_SET(&ev_set[indexListenSocket], clientFD, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
  // EV_SET(&ev_set[indexListenSocket], clientFD, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
  if (fcntl(clientFD, F_SETFL, O_NONBLOCK) < 0)
  {
    std::cout << "fcntl error: closing: " << clientFD << std::endl;
    close(clientFD);
    return ;
  }
  Client *cl = new Client(clientFD, _servers[indexListenSocket].getSockFd(), client_address);
  _clients.insert(std::pair<int, Client *>(clientFD, cl));
}

Client *ServerManager::getClient(int fd)
{
  std::map<int, Client *>::iterator it = _clients.find(fd);
  if (it != _clients.end())
    return it->second;
  return nullptr;
}

// void ServerManager::acceptNewConnections(int nev)
// {
//   int conn_fd;

//   for (int i = 0; i < nev; i++)
//   {
//     for (size_t j = 0; j < _servers.size(); j++)
//     {
//       if (static_cast<int>(ev_list[i].ident) == _servers[j].getSockFd())
//       {
//         // Accepting new connections
//         struct sockaddr_in client_address;
//         long client_address_size = sizeof(client_address);
//         conn_fd = accept(_servers[j].getSockFd(), (struct sockaddr *)&client_address, (socklen_t *)&client_address_size);
//         if (conn_fd < 0)
//         {
//           perror("accept");
//           continue;
//         }
//         // std::cout << "accepted: " << conn_fd << std::endl;
//         // Add the new socket to kqueue
//         EV_SET(&ev_set[j], conn_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
//         if (fcntl(conn_fd, F_SETFL, O_NONBLOCK) < 0)
//         {
//           std::cout << RED << "fcntl error: closing: " << conn_fd << std::endl;
//           close(conn_fd);
//         }
//       }
//     }
//   }
//   accepting = false;
// }

#include <fstream>

// void send_file(int client_socket, const std::string &filename, const std::string &content_type)
// {
//   std::ifstream file(filename, std::ios::in | std::ios::binary);
//   if (!file.is_open())
//   {
//     perror("Error opening file");
//     return;
//   }

//   std::ostringstream file_contents;
//   file_contents << file.rdbuf();

//   std::string response =
//       "HTTP/1.1 200 OK\r\n"
//       "Content-Type: " +
//       content_type + "\r\n"
//                      "Connection: keep-alive" +
//       "\r\n"
//       "\r\n" +
//       file_contents.str();

//   send(client_socket, response.c_str(), response.length(), 0);

//   file.close();
// }

bool ServerManager::isListeningSocket(int socket_fd)
{
  for (size_t i = 0; i < _servers.size(); i++)
  {
    if (_servers[i].getSockFd() == socket_fd)
    {
      return true;
    }
  }
  return false;
}

// void ServerManager::processConnectionIO(int nev)
// {
//   char buffer[BUFFER_SIZE];

//   for (int i = 0; i < nev; i++)
//   {
//     if (ev_list[i].filter == EVFILT_READ && !isListeningSocket(ev_list[i].ident))
//     {
//       // Handling read event
//       ssize_t n = read(ev_list[i].ident, buffer, BUFFER_SIZE - 1);
//       if (n == 0)
//       {
//         std::cout << YELLOW << "closing connection with: " << ev_list[i].ident << RESET << std::endl;
//         close(ev_list[i].ident);
//       }
//       else if (n < 0)
//       {
//         // Connection closed
//         std::cout << RED << "failed to read: " << ev_list[i].ident << " closing connection." << RESET << std::endl;
//         close(ev_list[i].ident);
//         exit(EXIT_FAILURE);
//       }
//       else
//       {
//         // std::cout << buffer << std::endl;
//         buffer[n] = '\0';
//         // std::cout << GREEN << buffer << "\n\n\n"<< std::endl;
//         // Cout::output(GREEN, buffer, 1);

//         HTTPResponse *HTTPResponsePtr = new HTTPRequest(buffer);
//         HTTPResponsePtr->generateResponse(ev_list[i].ident);
//         delete HTTPResponsePtr;
//         close(ev_list[i].ident);
//       }
//     }
//   }
//   accepting = true;
// }

void ServerManager::updateEvent(int ident, short filter, u_short flags, u_int fflags, int data, void *udata)
{
  struct kevent kev;
  EV_SET(&kev, ident, filter, flags, fflags, data, udata);
  kevent(kq, &kev, 1, NULL, 0, NULL);
}

void ServerManager::runKQ()
{
  Client *myClient;

  int numServers = _servers.size();
  ev_set_count = numServers;
  accepting = true; // to check if socket should be added to the queue.
  std::cout << "\n\n\n\n\n";
  createQ();
  while (true)
  {
    // int nev = kevent(kq, ev_set, numServers, ev_list, MAX_EVENTS, nullptr);
    int nev = kevent(kq, NULL, 0, ev_list, MAX_EVENTS, nullptr);
    if (nev == 0)
      continue;

    for (int i = 0; i < nev; i++)
    {

      std::cout << "number of events was: " << nev << std::endl;
      for (int j = 0; j < numServers; j++)
      {
        if (ev_list[i].ident == (unsigned int)_servers[j].getSockFd())
          acceptClient(j);
      }
      if (!isListeningSocket(ev_list[i].ident))
        {
          myClient = getClient(ev_list[i].ident);
          if (myClient == nullptr)
          {
            std::cout << ev_list[i].ident << ": you are gay the client does not exist\n";
            continue;
          }
          else
            std::cout << myClient->getSockFD() << " is connected to " << myClient->getSockFDconnectedTo() << "\n";
          if (ev_list[i].filter == EVFILT_READ)
          {
            // send(ev_list[i].filter, "hello", 6, 6);
            char *buff = NULL;
            read(ev_list[i].ident, buff, 1080);
            std::cout << GREEN << "read the httpRequest\n";
            updateEvent(ev_list[i].ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
            updateEvent(ev_list[i].ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
            // EV_SET(&ev_set[0], ev_list[i].ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
            // EV_SET(&ev_set[0], ev_list[i].ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
          }
          else if (ev_list[i].filter == EVFILT_WRITE)
          {
            // write(ev_list[i].ident, "hello", 6);
            // std::ostringstream file_contents;
            // file_contents << file.rdbuf();

            std::string response =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: html\r\n"
                "Connection: keep-alive"
                "\r\n"
                "\r\n"
                "helloWorld";
                // file_contents.str();

            send(ev_list[i].ident, response.c_str(), response.length(), 0);
            updateEvent(ev_list[i].ident, EVFILT_READ, EV_ENABLE, 0, 0, NULL);
            updateEvent(ev_list[i].ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);

            // close(ev_list[i].ident);
             std::cout << GREEN << "send the httpResponse\n";
          }
          // if (ev_list[i].ident == _clients.)
          // if (ev_list[i].filter == EVFILT_READ)
          // {
          //   // Handling read event
          //   char buffer[BUFFER_SIZE];
          //   ssize_t n = read((int)ev_list[i].ident, buffer, BUFFER_SIZE - 1);
          //   if (n == 0)
          //   {
          //     std::cout << YELLOW << "closing connection with: " << ev_list[i].ident << RESET << std::endl;
          //     close(ev_list[i].ident);
          //   }
          //   else if (n < 0)
          //   {
          //     // Connection closed
          //     std::cout << RED << "failed to read: " << ev_list[i].ident << " closing connection." << RESET << std::endl;
          //     close(ev_list[i].ident);
          //     exit(EXIT_FAILURE);
          //   }
          //   else
          //   {
          //     // std::cout << buffer << std::endl;
          //     buffer[n] = '\0';
          //     // std::cout << GREEN << buffer << "\n\n\n"<< std::endl;
          //     // Cout::output(GREEN, buffer, 1);

          //     HTTPResponse *HTTPResponsePtr = new HTTPRequest(buffer);
          //     HTTPResponsePtr->generateResponse(ev_list[i].ident);
          //     delete HTTPResponsePtr;
          //     close(ev_list[i].ident);
          //   }
          // }
        }
      }
    }
  }
// }