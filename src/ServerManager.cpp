#include "ServerManager.hpp"
#include "Cout.hpp"

static void areFdsOpen(int pipein[2], int pipeout[2]) {
  int fd = pipein[0];
  for (int i = 0; i < 2; i++)
  {
    // Use fcntl with F_GETFD to get the file descriptor flags
    int flags = fcntl(fd, F_GETFD);
    if (flags != -1)
      std::cout << BLUE << fd << " is open\n" << RESET;
    else
      std::cout << BLUE << fd << " is not open\n" << RESET;
    fd = pipein[1];
  }

  fd = pipeout[0];
  for (int i = 0; i < 2; i++)
  {
    // Use fcntl with F_GETFD to get the file descriptor flags
    int flags = fcntl(fd, F_GETFD);
    if (flags != -1)
      std::cout << BLUE << fd << " is open\n" << RESET;
    else
      std::cout << BLUE << fd << " is not open\n" << RESET;
    fd = pipeout[1];
  }
    // return (flags != -1); // If fcntl returns -1, the file descriptor is not open
}

// static void isFdOpen(int fd) {
//     // Use fcntl with F_GETFD to get the file descriptor flags
//     int flags = fcntl(fd, F_GETFD);
//     if (flags != -1)
//       std::cout << BLUE << fd << " is open\n" << RESET;
//     else
//       std::cout << BLUE << fd << " is not open\n" << RESET;
//     // return (flags != -1); // If fcntl returns -1, the file descriptor is not open
// }

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
    perror("kqueue");
    exit(EXIT_FAILURE);
  }

  for (size_t i = 0; i < _servers.size(); i++)
  {
    EV_SET(&ev_set[i], _servers[i].getSockFd(), EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &ev_set[i], 1, NULL, 0, NULL) == -1)
    {
      perror("kevent");
      exit(EXIT_FAILURE);
    }
  }
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
  std::cout << "\n\nclosing connection with: " << fd << "\n\n";
  if (isCgiRead)
  {
    updateEvent(cl->getSockFD(), EVFILT_READ, EV_DISABLE, 0, 0, NULL);
    updateEvent(cl->getSockFD(), EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
    std::map<int, Client *>::iterator it = _cgiRead.find(fd);
    if (it != _cgiRead.end())
    {
      updateEvent(it->first, EVFILT_READ, EV_DELETE, 0, 0, NULL);
      close(it->first);
      _cgiRead.erase(it);
    }
    isCgiRead = false;
  }
  else if (isCgiWrite)
  {
    std::map<int, Client *>::iterator it = _cgiWrite.find(fd);
    if (it != _cgiWrite.end())
    {
      updateEvent(it->first, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
      close(it->first);
      _cgiWrite.erase(it);
    }
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
          std::cout << RED << ev_list[i].ident << ": client does not exist\n"
                    << RESET;
          continue;
        }
      }
      if (ev_list[i].flags & EV_EOF)
      {
        areFdsOpen(myClient->pipe_in, myClient->pipe_out);
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
      {areFdsOpen(myClient->pipe_in, myClient->pipe_out);
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
  std::cerr << "READ: " << MAGENTA << buffer << RESET << "\n";

  if (bytesRead == 0)
  {
    std::cerr << BOLDWHITE << "bytes READ = 0\n" << RESET;
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
    std::cerr << RED << "ERROR: cgiReadHandler: " << strerror(errno) << RESET << "\n";
    updateEvent(ev_list.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    close(cl->pipe_in[0]);
    close(cl->pipe_out[0]);
    std::cout << "closing: " << cl->pipe_in[0] << " " << cl->pipe_out[0] << "\n";
  }
  else
  {
    std::cerr << BOLDWHITE << "bytes READ: " << bytesRead << "\n" << RESET;
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
      std::cerr << "WROTE: " << MAGENTA << message.getMessage().c_str() + message.getBufferSent() << RESET << "\n";
    }
    else
    {
      bytes_sent = write(ev_list.ident, message.getMessage().c_str() + message.getBufferSent(), message.size());
      std::cerr << MAGENTA << message.getMessage().c_str() + message.getBufferSent() << RESET << "\n";
    }

    if (bytes_sent < 0)
    {
      std::cerr << "ERROR: sending body to cgi\n" << RESET;
      updateEvent(ev_list.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
      close(cl->pipe_in[1]);
      close(cl->pipe_out[1]);
    }

    else if (bytes_sent == 0 || bytes_sent == message.size())
    {
      bool isWrite = true;
      bool isRead = false;
      close(cl->pipe_in[1]);
      close(cl->pipe_out[1]);
      handleEOF(cl, ev_list.ident, isRead, isWrite);
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
  if (cl == NULL)
    return;
  std::cout << BOLDYELLOW << "closing connection with: " << cl->getSockFD() << RESET << "\n";
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
    std::cout << RED << "Client disconnected\n"
              << RESET;
    closeConnection(cl);
    return false;
  }
  else if (readLen < 0)
  {
    std::cout << RED << "Something went wrong when reading\n"
              << RESET;
    closeConnection(cl);
    return false;
  }
  cl->appendRecvMessage(ClientMessage, readLen);
  cl->setBufferRead(readLen);
  ClientMessage[readLen] = '\0';
  std::cout << "received: " YELLOW << ClientMessage << RESET << std::endl;

  if (cl->getBufferRead() == dataLen)
  {
    HTTPRequest *_req = parseRequest(cl, cl->getRecvMessage());
    if (_req == NULL)
    {
      std::cerr << RED << "Failed to parse request\n"
                << RESET;
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
    // closeConnection(cl);
    return false;
  }
  std::cout << message.getMessage() << std::endl;
  int actualSend = send(cl->getSockFD(), message.getMessage().c_str(), attempSend, 0);
  std::cout << GREEN << "send the httpResponse\n"
            << RESET;
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
        std::cout << "could not file error page\n"
                  << std::endl;
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
  std::cout << "POSTHandler" << std::endl;
  // int pipe_out[2];
  // int pipe_in[2];

  if (pipe(cl->pipe_out) < 0)
  {
    std::cerr << RED << "failed to pipe to cgi\n" << RESET;
    return ;
  }

  if (pipe(cl->pipe_in) < 0)
  {
    std::cerr << RED << "failed to pipe to cgi\n" << RESET;
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


    std::cout << "pipe_in[0] = " << cl->pipe_in[0] << "\n";
    std::cout << "pipe_in[1] = " << cl->pipe_in[1] << "\n";
    std::cout << "pipe_out[0] = " << cl->pipe_out[0] << "\n";
    std::cout << "pipe_out[1] = " << cl->pipe_out[1] << "\n";
  }
  else
  {
    std::cerr << RED << "forking failed\n" << RESET;
  }
    // perror("fork");
}
