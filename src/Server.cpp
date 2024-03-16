#include "Server.hpp"

// Ref: https://stackoverflow.com/questions/41104320/c-copy-constructor-of-object-owning-a-posix-file-descriptor
// Ref: https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
// Ref: https://stackoverflow.com/questions/5481539/what-does-t-double-ampersand-mean-in-c11

// Also
// Ref: https://stackoverflow.com/questions/56369138/moving-an-object-with-a-file-descriptor
// Ref: https://stackoverflow.com/questions/4172722/what-is-the-rule-of-three#:~:text=The%20rule%20of%203%2F5,functions%20when%20creating%20your%20class.

// static int safe_dup(int fd) {
//   if (fd == -1) {
//     return -1;
//   }
//   int copy = dup(fd);
//   if (copy < 0) {
//     throw std::runtime_error(strerror(errno));
//   }
//   return copy;
// }

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
  this->initMethodPermissions();
  std::cout << YELLOW << "Server\t: " << RESET << "default constructor called. " << std::endl;
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
  this->initMethodPermissions();
  _id = serverId;
  std::cout << YELLOW << "Server\t: " << RESET 
  << "id constructor called, id: " << _id << std::endl;
}

// Server::Server(const Server& other) {
//   _sockfd = safe_dup(other._sockfd);
//   _host = other._host;
//   _listen = other._listen;
//   std::cout << YELLOW << "Server\t: " << RESET 
//   <<"Copy constructor called. " << _host << ":" << _listen << " fd: " << _sockfd << std::endl;
// }

Server::Server(const Server& other) : 
    _listen(other._listen), 
    _host(other._host), 
    _server_name(other._server_name), 
    _root(other._root), 
    _index(other._index), 
    _sockfd(other._sockfd), 
    _client_max_body_size(other._client_max_body_size), 
    _autoindex(other._autoindex) {
    // Assume _locations is a std::vector<std::string>
    _locations = other._locations;
    _serverPermissions = other._serverPermissions;
      std::cout << YELLOW << "Server\t: " << RESET 
  <<"Copy constructor called. " << _host << ":" << _listen << " fd: " << _sockfd << std::endl;
    // If there were pointers or complex types requiring deep copies, you'd handle them here, like so:
    // _complexType = new ComplexType(*other._complexType);
}


Server& Server::operator=(const Server& other) {
    _listen = other._listen;
    _host = other._host;
    _server_name = other._server_name;
    _root = other._root;
    _index = other._index;
    _sockfd = other._sockfd; 
    _client_max_body_size = other._client_max_body_size;
    _autoindex = other._autoindex;
    _locations = other._locations;
    _serverPermissions = other._serverPermissions;
  std::cout << YELLOW << "Server\t: " << RESET 
  <<"equals override constructor called. " << _host << ":" << _listen<< " fd: " << _sockfd << std::endl;
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

void Server::initMethodPermissions()
{
    _serverPermissions[r_GET] = false;
    _serverPermissions[r_POST] = false;
    _serverPermissions[r_DELETE] = false;
}

/*------------------------------------------*\
|              BASIC GETTERS                 |
\*------------------------------------------*/

    // //Getters:
    // std::string getListen(void) const;
    // std::string getHost(void) const;
    // std::string getIndex(void) const;
    // std::string getSockFd(void) const;

  std::string Server::getListen(void) const
  {
    return _listen;
  }

  std::string Server::getHost(void) const
  {
    return _host;
  }

  std::string Server::getIndex(void) const
  {
    return _index;
  }

  std::string Server::getRoot(void) const
  {
    return _root;
  }

  int Server::getSockFd(void) const
  {
    return _sockfd;
  }

  std::string Server::getErrorPage(const int errorCode) const {
    std::map<int, std::string>::const_iterator it = _err_pages.find(errorCode);
      if (it != _err_pages.end() && it->second != "") {
        return it->second; // Return the associated error message page path.
      } else {
        return "E404.html"; 
      }
  }

std::vector<Location> Server::getLocations(void) const
{
  return _locations;  
}

/*------------------------------------------*\
|                 SETTERS                    |
\*------------------------------------------*/

  void Server::setListen(std::string listen)
  {
    _listen = listen;
  }

  void Server::setHost(std::string host)
  {
    _host = host;
  }

  void Server::setIndex(std::string index)
  {
    _index = index;
  }

  void Server::setSockFd(int sockfd)
  {
    _sockfd = sockfd;
  }

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

/*------------------------------------------*\
|             OTHER METHODS                  |
\*------------------------------------------*/


/*
 * @Brief: Basic printing function to display the attributes of our server class.
*/
void Server::printState(void)
{
  std::cout << BLUE;
  std::cout << "Server state: " << std::endl;
  std::cout << "_listen = " << _listen << std::endl;
  std::cout << "_host = " << _host << std::endl;
  std::cout << "_server_name = " << _server_name << std::endl;
  std::cout << "_root = " << _root << std::endl;
  std::cout << "_index = " << _index << std::endl;
  std::cout << "_sockfd = " << _sockfd << std::endl;
  std::cout << "client_max_body_size = " << _client_max_body_size << std::endl;
  if (_autoindex)
    std::cout << "_autoindex = true" << std::endl;
  else
    std::cout << "_autoindex = false" << std::endl;

  std::cout << "_locations: " << std::endl;
  for (std::vector<Location>::iterator it = _locations.begin(); it != _locations.end(); ++it)
  {
    std::cout << it->getPath() << std::endl;
  }
  std::cout << RESET;
}

void Server::startServer(void) {

  //If we haven't set a page to serve by default, serve up index.html.
  if (_index.empty()) {
    _index = "index.html";  // Set to default value
  }

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

void Server::acceptNewLocation(Location newLocation) {
  _locations.push_back(newLocation);
}

void Server::setErrorPage(const std::string& value) {
    std::istringstream iss(value);
    int errorCode;
    std::string errorPage;

    iss >> errorCode; // Extract the error code as an integer
    iss >> errorPage; // Extract the error page path

    std::cout << YELLOW << "Server: setting error page " << errorCode << " to: "<< errorPage << std::endl;
    // Ensure that the error page path does not have a trailing semicolon
    if (!errorPage.empty() && errorPage[errorPage.length() - 1] == ';') {
        errorPage.erase(errorPage.length() - 1);
    }

    // Store the extracted values into the map
    _err_pages[errorCode] = errorPage;
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
  else if (name == "index")
  {
    _index = value;
  }
  else if (name == "error_page")
  {
    setErrorPage(value);
  }
}

/*
HTTPResponse & Server::makeResponse(HTTPRequest &request)
{
std::cout << "[]"

}*/
