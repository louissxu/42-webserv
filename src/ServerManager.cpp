#include "ServerManager.hpp"

ServerManager::ServerManager() {
  // _pfds = new struct pollfd[10];
  // _pfds_count = 0;
  // _pfds_array_size = 10;

}

// ServerManager::ServerManager(ServerManager& other) {

// }

// ServerManager& ServerManager::operator=(ServerManager& other) {

// }

ServerManager::~ServerManager() {
  // delete[] _pfds;
  // _pfds_count = 0;
  // _pfds_array_size = 0;
  std::cout << "ServerManager destructor" << std::endl;
}

void ServerManager::addServer(const Server& server) {
  _servers.push_back(server);
  return;
}

void ServerManager::runPoll() {
  size_t pfd_count = _servers.size(); // + connections
  struct pollfd *pfds = new struct pollfd[pfd_count];
  for (size_t i = 0; i < _servers.size(); i++) {
    pfds[i].fd = _servers[i].getSockFd();
    pfds[i].events = POLLIN;
  }
  // const char *str = ;
  Cout::output(RED, "hello world", STDOUT_FILENO);
  // std::cout << "Hit RETURN or wait 10 seconds for timeout" << std::endl;
  // std::cout << "If you try to connect to 127.0.0.1:2345 while this is running you will see a hello world message" << std::endl;;
  size_t num_events = poll(pfds, pfd_count, 10000);

  std::cout << "number of ready events is: " << num_events << std::endl;

  for (size_t i = 0; i < _servers.size(); i++) {
    if (pfds[i].revents & POLLIN) {
      _servers[i].acceptNewConnection();
    }
  }

  delete[] pfds;
}

/*
*purpose
add all the listening sockets(servers) to the empty queue (kq);
*/
void ServerManager::createQ() {
  ev_set = new struct kevent[_servers.size()];
  kq = kqueue();
  if (kq == -1) {
      perror("kqueue");
      exit(EXIT_FAILURE);
  }

  for (size_t i = 0; i < _servers.size(); i++)
	{
    EV_SET(&ev_set[i], _servers[i].getSockFd(), EVFILT_READ, EV_ADD, 0, 0, NULL);
		if (kevent(kq, &ev_set[i], 1, NULL, 0, NULL) == -1) {
			perror("kevent");
			exit(EXIT_FAILURE);
		}
	}
}

void ServerManager::acceptNewConnections( int nev ) {
  int conn_fd;

  for (int i = 0; i < nev; i++) {
      for (size_t j = 0; j < _servers.size(); j++) {
          if (static_cast<int>(ev_list[i].ident) == _servers[j].getSockFd()) {
              // Accepting new connections
              conn_fd = accept(_servers[j].getSockFd(), (struct sockaddr *)NULL, NULL);
              if (conn_fd < 0) {
                  perror("accept");
                  continue;
              }
              // std::cout << "accepted: " << conn_fd << std::endl;
              // Add the new socket to kqueue
              EV_SET(&ev_set[j], conn_fd, EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, NULL);
          }
      }
    }
  accepting = false;
}
#include <fstream>

void send_file(int client_socket, const std::string& filename, const std::string& content_type) {
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        perror("Error opening file");
        return;
    }

    std::ostringstream file_contents;
    file_contents << file.rdbuf();

/*
HTTP/1.1 200 OK\r\n
Content-Type: text/html\r\n
Content-Length: 2087\r\n
Connection: keep-alive\r\n
Server: AMAnix\r\n
Date: Fri, 19 Jan 2024 05:55:55 UTC\r\n\r\n
*/

    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: " + content_type + "\r\n"
        "Connection: keep-alive" + "\r\n"
        "\r\n" + file_contents.str();

    send(client_socket, response.c_str(), response.length(), 0);

    file.close();
}


void ServerManager::processConnectionIO( int nev ) {
   char buffer[BUFFER_SIZE];
  
   for (int i = 0; i < nev; i++) {
        if (ev_list[i].filter == EVFILT_READ) {

          
            // Handling read event
            ssize_t n = read(ev_list[i].ident, buffer, BUFFER_SIZE - 1);
            if (n <= 0) {
                // Connection closed
                std::cout << "failed to read: " << ev_list[i].ident << std::endl;
                close(ev_list[i].ident);
                exit (EXIT_FAILURE);
            }
            buffer[n] = '\0';
            Message* messagePtr = new HTTPRequest(buffer);
            messagePtr->generateResponse(ev_list[i].ident);
            // HTTPRequest request(buffer);
            // Message message = request;
            // HTTPResponse(message);
            // message.generateResponse();
            // request.print();

            // std::cout << "read from " << ev_list[i].ident << std::endl;
            // buffer[n] = '\0';
            // // std::cout << "Received: \n" << buffer << "\n\n" << std::endl;

            // static int first = 1;
            // if (first)
            // {
            //   send_file(ev_list[i].ident, "webpages/menu.html", "text/html");
            //   first = 0;
            // }
            // else
            // {
            //   send_file(ev_list[i].ident, "webpages/styles.css", "text/css");
            //   first = 1;
            // }
            close(ev_list[i].ident);
        }
    }
	accepting = true;
}


// void ServerManager::processConnectionIO( int nev ) {
//    char buffer[BUFFER_SIZE];
  
//    for (int i = 0; i < nev; i++) {
//         if (ev_list[i].filter == EVFILT_READ) {
//             // Handling read event
//             ssize_t n = read(ev_list[i].ident, buffer, BUFFER_SIZE - 1);
//             if (n <= 0) {
//                 // Connection closed
//                 std::cout << "failed to read: " << ev_list[i].ident << std::endl;
//                 close(ev_list[i].ident);
//                 exit (EXIT_FAILURE);
//             }

//             HTTPRequest request(buffer);
//             request.print();

//             // std::cout << "read from " << ev_list[i].ident << std::endl;
//             buffer[n] = '\0';
//             // std::cout << "Received: \n" << buffer << "\n\n" << std::endl;

//             static int first = 1;
//             if (first)
//             {
//               send_file(ev_list[i].ident, "webpages/menu.html", "text/html");
//               first = 0;
//             }
//             else
//             {
//               send_file(ev_list[i].ident, "webpages/styles.css", "text/css");
//               first = 1;
//             }
//             close(ev_list[i].ident);
//         }
//     }
// 	accepting = true;
// }

void ServerManager::runKQ() {
  int numServers = _servers.size();
  // struct kevent ev_list[MAX_EVENTS];
  accepting = true; // to check if socket should be added to the queue. 
	struct timespec tmout = { 5, 0 };     /* block for 5 seconds at most */ 
  std::cout << "\n\n\n\n\n";
  createQ();
  while (true) {
    int nev = kevent(kq, ev_set, numServers, ev_list, MAX_EVENTS, &tmout);
    if (nev < 0) {
        perror("kevent");
        exit(EXIT_FAILURE);
    }
    else if (nev == 0)
    {
      Cout::output(RED, "number of events 0", STDOUT_FILENO);
      continue;
    }
    if (accepting)
      acceptNewConnections(nev); // Process new connections
    else {
      // Add the listening sockets back to kqueue
      processConnectionIO( nev );
      for (int j = 0; j < numServers; j++) {
          EV_SET(&ev_set[j], _servers[j].getSockFd(), EVFILT_READ, EV_ADD, 0, 0, NULL);
      }
    }
  }
}