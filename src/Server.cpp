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
  DEBUG("\t\tDefault constructor called.");
  _listen = ""; // Port
  _host = ""; // IP.
  _server_name = "";  //default localhost on most systems.
  _root = "application";  //root directory of server.
  _index = "";
  _sockfd = -1; //server FD.
  _client_max_body_size = MAX_CONTENT_LENGTH;
  _autoindex = false;
  this->initialiseErrorPages();
  this->initMethodPermissions();
}

Server::Server(size_t serverId) {
  DEBUG("\t\tID Param constructor called.");
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
}


Server::Server(const Server& other) :
    _listen(other._listen),
    _host(other._host),
    _server_name(other._server_name),
    _root(other._root),
    _index(other._index),
    _sockfd(other._sockfd),
    _client_max_body_size(other._client_max_body_size),
    _autoindex(other._autoindex)
{
    DEBUG("\t\tCopy constructor called.");
    _locations = other._locations;
    _defaultPermissions = other._defaultPermissions;
}


Server& Server::operator=(const Server& other) {
  DEBUG("\t\tCopy equals override constructor called.");
    _listen = other._listen;
    _host = other._host;
    _server_name = other._server_name;
    _root = other._root;
    _index = other._index;
    _sockfd = other._sockfd;
    _client_max_body_size = other._client_max_body_size;
    _autoindex = other._autoindex;
    _locations = other._locations;
    _defaultPermissions = other._defaultPermissions;
  return *this;
}

Server::~Server() {
  DEBUG("\t\tDestructor called.");
  if (_sockfd != -1) {
    shutdown(_sockfd, 2);
  }
}

void Server::initMethodPermissions()
{
    _defaultPermissions[r_GET] = false;
    _defaultPermissions[r_POST] = false;
    _defaultPermissions[r_DELETE] = false;
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

  std::string Server::getReturnPath(const std::string &reqPath) const
  {
    for (size_t i = 0; i < _locations.size(); ++i)
    {
      if (_locations[i].getPath() == reqPath)
      {
        return _locations[i].getReturn();
      }
    }
    return std::string();
  }

bool Server::getMethodPermission(enum e_HRM method) const {
    std::map<enum e_HRM, bool>::const_iterator it = _defaultPermissions.find(method);
    if (it != _defaultPermissions.end()) {
        return it->second;
    }
    return false; // Default if not set
}

std::map<enum e_HRM, bool> Server::getMethodPermissions(void) const
{
  return _defaultPermissions;
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
  _err_pages[204] = "E204.html";
	_err_pages[301] = "E301.html";
	_err_pages[302] = "E302.html";
	_err_pages[400] = "E400.html";
	_err_pages[401] = "E401.html";
	_err_pages[402] = "E402.html";
	_err_pages[403] = "E403.html";
	_err_pages[404] = "E404.html";
	_err_pages[405] = "E405.html";
	_err_pages[406] = "E406.html";
  _err_pages[413] = "E413.html";
	_err_pages[500] = "E500.html";
	_err_pages[501] = "E501.html";
	_err_pages[502] = "E502.html";
	_err_pages[503] = "E503.html";
	_err_pages[505] = "E505.html";
}

void Server::setMethodPermission(enum e_HRM test, bool permissionState)
{
    _defaultPermissions[test] = permissionState;
}

void Server::setAllowedMethods(const std::string& methods) {
    std::istringstream methodStream(methods);
    std::string method;

    //each usage of methodStream >> method reads the next word
    while(methodStream >> method) {
        if(method == "GET") {
          DEBUG("\t\tSetting server default permission [GET] to true.");
          setMethodPermission(r_GET, true);
        } else if(method == "POST") {
          DEBUG("\t\tSetting server default permission [POST] to true.");
          setMethodPermission(r_POST, true);
        } else if(method == "DELETE") {
          DEBUG("\t\tSetting server default permission [DELETE] to true.");
          setMethodPermission(r_DELETE, true);
        }
    }
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
    throw std::runtime_error("Server\t\t: socket: failed");
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

  //std::cout << "DBG: sockfd: " << sockfd << std::endl;
  //std::cout << "servinfo aiaddr: " << servinfo->ai_addr << std::endl;
  //std::cout << "servinfo ai_addrlen: " << servinfo->ai_addrlen << std::endl;
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

  DEBUG("Starting Server on %s:%s, fd: %d", _host.c_str(), _listen.c_str(), _sockfd);
  //std::cout << YELLOW << "Server\t\t: " << RESET
  //<<"starting on " << _host << ":" << _listen << " fd: " << _sockfd << std::endl;
  freeaddrinfo(servinfo);
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

    DEBUG("\t\tSetting error page %d to: %s", errorCode, errorPage.c_str());
    //std::cout << YELLOW << "Server: setting error page " << errorCode << " to: "<< errorPage << std::endl;
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
  else if (name == "allow_methods")
  {
    //DEBUG("\t\tServer default method set: %s", value.c_str());
    setAllowedMethods(value);
  }
}

bool Server::hasLocation(const std::string &reqPath) {
  std::cout << GREEN;
  DEBUG("Comparing: %s with", reqPath.c_str());
  for (size_t i = 0; i < _locations.size(); ++i) {
    DEBUG(": %s, ", _locations[i].getPath().c_str());
    if (_locations[i].getPath() == reqPath) {
      std::cout << RESET;
      return true; // Found a matching path, so return true.
    }
  }
  std::cout << RESET;
  return false; // No match found after checking all locations.
}

// Location* Server::getLocationByPath(const std::string& reqPath) {
//     for (size_t i = 0; i < _locations.size(); ++i) {
//         if (_locations[i].getPath() == reqPath) {
//             return &_locations[i]; // Return a pointer to the matching Location instance.
//         }
//     }
//     return NULL; // Return NULL if no match is found.
// }

Location& Server::getLocationByPath(const std::string& reqPath) {
    for (size_t i = 0; i < _locations.size(); ++i) {
        if (_locations[i].getPath() == reqPath) {
            return _locations[i]; // Return a reference to the matching Location instance.
        }
    }
    DEBUG("\t\tCould not find reqPath: %s in our Servers Vector of Locations..", reqPath.c_str());
    return Location::NullLocation;
}

