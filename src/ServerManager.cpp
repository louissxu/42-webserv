#include "ServerManager.hpp"
#include "Cout.hpp"

static void areFdsOpen(int pipein[2], int pipeout[2]) {
  if (pipein == NULL || pipeout == NULL)
    return ;
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
    // ERR("Ke")
    ERR("Kqueue: %s", strerror(errno));
    // perror("kqueue");
    exit(EXIT_FAILURE);
  }

  for (size_t i = 0; i < _servers.size(); i++)
  {
    EV_SET(&ev_set[i], _servers[i].getSockFd(), EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &ev_set[i], 1, NULL, 0, NULL) == -1)
    {
      // perror("kevent");
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
    perror("accept");
  }


  if (fcntl(clientFD, F_SETFL, O_NONBLOCK) < 0)
  {
    std::cout << "fcntl error: closing: " << clientFD << std::endl;
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
}

void ServerManager::deleteCgi(std::map<int, Client *> &fdmap, int fd, short filter)
{
  std::map<int, Client *>::iterator it = fdmap.find(fd);
  if (it != fdmap.end())
  {
      updateEvent(it->first, filter, EV_DELETE, 0, 0, NULL);
      close(it->first);
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

// #include <fstream>

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

void ServerManager::handleEOF(Client *cl, int fd, bool &isCgiRead, bool &isCgiWrite)
{
  // std::cout << "\n\nclosing connection with: " << fd << "\n\n";
  if (isCgiRead)
  {
    // WARN("closing read : %d disconnected!", cl->getSockFD());
    updateEvent(cl->getSockFD(), EVFILT_READ, EV_DISABLE, 0, 0, NULL);
    updateEvent(cl->getSockFD(), EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
    deleteCgi(_cgiRead, fd, EVFILT_READ);
    // std::map<int, Client *>::iterator it = _cgiRead.find(fd);
    // if (it != _cgiRead.end())
    // {
    //   updateEvent(it->first, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    //   close(it->first);
    //   _cgiRead.erase(it);
    // }
    isCgiRead = false;
  }
  else if (isCgiWrite)
  {
    deleteCgi(_cgiRead, fd, EVFILT_READ);
    // std::map<int, Client *>::iterator it = _cgiWrite.find(fd);
    // if (it != _cgiWrite.end())
    // {
    //   updateEvent(it->first, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    //   close(it->first);
    //   _cgiWrite.erase(it);
    // }
    isCgiWrite = false;
  }
  else
  {
    closeConnection(cl);
    // std::cout << "\n\nclosing connection with: " << fd << "\n\n";
  }
}

void ServerManager::runKQ()
{
  bool isCgiRead = false;
  bool isCgiWrite = false;

  createQ();
  while (true)
  {
    int nev = kevent(kq, NULL, 0, ev_list, MAX_EVENTS, nullptr);
    if (nev <= 0)
    {
      ERR("Kevent: %s", strerror(errno));
      std::cerr << RED << "kevent error\n"
                << RESET;
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
          std::cout << RED << ev_list[i].ident << ": client does not exist\n"
                    << RESET;
          continue;
        }
      }
      if (ev_list[i].flags & EV_EOF)
      {
        // std::cout << "hello\n";
        // isFdOpen(ev_list[i].ident);
        handleEOF(myClient, ev_list[i].ident, isCgiRead, isCgiWrite);
        continue;
      }
      if (isCgiWrite)
      {
        areFdsOpen(myClient->pipe_in, myClient->pipe_out);
        CgiWriteHandler(myClient, ev_list[i]);
        isCgiWrite = false;
      }
      else if (isCgiRead)
      {
        areFdsOpen(myClient->pipe_in, myClient->pipe_out);
        CgiReadHandler(myClient, ev_list[i]);
        isCgiRead = false;
      }
      else if (ev_list[i].filter == EVFILT_READ)
      {
        int r = handleReadEvent(myClient, ev_list[i].data);
        if (r == 0)
        {
          updateEvent(ev_list[i].ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
          updateEvent(ev_list[i].ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
        }
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

void ServerManager::CgiReadHandler(Client *cl, struct kevent ev_list)
{
  char buffer[BUFFERSIZE * 2];
  memset(buffer, 0, sizeof(buffer));
  int bytesRead = 0;
  static std::string message = "";
  // std::cout << "cgi listenning socket was: " << ev_list.ident << std::endl;
  bytesRead = read(ev_list.ident, buffer, BUFFER_SIZE * 2);
  // std::cerr << "READ: " << MAGENTA << buffer << RESET << "\n";
  DEBUG("cgiReadHandler: Read: %s", buffer);
  if (bytesRead == 0)
  {
    // std::cerr << BOLDWHITE << "bytes READ = 0\n" << RESET;
    DEBUG("cgiReadHandler: Bytes Read = 0");
    updateEvent(ev_list.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    close(cl->pipe_in[0]);
    close(cl->pipe_out[0]);

    wait(NULL);
    HTTPResponse cgiResponse;
    cgiResponse.setBody(message);
    cgiResponse.addHeader("Content-Length", std::to_string(message.size()));
    Message cgiMessage = Message(cgiResponse);
    cl->setMessage(cgiMessage);
    message.clear();
    updateEvent(cl->getSockFD(), EVFILT_READ, EV_DISABLE, 0, 0, NULL);
    updateEvent(cl->getSockFD(), EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
  }
  else if (bytesRead < 0)
  {
    // std::cerr << RED << "ERROR: cgiReadHandler: " << strerror(errno) << RESET << "\n";
    ERR("cgiReadHandler: %s", strerror(errno));
    updateEvent(ev_list.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    close(cl->pipe_in[0]);
    close(cl->pipe_out[0]);
    // std::cout << "closing: " << cl->pipe_in[0] << " " << cl->pipe_out[0] << "\n";
    DEBUG("PIPES ends closing: %d %d", cl->pipe_in[0], cl->pipe_out[0]);
  }
  else
  {
    // std::cerr << BOLDWHITE << "bytes READ: " << bytesRead << "\n" << RESET;
    DEBUG("Bytes Read: %d", bytesRead);
    message.append(buffer);


    HTTPResponse cgiResponse;
    cgiResponse.setBody(message);
    cgiResponse.addHeader("Content-Length", std::to_string(message.size()));
    Message cgiMessage = Message(cgiResponse);
    cl->setMessage(cgiMessage);
    message.clear();
    updateEvent(cl->getSockFD(), EVFILT_READ, EV_DISABLE, 0, 0, NULL);
    updateEvent(cl->getSockFD(), EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
  }
}

bool ServerManager::CgiWriteHandler(Client *cl, struct kevent ev_list)
{
    int bytes_sent;
    if (cl == NULL)
        return false;

    Message message = cl->getMessage();

    if (message.size() == 0)
      bytes_sent = 0;
    else if (message.size() >= BUFFERSIZE)
    {
      bytes_sent = write(ev_list.ident, message.getMessage().c_str() + message.getBufferSent(), BUFFERSIZE);
      DEBUG("Body sent to CGI-Script: %s", message.getMessage().c_str() + message.getBufferSent());
      // std::cerr << "WROTE: " << MAGENTA << message.getMessage().c_str() + message.getBufferSent() << RESET << "\n";
    }
    else
    {
      bytes_sent = write(ev_list.ident, message.getMessage().c_str() + message.getBufferSent(), message.size());
      DEBUG("Body sent to CGI-Script: %s", message.getMessage().c_str() + message.getBufferSent());
      // std::cerr << MAGENTA << message.getMessage().c_str() + message.getBufferSent() << RESET << "\n";
    }

    if (bytes_sent < 0)
    {
      ERR("Unable to send Body to CGI-Script");
      // std::cerr << "ERROR: sending body to cgi\n" << RESET;
      deleteCgi(_cgiWrite, cl, EVFILT_WRITE);
      // updateEvent(ev_list.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
      // close(cl->pipe_in[1]);
      // close(cl->pipe_out[1]);
    }

    else if (bytes_sent == 0 || bytes_sent == message.size())
    {
      deleteCgi(_cgiWrite, cl, EVFILT_WRITE);
      // bool isWrite = true;
      // bool isRead = false;
      // close(cl->pipe_in[1]);
      // close(cl->pipe_out[1]);
      // handleEOF(cl, ev_list.ident, isRead, isWrite);
      updateEvent(cl->pipe_out[0], EVFILT_READ, EV_ENABLE, 0, 0, NULL);
    }
    else
    {
      message.setMessage(message.getMessage().substr(bytes_sent));
      cl->setMessage(message);
    }
    return true;
}

void ServerManager::closeConnection(Client *cl)
{
  static int i = 0;
  DEBUG("number of times gone in closeConnection: %d", ++i);
  if (cl == NULL)
    return;
  // std::cout << BOLDYELLOW << "closing connection with: " << cl->getSockFD() << RESET << "\n";
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
  if (cl == NULL)
    return false;

  // char ClientMessage[dataLen - cl->getBufferRead()];
  char ClientMessage[BUFFERSIZE];

  int readLen = recv(cl->getSockFD(), ClientMessage, dataLen, MSG_DONTWAIT); // MSG_DONTWAIT is similar to O_NONBLOCK

  if (readLen == 0)
  {
    // WARN("Client: %d disconnected!", cl->getSockFD());
    // std::cout << RED << "Client disconnected\n"
              // << RESET;
    closeConnection(cl);
    return false;
  }
  else if (readLen < 0)
  {
    ERR("unable to recv from %d", cl->getSockFD());
    // std::cout << RED << "Something went wrong when reading\n"
    //           << RESET;
    closeConnection(cl);
    return false;
  }
  cl->appendRecvMessage(ClientMessage, readLen);
  cl->setBufferRead(readLen);
  ClientMessage[readLen] = '\0';
  DEBUG("Recived from: %d\n%s%s%s", cl->getSockFD(), BLUE, ClientMessage, RESET);
  // std::cout << "received: " YELLOW << ClientMessage << RESET << std::endl;

  if (cl->getBufferRead() == dataLen)
  {
    HTTPRequest *_req = parseRequest(cl, cl->getRecvMessage());
    if (_req == NULL)
    {
      ERR("Failed to parse request from %d: ", cl->getSockFD());
      // std::cerr << RED << "Failed to parse request\n"
      //           << RESET;
      closeConnection(cl);
      return false;
    }
    if (_req->getUri() == "/favicon.ico")
      std::cout << "\n";

    if (_req->getMethod() == POST)
    {
      launchCgi(*_req, cl);
      Message message(_req->getBody());
      cl->setMessage(message);
      delete _req; // Clean up dynamically allocated memory
      return 2;
    }

    HTTPResponse _resp(*_req);
    Message message(_resp);
    cl->setMessage(message);
    cl->setBufferRead(0);
    cl->resetRecvMessage();
    delete _req; // Clean up dynamically allocated memory
    return false;
  }

  return true;
}

/*
send may not send the full response, therefore we have to check
if the actual amount send was equal to the length of the respoinse string.
if not we have to send it in the second try.
*/
bool ServerManager::handleWriteEvent(Client *cl, int dataLen)
{
  (void)dataLen;
  Message message = cl->getMessage();

  int attempSend = message.size();
  if (message.getBufferSent() == attempSend)
  {
    // message.setBufferSent(0);
    // cl->setMessage(message);
    closeConnection(cl);
    return false;
  }
  // std::cout << message.getMessage() << std::endl;
  int actualSend = send(cl->getSockFD(), message.getMessage().c_str(), attempSend, 0);
  DEBUG("sent to: %d: \n%s%s%s", cl->getSockFD(), GREEN, message.getMessage().c_str(), RESET);
  // std::cout << GREEN << "send the httpResponse\n"
            // << RESET;
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
        // std::cout << "Could not find error page\n"
                  // << std::endl;
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

  std::stringstream ss(message);
  std::string line;

  std::getline(ss, line, '\n');
  std::stringstream line_stream(line);
  std::getline(line_stream, method, ' ');
  std::getline(line_stream, uri, ' ');
  std::getline(line_stream, version, '\r');

  // Parse headers
  while (std::getline(ss, line) && !line.empty())
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
  return (new HTTPRequest(headers, body, meth, uri, HTTP_1_1));
}

void ServerManager::launchCgi(HTTPRequest const &request, Client *cl)
{
  (void)request;
  DEBUG("stepping into launchCgi");
  // std::cout << "POSTHandler" << std::endl;
  // int pipe_out[2];
  // int pipe_in[2];

  if (pipe(cl->pipe_out) < 0)
  {
    ERR("Failed pipe_out cgi");
    std::cerr << RED << "failed pipe_out cgi\n" << RESET;
    return ;
  }

  if (pipe(cl->pipe_in) < 0)
  {
    // std::cerr << RED << "failed to pipe to cgi\n" << RESET;
    ERR("Failed pipe_in cgi");
    return ;
  }
  // cl->setPipeFrom(pipe_in);
  // cl->setPipeTo(pipe_out);

  // Fork to create a child process for the CGI script
  pid_t pid = fork();
  if (pid == 0)
  {
    dup2(cl->pipe_in[0], STDIN_FILENO);
    dup2(cl->pipe_out[1], STDOUT_FILENO);
    close(cl->pipe_out[0]);
    close(cl->pipe_out[1]);
    close(cl->pipe_in[0]);
    close(cl->pipe_in[1]);

    // Execute the CGI script
    execl("application/cgiBin/login.sh", "application/cgiBin/login.sh", NULL);

    // If execl fails
    perror("execl");
    std::cerr << "something happeneed to cgi\n";
    exit(EXIT_FAILURE);
  }
  else if (pid > 0)
  {
    updateEvent(cl->pipe_in[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
    updateEvent(cl->pipe_out[0], EVFILT_READ, EV_ADD | EV_DISABLE, 0, 0, NULL);
    close(cl->pipe_in[0]);
    close(cl->pipe_out[1]);
    _cgiWrite.insert(std::pair<int, Client *>(cl->pipe_in[1], cl));
    _cgiRead.insert(std::pair<int, Client *>(cl->pipe_out[0], cl));

    DEBUG("%s %d", "pipe_in[0] = ", cl->pipe_in[0]);
    DEBUG("%s %d", "pipe_in[1] = ", cl->pipe_in[1]);
    DEBUG("%s %d", "pipe_out[0] = ", cl->pipe_out[0]);
    DEBUG("%s %d", "pipe_out[1] = ", cl->pipe_out[1]);
  }
  else
  {
    ERR("Failed to fork: %s", strerror(errno));
  }
    // perror("fork");
}
