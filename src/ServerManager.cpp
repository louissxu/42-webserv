#include "ServerManager.hpp"

static void areFdsOpen(int pipein[2], int pipeout[2])
{
  if (pipein == NULL || pipeout == NULL)
    return;
  int fd = pipein[0];
  for (int i = 0; i < 2; i++) 
  {
    int flags = fcntl(fd, F_GETFD);
    if (flags != -1)
    {
      DEBUG("%d: is open", fd);
    }
    else
    {
      DEBUG("%d: is not open", fd);
    }
    fd = pipein[1];
  }

  fd = pipeout[0];
  for (int i = 0; i < 2; i++)
  {
    int flags = fcntl(fd, F_GETFD);
    if (flags != -1)
    {
      DEBUG("%d: is open", fd);
    }
    else
    {
      DEBUG("%d: is not open", fd);
    }
    fd = pipeout[1];
  }
}

ServerManager::ServerManager()
{
  defaultPath = "./application";
}

ServerManager::~ServerManager() {}

void ServerManager::addServer(const Server &server)
{
  _servers.push_back(server);
  _portsActive.push_back(server.getListen());
}

Client *ServerManager::getClient(int fd)
{
  std::map<int, Client *>::iterator it = _clients.find(fd);
  if (it != _clients.end())
    return it->second;
  return nullptr;
}

Client *ServerManager::getCgiClient(int fd, bool &isRead, bool &isWrite)
{
  Client *cl;

  cl = getCgiWrite(fd);
  if (cl)
  {
    isWrite = true;
    return cl;
  }
  cl = getCgiRead(fd);
  if (cl)
  {
    isRead = true;
    return cl;
  }
  else
    return nullptr;
}

Client *ServerManager::getCgiRead(int fd)
{
  std::map<int, Client *>::iterator it = _cgiRead.find(fd);
  if (it != _cgiRead.end())
    return it->second;
  return nullptr;
}

void ServerManager::deleteCgi(std::map<int, Client *> &fdmap, Client *cl, short filter)
{
  std::map<int, Client *>::iterator it;
  for (it = fdmap.begin(); it != fdmap.end(); ++it)
  {
    if (it->second == cl)
    {
      updateEvent(it->first, filter, EV_DELETE, 0, 0, NULL);
      close(it->first);
      fdmap.erase(it);
    }
    if (fdmap.empty())
      return;
  }
  DEBUG("\n\n");
  areFdsOpen(cl->pipe_in, cl->pipe_out);
  DEBUG("\n\n");
}

void ServerManager::deleteCgi(std::map<int, Client *> &fdmap, int fd, short filter)
{
  std::map<int, Client *>::iterator it = fdmap.find(fd);
  if (it != fdmap.end())
  {
    updateEvent(it->first, filter, EV_DELETE, 0, 0, NULL);
    close(it->first);
    DEBUG("\n\n");
    areFdsOpen(it->second->pipe_in, it->second->pipe_out);
    DEBUG("\n\n");
    fdmap.erase(it);
  }
}

int ServerManager::getCgiReadFd(Client *cl)
{
  std::map<int, Client *>::iterator it = _cgiRead.begin();
  for (; it != _cgiRead.end(); ++it)
  {
    if (it->second == cl)
      return it->first;
  }
  return -1;
}

Client *ServerManager::getCgiWrite(int fd)
{
  std::map<int, Client *>::iterator it = _cgiWrite.find(fd);
  if (it != _cgiWrite.end())
    return it->second;
  return nullptr;
}

bool ServerManager::isListeningSocket(int socket_fd)
{
  for (size_t i = 0; i < _servers.size(); i++)
    if (_servers[i].getSockFd() == socket_fd)
      return true;
  return false;
}

void ServerManager::updateEvent(int ident, short filter, u_short flags, u_int fflags, int data, void *udata)
{
  struct kevent kev;
  EV_SET(&kev, ident, filter, flags, fflags, data, udata);
  kevent(kq, &kev, 1, NULL, 0, NULL);
}

