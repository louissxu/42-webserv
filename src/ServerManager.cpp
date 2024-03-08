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
  return;
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

void ServerManager::handleEvent(struct kevent const &ev)
{
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

        this->_resp = HTTPResponse(*_req);
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
          this->_resp = HTTPResponse(*_req);
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
  DEBUG("Recived from: %d\n%s%s%s", cl->getSockFD(), BLUE, ClientMessage, RESET);

  unsigned int left = (unsigned)event.data;
  if (left <= (unsigned)readLen)
  {
    DEBUG("no more data");
    return NOMOREDATA;
  }
  return true;
}

/*
send may not send the full response, therefore we have to check
if the actual amount send was equal to the length of the respoinse string.
if not we have to send it in the second try.
*/

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
|             CONFIG READING                 |
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
      newServ.startServer();
      this->addServer(newServ);
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
