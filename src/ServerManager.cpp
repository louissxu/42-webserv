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
  // std::cout << "If you try to connect to 127.0.0.1:2345 while this is running you will see a hello world HTTPResponse" << std::endl;;
  size_t num_events = poll(pfds, pfd_count, 10000);

  std::cout << "number of ready events is: " << num_events << std::endl;

  for (size_t i = 0; i < _servers.size(); i++) {
    if (pfds[i].revents & POLLIN) {
      _servers[i].acceptNewConnection();
    }
  }

  delete[] pfds;
}


void ServerManager::add_cgi_IO_to_ev_set()
{

		// Create pipes for communication
		int pipe_to_cgi[2];
		int pipe_from_cgi[2];

		pipe(pipe_to_cgi);
		pipe(pipe_from_cgi);

		// Fork to create a child process for the CGI script
		pid_t pid = fork();

		if (pid == 0) {
			// Child process (CGI script)

			// Close unused pipe ends
			close(pipe_to_cgi[1]);
			close(pipe_from_cgi[0]);

			// Redirect standard input and output
			dup2(pipe_to_cgi[0], STDIN_FILENO);
			dup2(pipe_from_cgi[1], STDOUT_FILENO);

			// Execute the CGI script
			execl("cgiBin/login.sh", "cgiBin/login.sh", nullptr);

			// If execl fails
			perror("execl");
			exit(EXIT_FAILURE);
		} else if (pid > 0) {
			// Parent process (C++ server)

			// Close unused pipe ends
			close(pipe_to_cgi[0]);
			close(pipe_from_cgi[1]);

			// Write data to the CGI script
			EV_SET(&ev_set[ev_set_count], pipe_to_cgi[1], EVFILT_WRITE, EV_ADD, 0, 0, NULL);
			if (fcntl(pipe_to_cgi[1], F_SETFL, O_NONBLOCK) < 0) {
				std::cout << RED << "fcntl error: closing: " << pipe_to_cgi[1] << std::endl;
				close(pipe_to_cgi[1]);
			}
			ev_set_count++;

			// Read data from the CGI script
			// std::cerr << "data was sent\n";
			EV_SET(&ev_set[ev_set_count], pipe_from_cgi[0], EVFILT_READ, EV_ADD, 0, 0, NULL);
			if (fcntl(pipe_to_cgi[1], F_SETFL, O_NONBLOCK) < 0) {
				std::cout << RED << "fcntl error: closing: " << pipe_to_cgi[1] << std::endl;
				close(pipe_to_cgi[1]);
			}
      ev_set_count++;
		} else {
			// Fork failed
			perror("fork");
			exit(EXIT_FAILURE);
		}
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
              struct sockaddr_in client_address;
              long  client_address_size = sizeof(client_address);
              conn_fd = accept(_servers[j].getSockFd(), (struct sockaddr *)&client_address, (socklen_t*)&client_address_size);
              if (conn_fd < 0) {
                  perror("accept");
                  continue;
              }
              // std::cout << "accepted: " << conn_fd << std::endl;
              // Add the new socket to kqueue
              EV_SET(&ev_set[j], conn_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
              if (fcntl(conn_fd, F_SETFL, O_NONBLOCK) < 0) {
                std::cout << RED << "fcntl error: closing: " << conn_fd << std::endl;
                close(conn_fd);
              }
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

    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: " + content_type + "\r\n"
        "Connection: keep-alive" + "\r\n"
        "\r\n" + file_contents.str();

    send(client_socket, response.c_str(), response.length(), 0);

    file.close();
}

bool ServerManager::isListeningSocket(int socket_fd) {
    for (size_t i = 0; i < _servers.size(); i++) {
        if (_servers[i].getSockFd() == socket_fd) {
            return true;
        }
    }
    return false;
}

void ServerManager::processConnectionIO( int nev ) {
   char buffer[BUFFER_SIZE];

   for (int i = 0; i < nev; i++) {
        if (ev_list[i].filter == EVFILT_READ && !isListeningSocket(ev_list[i].ident)) {
            // Handling read event
            ssize_t n = read(ev_list[i].ident, buffer, BUFFER_SIZE - 1);
            if (n == 0)
            {
              std::cout << YELLOW << "closing connection with: " << ev_list[i].ident << RESET <<std::endl;
              close(ev_list[i].ident);
            }
            else if (n < 0) {
                // Connection closed
              std::cout << RED << "failed to read: " << ev_list[i].ident << " closing connection." << RESET << std::endl;
              close(ev_list[i].ident);
              exit (EXIT_FAILURE);
            }
            else {
              // std::cout << buffer << std::endl;
              buffer[n] = '\0';
              // std::cout << GREEN << buffer << "\n\n\n"<< std::endl;
              // Cout::output(GREEN, buffer, 1);
              
              HTTPResponse* HTTPResponsePtr = new HTTPRequest(buffer);
              HTTPResponsePtr->generateResponse(ev_list[i].ident);
              delete HTTPResponsePtr;
              close(ev_list[i].ident);
            }
        }
    }
	accepting = true;
}

void ServerManager::runKQ() {
  int numServers = _servers.size();
  ev_set_count = numServers;
  accepting = true; // to check if socket should be added to the queue.
  std::cout << "\n\n\n\n\n";
  createQ();
  while (true) {
    int nev = kevent(kq, ev_set, numServers, ev_list, MAX_EVENTS, nullptr);
    std::cout << "number of events was: " << nev << std::endl;
    if (
      nev < 0) {
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