void ServerManager::closeConnection(Client *cl)
{
  // int i = 0;
  // DEBUG("number of times gone in closeConnection: %d", ++i);
  if (cl == NULL)
    return;
  WARN("Client: %d disconnected!", cl->getSockFD());
  updateEvent(cl->getSockFD(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
  updateEvent(cl->getSockFD(), EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  close(cl->getSockFD());
  std::map<int, Client *>::iterator it = _clients.find(cl->getSockFD());
  if (it != _clients.end())
  {
    close(cl->pipe_in[0]);
    close(cl->pipe_in[1]);
    close(cl->pipe_out[0]);
    close(cl->pipe_out[1]);
    deleteCgi(_cgiRead, cl, EVFILT_READ);
    deleteCgi(_cgiWrite, cl, EVFILT_WRITE);
    delete it->second;
    _clients.erase(it);
  }
}

void ServerManager::handleEOF(Client *cl, int fd, bool &isCgiRead, bool &isCgiWrite)
{
  if (isCgiRead)
  {
    updateEvent(cl->getSockFD(), EVFILT_READ, EV_DISABLE, 0, 0, NULL);
    updateEvent(cl->getSockFD(), EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
    deleteCgi(_cgiRead, fd, EVFILT_READ);
    isCgiRead = false;
  }
  else if (isCgiWrite)
  {
    deleteCgi(_cgiRead, fd, EVFILT_READ);
    isCgiWrite = false;
  }
  else
  {
    closeConnection(cl);
  }
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
    ERR("Kqueue: %s", strerror(errno));
    exit(EXIT_FAILURE);
  }

  for (size_t i = 0; i < _servers.size(); i++)
  {
    EV_SET(&ev_set[i], _servers[i].getSockFd(), EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &ev_set[i], 1, NULL, 0, NULL) == -1)
    {
      ERR("Kqueue: %s", strerror(errno));
      exit(EXIT_FAILURE);
    }
  }
  std::cout << "\n\n";
}

void ServerManager::acceptClient(int ListenSocket)
{
  int clientFD;
  struct sockaddr_in client_address;
  long client_address_size = sizeof(client_address);

  clientFD = accept(ListenSocket, (struct sockaddr *)&client_address, (socklen_t *)&client_address_size);
  if (clientFD < 0)
  {
    ERR("Accpet: %s", strerror(errno));
    return;
  }
  RECORD("LISTENER: %d\t ACCEPTED: %d", ListenSocket, clientFD);
  if (fcntl(clientFD, F_SETFL, O_NONBLOCK) < 0)
  {
    ERR("fcntl error: closing: %d\n errno: %s", clientFD, strerror(errno));
    close(clientFD);
    return;
  }

  Client *cl = new Client(clientFD, ListenSocket, client_address);
  _clients.insert(std::pair<int, Client *>(clientFD, cl));

  updateEvent(clientFD, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
  updateEvent(clientFD, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
}




void ServerManager::runKQ()
{
  createQ();
  while (true)
  {
    int nev = kevent(kq, NULL, 0, ev_list, MAX_EVENTS, nullptr);
    if (nev <= 0)
    {
      ERR("Kevent: %s", strerror(errno));
      continue;
    }
    for (int i = 0; i < nev; i++)
    {
      if (isListeningSocket(ev_list[i].ident))
      {
        acceptClient(ev_list[i].ident);
        continue;
      }
      handleEvent(ev_list[i]);
    }
  }
}

Server* ServerManager::getServerByDescriptor(int sockfd) {
    for (size_t i = 0; i < _servers.size(); ++i) {
        if (_servers[i].getSockFd() == sockfd) {
            return &_servers[i]; // Return the address of the Server instance
        }
    }
    return NULL; // Return nullptr if not found
}

Server* ServerManager::getServerByPort(std::string port) {
    for (size_t i = 0; i < _servers.size(); ++i) {
        if (_servers[i].getListen() == port) {
            return &_servers[i]; // Assuming Server has a getPort() method
        }
    }
    return NULL; // Server not found
}

Server* ServerManager::getServerByRequestHost(HTTPRequest* _req) {
    if (!_req) {
        ERR("HTTP Request is nullptr.");
        return NULL; // Early exit if request is nullptr
    }
    
    std::string hostHeader = _req->getHeader("Host");
    std::string port = "80"; // Default HTTP port. Use 443 for HTTPS by default if you support it.

    // Check if the host header includes a port
    size_t colonPos = hostHeader.find(':');
    if (colonPos != std::string::npos) {
        // Extract the port number
        port = hostHeader.substr(colonPos + 1);
    }

    // Now, find the server by port
    return getServerByPort(port);
}

std::string ServerManager::stripWhiteSpace(std::string src) {
    std::string result;
    for (size_t i = 0; i < src.length(); ++i) {
        char c = src[i];
        //If the character is non whitespace, add it to our output.
        if (c != '\n' && c != '\r' && c != '\t' && c != ' ') {
            result += c; 
        }
    }
    return result;
}



Server &ServerManager::getRelevantServer(HTTPRequest &request, std::vector<Server>& servers) {
    if (servers.empty()) {
        throw ErrorException("No servers are configured.");
    }

    std::string hostHeader = request.getHeader("Host");
    if (hostHeader.empty()) {
        throw ErrorException("No Host header found in the request.");
    }

    std::string requestHost;
    std::string requestPort = "80"; // Default HTTP port.
    size_t colonPos = hostHeader.find(':');
    if (colonPos != std::string::npos) {
        requestHost = stripWhiteSpace(hostHeader.substr(0, colonPos));
        requestPort = stripWhiteSpace(hostHeader.substr(colonPos + 1));
    } else {
        requestHost = hostHeader;
    }

    if (requestHost == "localhost")
    {
      requestHost = LOCALHOST;
    }

    for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
        std::string serverHost = it->getHost();
        std::string serverPort = it->getListen(); 

        // Find a server with matching host and port.
        if (serverHost == requestHost && serverPort == requestPort) {
          //std::cout << "getRelServer: Returning server: " << std::endl;
          //it->printState();
          return (*it);
        }
    }
    throw ErrorException("No matching server found for the given host and port.");
}

void ServerManager::startServer(Server &mServer) {

  //If we haven't set a page to serve by default, serve up index.html.
  if (mServer.getIndex().empty()) {
    mServer.setIndex("index.html");  // Set to default value
  }

  struct addrinfo hints;
  memset(&hints, 0, sizeof hints);

  hints.ai_family = AF_INET;       // set to IPv4
  hints.ai_socktype = SOCK_STREAM; // set to TCP
  // hints.ai_flags = AI_PASSIVE; // fill in my ip for me

  struct addrinfo *servinfo;
  int error_return;
 // error_return = getaddrinfo(NULL, _listen.c_str(), &hints, &servinfo);
  error_return = getaddrinfo(mServer.getHost().c_str(), mServer.getListen().c_str(), &hints, &servinfo);

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
  mServer.setSockFd(sockfd);
  mServer.setHost(ipstr);
 // _sockfd = sockfd;
 // _host = ipstr;

  std::cout << YELLOW << "ServerManager -> Server\t: " << RESET
  <<"starting on " << mServer.getHost() << ":" << mServer.getListen() << " fd: " << mServer.getSockFd() << std::endl;
  freeaddrinfo(servinfo);
}


void ServerManager::handleEvent(struct kevent const &ev)
{
  //Checking which server is handling the event.
  //std::cout << RED << "HANDLE EVENT: " << RESET << std::endl;
  //server->printState();

  bool isCgiRead = false;
  bool isCgiWrite = false;
  Client *cl = getClient(ev.ident);

  if (!(cl = getClient(ev.ident)) && !(cl = getCgiClient(ev.ident, isCgiRead, isCgiWrite)))
  {
    ERR("Client: %d does not exist!", static_cast<int>(ev.ident));
    return;
  }
  if (ev.flags & EV_EOF)
  {
    handleEOF(cl, ev.ident, isCgiRead, isCgiWrite);
    return;
  }
  if (isCgiWrite || isCgiRead)
  {
    Cgi cgi;
    areFdsOpen(cl->pipe_in, cl->pipe_out);

    if (isCgiWrite && !cgi.CgiWriteHandler(*this, cl, ev))
    {
      deleteCgi(_cgiWrite, cl, EVFILT_WRITE);
    }
    else if (isCgiRead)
    {
      cgi.CgiReadHandler(*this, cl, ev);
    }
  }
  else if (ev.filter == EVFILT_READ)
  {
    int r = handleReadEvent(cl, ev);
    if (r == NOMOREDATA)
    {
      HTTPRequest *_req = parseRequest(cl, cl->getRecvMessage());
      if (_req == NULL)
      {
        ERR("Failed to parse request from %d: ", cl->getSockFD());
        closeConnection(cl);
      }

      if (_req->getHeader("Content-Length").empty())
      {
        RECORD("RECIEVED FROM: %lu, METHOD: %s, URI: %s", ev.ident, _req->getMethodString().c_str(), _req->getUri().c_str());
        this->_resp = HTTPResponse(*_req, getRelevantServer(*_req, _servers));
        Message message(_resp);
        cl->setMessage(message);
        cl->setBufferRead(0);
        cl->resetRecvMessage();

        updateEvent(ev.ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
        updateEvent(ev.ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
      }
      else
      {
        if (std::stoi(_req->getHeader("Content-Length")) == static_cast<int>(_req->getBody().size()))
        {
          //*-----Creating our response from a given request.-----
          //this->_resp = HTTPResponse(*_req);
          this->_resp = HTTPResponse(*_req, getRelevantServer(*_req, _servers));
          if (_req->getCGIStatus() == true)
          {
            Cgi *cgi = new Cgi();
            cgi->launchCgi(*_req, cl);
            _cgiWrite.insert(std::pair<int, Client *>(cl->pipe_in[1], cl));
            _cgiRead.insert(std::pair<int, Client *>(cl->pipe_out[0], cl));

            updateEvent(cl->pipe_in[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
            updateEvent(cl->pipe_out[0], EVFILT_READ, EV_ADD | EV_DISABLE, 0, 0, NULL);

            Message message(_req->getBody());
            cl->setMessage(message);
            cl->setBufferRead(0);
            cl->resetRecvMessage();

            updateEvent(ev.ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
            updateEvent(ev.ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
          }
          RECORD("RECIEVED FROM: %lu, METHOD: %s, URI: %s, BODY: %s", ev.ident, _req->getMethodString().c_str(), _req->getUri().c_str(), _req->getBody().c_str());
        }
      }
      delete _req;
    }
  }
  else if (ev.filter == EVFILT_WRITE)
  {
    if (!handleWriteEvent(cl, ev.data))
    {
      updateEvent(ev.ident, EVFILT_READ, EV_ENABLE, 0, 0, NULL);
      updateEvent(ev.ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
    }
  }
}


// *unsure if recv will always read all the avialable data, need to learn.
int ServerManager::handleReadEvent(Client *cl, struct kevent event)
{
  if (cl == NULL)
    return false;

  char ClientMessage[event.data * 2];

  // int readLen = recv(cl->getSockFD(), ClientMessage, dataLen, MSG_DONTWAIT); // MSG_DONTWAIT is similar to O_NONBLOCK

  int readLen = read(cl->getSockFD(), ClientMessage, event.data * 2);

  if (readLen == 0)
  {
    return (NOMOREDATA);
  }
  else if (readLen < 0)
  {
    ERR("unable to recv from %d", cl->getSockFD());
    closeConnection(cl);
    return ERRORDATA;
  }
  cl->appendRecvMessage(ClientMessage, readLen);
  cl->setBufferRead(readLen);
  ClientMessage[readLen] = '\0';
  unsigned int left = (unsigned)event.data;
  if (left <= (unsigned)readLen)
  {
    DEBUG("no more data");
    return NOMOREDATA;
  }
  return true;
}


// send may not send the full response, therefore we have to check
// if the actual amount send was equal to the length of the respoinse string.
// if not we have to send it in the second try.


// TODO: safari has a delay when it sends EOF, we need a mechinism for it.
bool ServerManager::handleWriteEvent(Client *cl, int dataLen)
{
  // DEBUG("writing response\n");
  (void)dataLen;
  Message message = cl->getMessage();

  int attempSend = message.size();
  if (message.getBufferSent() == attempSend)
  {
    std::string status = message.getMessage().substr(0, message.getMessage().find('\n'));
    ;
    RECORD("SENT TO: %d\t STATUS: %s", cl->getSockFD(), status.c_str());
    // closeConnection(cl); might need to close connection here.
    return false;
  }
  int actualSend = send(cl->getSockFD(), message.getMessage().c_str(), attempSend, 0);
  DEBUG("sent to: %d: \n%s%s%s", cl->getSockFD(), GREEN, message.getMessage().c_str(), RESET);
  if (actualSend >= attempSend)
    message.setBufferSent(actualSend);
  cl->setMessage(message);
  return true;
}

std::string ServerManager::getFileContents(std::string uri)
{
  std::string path = defaultPath + uri;
  // std::string contents;
  struct stat s;
  if (stat(path.c_str(), &s) == 0)
  {
    if (s.st_mode & S_IFDIR)
    {
      // it's a directory
    }
    else if (s.st_mode & S_IFREG)
    {
      int len = s.st_size;
      char contents[len];

      std::ifstream file;
      file.open(uri, std::ios::in | std::ios::binary);
      if (!file.is_open())
      {
        ERR("Could not find: %s", uri.c_str());
      }
      file.read(contents, len);
      return (contents);
      // this->status = OK;
    }
    else
    {
      // something else
    }
  }
  else
  {
    // error
  }
  return "";
}

HTTPRequest *ServerManager::parseRequest(Client *cl, std::string const &message)
{
  (void)cl;
  std::string key, value; // header, value
  std::string method;
  std::string version;
  std::string uri;
  std::map<std::string, std::string> headers;
  std::string body;
  bool isCGI = false;

  std::stringstream ss(message);
  std::string line;

  std::getline(ss, line, '\n');
  std::stringstream line_stream(line);
  std::getline(line_stream, method, ' ');
  std::getline(line_stream, uri, ' ');
  std::getline(line_stream, version, '\r');

  if (uri.compare(1, 7, "cgi-bin") == 0)
    isCGI = true;

  // Parse headers
  while (std::getline(ss, line) && !line.empty() && line != "\r")
  {
    size_t colonPos = line.find(':');
    if (colonPos != std::string::npos)
    {
      std::string key = line.substr(0, colonPos);
      std::string value = line.substr(colonPos + 2); // Skip ': ' after colon
      headers[key] = value;
    }
  }
  std::getline(ss, line);
  body = line;
  // Get the rest as the body
  // body = ss.str();
  // // Remove headers from the body
  // body.erase(0, ss.tellg());
  // std::cout << "\n\n\nbody: " << body << "\n\n\n\n";

  Method meth;
  if (method == "GET")
    meth = GET;
  if (method == "POST")
    meth = POST;
  if (method == "DELETE")
    meth = DELETE;
  return (new HTTPRequest(headers, body, meth, uri, HTTP_1_1, isCGI));
}

HTTPResponse &ServerManager::getResponse()
{
  return this->_resp;
}


/*------------------------------------------*\
|          CONFIG READING METHODS            |
\*------------------------------------------*/


/*
 * Check if a given port is already in use.
*/
bool ServerManager::portIsAvailable(std::string portNo)
{
  for(std::vector<std::string>::iterator it = _portsActive.begin(); it != _portsActive.end(); ++it)
  {
    if(portNo == *it)
    {
      return false;
    }
  }
  return true;
}

bool ServerManager::isValidDirectiveName(const std::string &src) {
    static const std::string validDirectiveNames[] = {
        "listen",
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


/*
* @Brief: ns_addDirectives(ConfigParser &src)
* Adds Directives, Contexts, and Locations to a new Server block
* before starting said server.
*/
void ServerManager::ns_addDirectives(ConfigParser &src)
{
   // std::cout << "ServerManager: printDirectives called." << std::endl;
    size_t i = 0;
    static size_t server_id = 0;

    if (src.getName() == "server")
    {
        Server newServ = Server(server_id);
        //adding Directives to the current server block.
        std::cout << GREEN << "SManage\t: " << RESET
        "Server " << server_id++ <<" Initialising: " << std::endl;
        std::vector< std::pair < std::string, std::string> > temp = src.get_directives();
        if (temp.empty())
        {
            std::cout << RED <<": No directives to print." << RESET << std::endl;
            return;
        }
        for(std::vector< std::pair < std::string, std::string> >::iterator it = temp.begin(); it != temp.end(); ++it)
        {
            #ifdef _PRINT_
            std::cout <<"\t " << src.getName() << ": ";
            std::cout << "Directive [" << i << "]: Key: <" << it->first << "> Value: <" << it->second << ">." << std::endl;
            #endif
            if (isValidDirectiveName(it->first))
            {
              #ifdef _PRINT_
                std::cout << "Adding " << it->first << " to Server " << (server_id - 1) << ". " << std::endl;
              #endif
              if (it->first == "listen" && !portIsAvailable(it->second))
              {
                throw ErrorException("Port already in use.");
              }
              newServ.addDirective(it->first, it->second);
            }
            i++;
        }

      //adding contexts/locations to the current Server block
      size_t i = 0;
      std::vector< ConfigParser > src_contexts = src.get_contexts();
      if (temp.empty())
      {
        return;
      }

      for(std::vector< ConfigParser >::iterator it = src_contexts.begin(); it != src_contexts.end(); ++it)
      {
          #ifdef _PRINT_
          std::cout << RED << src.getName() << ": ";
          std::cout << "NSAD: context["<<i<<"]: name : <" << (*it).getName() << ">" << RESET << std::endl;
          #endif
          if (Utils::getFirst(it->getName())=="location")
          {
            #ifdef _PRINT_
            std::cout << RED << "Adding Location: " << Utils::getSecond(it->getName()) << RESET << std::endl;
            #endif
            Location newLocation = Location(Utils::getSecond(it->getName()));
            newLocation.initLocationDirectives(*it);
            //newLocation.printMethodPermissions();
            newServ.acceptNewLocation(newLocation);
          }
          ns_addContexts(*it);
        i++;
      }
      //starting the server now that the required fields have been populated.
      //newServ.startServer();
      startServer(newServ);
      newServ.printState();
      addServer(newServ);
    }
}

void    ServerManager::printAllServers()
{
  std::cout << "Calling print all servers: " << std::endl;
    for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
      it->printState();
    }
}


void    ServerManager::ns_addContexts(ConfigParser &src)
{
    size_t i = 0;

    std::vector< ConfigParser > temp = src.get_contexts();
    ns_addDirectives(src);
    if (temp.empty())
    {
      return;
    }
    for(std::vector< ConfigParser >::iterator it = temp.begin(); it != temp.end(); ++it)
    {
      #ifdef _PRINT_
        std::cout << RED << src.getName() 
        << ": context["<<i<<"]: name : <" << (*it).getName() 
        << ">" << RESET << std::endl;
      #endif
        ns_addContexts(*it);
        i++;
    }
}

void ServerManager::setStateFromParser(ConfigParser &src)
{

    //Out of server directives
    if (src.get_directives().empty()) {
        #ifdef _PRINT_
        std::cout << "No directives to print." << std::endl;
        #endif
    }
    else
    {
      ns_addDirectives(src);
    }
    //Context check:
    if (src.get_contexts().empty()) {
      #ifdef _PRINT_
      std::cout << "No contexts to print." << std::endl;
      #endif
    }
    else
    {
      #ifdef _PRINT_
      std::cout << "Calling server: Print contexts: " << std::endl;
      #endif
      ns_addContexts(src);
    }
}
