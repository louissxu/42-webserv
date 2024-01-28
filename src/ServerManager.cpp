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

// ConfigParser ServerManager::getParser()
// {
//   return parser;
// }



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
            //   send_file(ev_list[i].ident, "files/menu.html", "text/html");
            //   first = 0;
            // }
            // else
            // {
            //   send_file(ev_list[i].ident, "files/styles.css", "text/css");
            //   first = 1;
            // }
            delete messagePtr;
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
//               send_file(ev_list[i].ident, "files/menu.html", "text/html");
//               first = 0;
//             }
//             else
//             {
//               send_file(ev_list[i].ident, "files/styles.css", "text/css");
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



//  void ServerManager::setStateFromParser(ConfigParser *src)
//  {
//   std::cout << "Server: setStateFromParser called!" << std::endl;
//   //src->printContexts();
//   src->printDirectives();
//   // size_t i = 0;
//   // size_t j; // = 0;


//     //src->printDirectives();
//     // for(std::vector< ConfigParser >::iterator it = src.get_contexts().begin(); it != src.get_contexts().end(); ++it)
//     // {
//     //     j = 0;
//     //     while (j < src.get_contextLvl())
//     //     {
//     //         std::cout <<"\t";
//     //         j++;
//     //     }
//     //     std::cout << "context["<<i<<"]: name : <" << (*it).getName() << ">" << std::endl;
//     //     //(*it).printDirectives();
//     //     (*it).printContexts();
//     //     i++;
//     // }
//  }



bool ServerManager::isValidDirectiveName(const std::string &src) {
    static const std::string validDirectiveNames[] = {
        "server_name",
        "host",
        "root",
        "index",
        "error_page",
        "allow_methods",
        "autoindex",
        "return",
        "cgi_path",
        "cgi_ext",
    };

    static const size_t numDirectives = sizeof(validDirectiveNames) / sizeof(validDirectiveNames[0]);
    for (size_t i = 0; i < numDirectives; ++i) {
        if (validDirectiveNames[i] == src) {
            return true;
        }
    }
    return false;
}

// Server::Server() {
//   _sockfd = -1;
//   _host = "";
//   _listen = "";
//   _server_name = "";
//   _root = "";
//   _index = "";
//   _error_page = "";
//   std::cout << "default constructor ran. " << _host << ":" << _listen << " fd: " << _sockfd << std::endl;
// }




void ServerManager::p_d(ConfigParser &src)
{
   // std::cout << "ServerManager: printDirectives called." << std::endl;
    size_t i = 0;
    static size_t server_id = 0;
    if (src.getName() == "server")
    {
      std::cout << "Server: " << server_id++ <<" Init: " << std::endl;
      std::string listenValue = src.getListen();
      Server newServ = Server(listenValue);
     // Server newServ = Server(src.getListen());
      std::vector< std::pair < std::string, std::string> > temp = src.get_directives();
      if (temp.empty())
      {
       return;
      }
      for(std::vector< std::pair < std::string, std::string> >::iterator it = temp.begin(); it != temp.end(); ++it)
      {
         std::cout <<"\t " << src.getName() << ": ";
         std::cout << "Directive [" << i << "]: Key: <" << it->first << "> Value: <" << it->second << ">." << std::endl;
         if (isValidDirectiveName(it->first))
         {
            std::cout << "Adding " << it->first << " to Server " << (server_id - 1) << ". " << std::endl;
            newServ.addDirective(it->first, it->second);
         }
          i++;
      }
      this->addServer(newServ);
    }
}

void    ServerManager::p_c(ConfigParser &src)
{
    size_t i = 0;

    std::vector< ConfigParser > temp = src.get_contexts();
    p_d(src);
    if (temp.empty())
    {
      return;
    }
    for(std::vector< ConfigParser >::iterator it = temp.begin(); it != temp.end(); ++it)
    {
        std::cout << src.getName() << ": ";
        std::cout << "context["<<i<<"]: name : <" << (*it).getName() << ">" << std::endl;
        //(*it).printDirectives();
        p_c(*it);
        i++;
    }
}




 void ServerManager::setStateFromParser(ConfigParser &src)
{
    //Out of server directives
    if (src.get_directives().empty()) {
        std::cout << "No directives to print." << std::endl;
    }
    else
    {
      p_d(src);
    }

    //Context check:
    if (src.get_contexts().empty()) {
        std::cout << "No contexts to print." << std::endl;
    }
    else
    {
      std::cout << "Calling server: Print contexts: " << std::endl;
      p_c(src);
    }


}
