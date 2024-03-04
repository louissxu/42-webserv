#include "ServerManager.hpp"
#include "Cout.hpp"

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
    // perror("accept");
  }

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

void ServerManager::handleEOF(Client *cl, struct kevent fd, bool &isCgiRead, bool &isCgiWrite)
{
  std::cout << BOLDRED << "closing read " << fd.ident << RESET << std::endl;

  if (isCgiRead)
  {
    // updateEvent(ev_list[i].ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
    // updateEvent(ev_list[i].ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
    updateEvent(cl->getSockFD(), EVFILT_READ, EV_DISABLE, 0, 0, NULL);
    updateEvent(cl->getSockFD(), EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
    deleteCgi(_cgiRead, fd.ident, EVFILT_READ);
    isCgiRead = false;
  }
  else if (isCgiWrite)
  {
    deleteCgi(_cgiRead, fd.ident, EVFILT_READ);
    isCgiWrite = false;
  }
  else
  {
    closeConnection(cl);
  }
}
#include <stack>
void ServerManager::runKQ()
{
  bool isCgiRead = false;
  bool isCgiWrite = false;
  struct timespec timeout = {1, 0};
  std::stack<int> readingFds;

  createQ();
  while (true)
  {
    int nev = kevent(kq, NULL, 0, ev_list, MAX_EVENTS, &timeout);
    if (nev == 0)
    {
      while (!readingFds.empty())
      {
        Client *myClient = getClient(readingFds.top());

        HTTPRequest *_req = parseRequest(myClient, myClient->getRecvMessage());
        if (_req == NULL)
        {
          ERR("Failed to parse request from %d: ", myClient->getSockFD());
          // closeConnection(myClient);
          readingFds.pop();
          continue;
          // return false;
        }
        // if (_req->getUri() == "/favicon.ico")
        //   std::cout << "\n";

        // TODO change to cgi not POST
        if (_req->getMethod() == POST)
        {
          updateEvent(myClient->getSockFD(), EVFILT_READ, EV_DISABLE, 0, 0, NULL);
          updateEvent(myClient->getSockFD(), EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);

          Cgi *cgi = new Cgi();
          cgi->launchCgi(*_req, myClient);

          updateEvent(myClient->pipe_in[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
          updateEvent(myClient->pipe_out[0], EVFILT_READ, EV_ADD | EV_DISABLE, 0, 0, NULL);
          _cgiWrite.insert(std::pair<int, Client *>(myClient->pipe_in[1], myClient));
          _cgiRead.insert(std::pair<int, Client *>(myClient->pipe_out[0], myClient));

          this->_resp = HTTPResponse(*_req);

          Message message(_req->getBody());
          myClient->setMessage(message);
          myClient->setBufferRead(0);
          myClient->resetRecvMessage();
          delete _req; // Clean up dynamically allocated memory
          delete cgi;
          // return 2;
        }
        else
        {

          HTTPResponse _resp(*_req);
          Message message(_resp);
          myClient->setMessage(message);
          myClient->setBufferRead(0);
          myClient->resetRecvMessage();
          delete _req; // Clean up dynamically allocated memory
          updateEvent(readingFds.top(), EVFILT_READ, EV_DISABLE, 0, 0, NULL);
          updateEvent(readingFds.top(), EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
          // return false;
        }

        // cout << stack.top() << " ";

      readingFds.pop();
      }
    }
    else if (nev < 0)
    {
      ERR("Kevent: %s", strerror(errno));
      continue;
    }
    for (int i = 0; i < nev; i++)
    {
      if (isListeningSocket(ev_list[i].ident))
      {
        acceptClient(ev_list[i].ident);
        break;
      }
      Client *myClient = getClient(ev_list[i].ident);
      if (myClient == nullptr)
      {
        myClient = getCgiClient(ev_list[i].ident, isCgiRead, isCgiWrite);
        if (myClient == nullptr)
        {
          ERR("Client: %d does not exist!", (int)ev_list[i].ident);
          continue;
        }
      }
      if (ev_list[i].flags & EV_EOF)
      {
        handleEOF(myClient, ev_list[i], isCgiRead, isCgiWrite);
        continue;
      }
      if (isCgiWrite)
      {
        Cgi cgi;
        areFdsOpen(myClient->pipe_in, myClient->pipe_out);
        if (!cgi.CgiWriteHandler(*this, myClient, ev_list[i]))
        {
          deleteCgi(_cgiWrite, myClient, EVFILT_WRITE);
        }
        isCgiWrite = false;
      }
      else if (isCgiRead)
      {
        Cgi cgi;
        areFdsOpen(myClient->pipe_in, myClient->pipe_out);
        cgi.CgiReadHandler(*this, myClient, ev_list[i]);
        isCgiRead = false;
      }
      else if (ev_list[i].filter == EVFILT_READ)
      {
        readingFds.push(ev_list[i].ident);
        int r = handleReadEvent(myClient, ev_list[i].data);
        if (r == 0 || r == 1)
          continue;
        // if (r == 0)
        // {
        //   updateEvent(ev_list[i].ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
        //   updateEvent(ev_list[i].ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
        // }
        // if (r == 1)
        // {
        //   continue;
        //   // updateEvent(ev_list[i].ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
        //   // updateEvent(ev_list[i].ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
        // }
        else if (r == 2)
        {
          updateEvent(ev_list[i].ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
          updateEvent(ev_list[i].ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
        }
      }
      else if (ev_list[i].filter == EVFILT_WRITE)
      {
        if (!handleWriteEvent(myClient, ev_list[i].data))
        {
          updateEvent(ev_list[i].ident, EVFILT_READ, EV_ENABLE, 0, 0, NULL);
          updateEvent(ev_list[i].ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
        }
      }
    }
  }
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

// *unsure if recv will always read all the avialable data, need to learn.
int ServerManager::handleReadEvent(Client *cl, int dataLen)
{
  (void)dataLen;
  if (cl == NULL)
    return false;

  char ClientMessage[4000];
  int readLen = read(cl->getSockFD(), ClientMessage, 4000);

  // if (readLen == 0)
  // {
  //   closeConnection(cl);
  //   return false;
  // }
  if (readLen < 0)
  {
    ERR("unable to recv from %d", cl->getSockFD());
    closeConnection(cl);
    return false;
  }
  cl->appendRecvMessage(ClientMessage, readLen);
  cl->setBufferRead(readLen);
  ClientMessage[readLen] = '\0';
  DEBUG("Recived from: %d\n%s%s%s", cl->getSockFD(), BLUE, ClientMessage, RESET);

  // if (cl->getBufferRead() == dataLen)
  // {
  //   HTTPRequest *_req = parseRequest(cl, cl->getRecvMessage());
  //   if (_req == NULL)
  //   {
  //     ERR("Failed to parse request from %d: ", cl->getSockFD());
  //     closeConnection(cl);
  //     return false;
  //   }
  //   // if (_req->getUri() == "/favicon.ico")
  //   //   std::cout << "\n";

  //   // TODO change to cgi not POST
  //   if (_req->getMethod() == POST)
  //   {
  //     Cgi *cgi = new Cgi();
  //     cgi->launchCgi(*_req, cl);

  //     updateEvent(cl->pipe_in[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
  //     updateEvent(cl->pipe_out[0], EVFILT_READ, EV_ADD | EV_DISABLE, 0, 0, NULL);
  //     _cgiWrite.insert(std::pair<int, Client *>(cl->pipe_in[1], cl));
  //     _cgiRead.insert(std::pair<int, Client *>(cl->pipe_out[0], cl));

  //     this->_resp = HTTPResponse(*_req);

  //     Message message(_req->getBody());
  //     cl->setMessage(message);
  //     cl->setBufferRead(0);
  //     cl->resetRecvMessage();
  //     delete _req; // Clean up dynamically allocated memory
  //     delete cgi;
  //     return 2;
  //   }

  //   HTTPResponse _resp(*_req);
  //   Message message(_resp);
  //   cl->setMessage(message);
  //   cl->setBufferRead(0);
  //   cl->resetRecvMessage();
  //   delete _req; // Clean up dynamically allocated memory
  //   return false;
  // }

  return 1;
}

/*
send may not send the full response, therefore we have to check
if the actual amount send was equal to the length of the respoinse string.
if not we have to send it in the second try.
*/
bool ServerManager::handleWriteEvent(Client *cl, int dataLen)
{
  // DEBUG("writing response\n");
  (void)dataLen;
  Message message = cl->getMessage();

  int attempSend = message.size();
  if (message.getBufferSent() == attempSend)
  {
    // closeConnection(cl);
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
  if (message.empty())
    return NULL;
  (void)cl;
  std::string key, value; // header, value
  std::string method;
  std::string version;
  std::string uri;
  std::map<std::string, std::string> headers;
  std::string body;

  std::stringstream ss(message);
  std::string line;

  std::getline(ss, line, '\n');
  std::stringstream line_stream(line);
  std::getline(line_stream, method, ' ');
  std::getline(line_stream, uri, ' ');
  std::getline(line_stream, version, '\r');

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

  // Parse body
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
  return (new HTTPRequest(headers, body, meth, uri, HTTP_1_1));
}

HTTPResponse &ServerManager::getResponse()
{
  return this->_resp;
}
