#include "Server.hpp"

// Ref: https://stackoverflow.com/questions/41104320/c-copy-constructor-of-object-owning-a-posix-file-descriptor
// Ref: https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
// Ref: https://stackoverflow.com/questions/5481539/what-does-t-double-ampersand-mean-in-c11

// Also
// Ref: https://stackoverflow.com/questions/56369138/moving-an-object-with-a-file-descriptor
// Ref: https://stackoverflow.com/questions/4172722/what-is-the-rule-of-three#:~:text=The%20rule%20of%203%2F5,functions%20when%20creating%20your%20class.

static int safe_dup(int fd) {
  if (fd == -1) {
    return -1;
  }
  int copy = dup(fd);
  if (copy < 0) {
    throw std::runtime_error(strerror(errno));
  }
  return copy;
}

/*------------------------------------------*\
|              CONSTRUCTORS                  |
\*------------------------------------------*/

/* The only attributes not handled in constructor:
 * std::vector<Connection>     _connections;
 * std::vector<Location> 		_locations;
 * Do they need to be?
*/
Server::Server() {
  _listen = ""; // Port
  _host = ""; // IP.
  _server_name = "";  //default localhost on most systems.
  _root = "";  //root directory of server.
  _index = "";  
  _sockfd = -1; //server FD.
  _client_max_body_size = MAX_CONTENT_LENGTH;
  _autoindex = false;
  this->initialiseErrorPages();
  std::cout << YELLOW << "Server\t: " << RESET 
  << "default constructor called. " << _host << ":" << _listen << " fd: " << _sockfd << std::endl;
}

Server::Server(size_t serverId) {
  _listen = ""; // Port
  _host = ""; // IP.
  _server_name = "";  //default localhost on most systems.
  _root = "";  //root directory of server.
  _index = "";  
  _sockfd = -1; //server FD.
  _client_max_body_size = MAX_CONTENT_LENGTH;
  _autoindex = false;
  this->initialiseErrorPages();
  _id = serverId;
  std::cout << YELLOW << "Server\t: " << RESET 
  << "id constructor called, id: " << _id << std::endl;
}

Server::Server(const Server& other) {
  _sockfd = safe_dup(other._sockfd);
  _host = other._host;
  _listen = other._listen;
  std::cout << YELLOW << "Server\t: " << RESET 
  <<"Copy constructor called. " << _host << ":" << _listen << " fd: " << _sockfd << std::endl;
}

Server& Server::operator=(const Server& other) {
  int new_fd = safe_dup(other._sockfd);
  if (_sockfd != -1) {
    close(_sockfd);
  }
  _sockfd = new_fd;
  _host = other._host;
  _listen = other._listen;
  std::cout << YELLOW << "Server\t: " << RESET 
  <<"assignment constructor called. " << _host << ":" << _listen<< " fd: " << _sockfd << std::endl;
  return *this;
}

Server::~Server() {
  // TODO. Do teardown stuff
  std::cout << YELLOW << "Server\t: " << RESET
  <<"destructor called. " << _host << ":" << _listen << " fd: " << _sockfd << std::endl;
  if (_sockfd != -1) {
    shutdown(_sockfd, 2); 
  }
}

/*------------------------------------------*\
|              BASIC GETTERS                 |
\*------------------------------------------*/


  std::string Server::getListen(void) const
  {
    return _listen;
  }


/*------------------------------------------*\
|                 SETTERS                    |
\*------------------------------------------*/

void Server::initialiseErrorPages(void)
{
	_err_pages[301] = "";
	_err_pages[302] = "";
	_err_pages[400] = "";
	_err_pages[401] = "";
	_err_pages[402] = "";
	_err_pages[403] = "";
	_err_pages[404] = "";
	_err_pages[405] = "";
	_err_pages[406] = "";
	_err_pages[500] = "";
	_err_pages[501] = "";
	_err_pages[502] = "";
	_err_pages[503] = "";
	_err_pages[505] = "";
	_err_pages[505] = "";
}

void Server::setListen(std::string newListen)
{
  _listen = newListen;
}






/*------------------------------------------*\
|             OTHER METHODS                  |
\*------------------------------------------*/

void Server::startServer(void) {
  struct addrinfo hints;
  memset(&hints, 0, sizeof hints);

  hints.ai_family = AF_INET;       // set to IPv4
  hints.ai_socktype = SOCK_STREAM; // set to TCP
  // hints.ai_flags = AI_PASSIVE; // fill in my ip for me

  struct addrinfo *servinfo;
  int error_return;
 // error_return = getaddrinfo(NULL, _listen.c_str(), &hints, &servinfo);
  error_return = getaddrinfo(_host.c_str(), _listen.c_str(), &hints, &servinfo);

  if (error_return != 0)
  {
    gai_strerror(error_return);
    throw std::runtime_error("Server: getaddrinfo: failed");
  }

  // debug print the ip and port
  struct sockaddr_in *sai;
  sai = reinterpret_cast<struct sockaddr_in *>(servinfo->ai_addr);
  char ipstr[INET6_ADDRSTRLEN];
  inet_ntop(servinfo->ai_family, &sai->sin_addr, ipstr, sizeof ipstr);
  //std::cout << "Server: Starting on " << ipstr << ":" << ntohs(sai->sin_port) << std::endl;

  int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
  if (sockfd < 0)
  {
    perror("Server: socket");
    throw std::runtime_error("Server\t: socket: failed");
  }

  // set to allow port reuse? or something
  int yes = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

  // make the socket non-blocking
  int flags;
  // Get the current flags
  if ((flags = fcntl(sockfd, F_GETFD, 0)) == -1)
    perror("fcntl F_GETFL");
  // Set the O_NONBLOCK flag
  if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
    perror("fcntl F_SETFL O_NONBLOCK");

  std::cout << "DBG: sockfd: " << sockfd << std::endl;
  std::cout << "servinfo aiaddr: " << servinfo->ai_addr << std::endl;
  std::cout << "servinfo ai_addrlen: " << servinfo->ai_addrlen << std::endl;

  error_return = bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
  if (error_return != 0)
  {
    perror("Server: bind");
    throw std::runtime_error("Server: bind: failed");
  }

  error_return = listen(sockfd, 20); // start listening and set maximum number of pending connections to 20 (make this tuneable?)
  if (error_return != 0)
  {
    perror("Server: listen");
    throw std::runtime_error("Server: listen: failed");
  }
  _sockfd = sockfd;
  _host = ipstr;

  std::cout << YELLOW << "Server\t: " << RESET
  <<"starting on " << _host << ":" << _listen << " fd: " << _sockfd << std::endl;
  freeaddrinfo(servinfo);
}

int Server::getSockFd()
{
  return _sockfd;
}

void Server::acceptNewConnection()
{
  Connection newConnection = Connection(_sockfd);
  _connections.push_back(newConnection);
  std::cout << "connection accepted" << std::endl;
}

void Server::acceptNewLocation(Location newLocation) {
  _locations.push_back(newLocation);
}

std::vector<Connection>& Server::getConnections() {
  return _connections;
}

void Server::addDirective(const std::string& name, const std::string& value) {
  if (name == "listen") 
  {
    _listen = value;
  } 
  else if (name == "server_name")
  {
    _server_name = value;
  } 
  else if (name == "host")
  {
    _host = value;
  } 
  else if (name == "root")
  {
    _root = value;
  }
}