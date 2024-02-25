#include "ServerManager.hpp"
#include "Cout.hpp"

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

void ServerManager::acceptClient(int indexListenSocket)
{
  int clientFD;
  struct sockaddr_in client_address;
  long client_address_size = sizeof(client_address);
  clientFD = accept(_servers[indexListenSocket].getSockFd(), (struct sockaddr *)&client_address, (socklen_t *)&client_address_size);
  if (clientFD < 0)
  {
    perror("accept");
  }
  // std::cout << "accepted: " << conn_fd << std::endl;
  // Add the new socket to kqueue
  updateEvent(clientFD, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
  updateEvent(clientFD, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
  if (fcntl(clientFD, F_SETFL, O_NONBLOCK) < 0)
  {
    std::cout << "fcntl error: closing: " << clientFD << std::endl;
    close(clientFD);
    return;
  }
  Client *cl = new Client(clientFD, _servers[indexListenSocket].getSockFd(), client_address);
  _clients.insert(std::pair<int, Client *>(clientFD, cl));
}

Client *ServerManager::getClient(int fd)
{
  std::map<int, Client *>::iterator it = _clients.find(fd);
  if (it != _clients.end())
    return it->second;
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
  {
    if (_servers[i].getSockFd() == socket_fd)
    {
      return true;
    }
  }
  return false;
}

void ServerManager::updateEvent(int ident, short filter, u_short flags, u_int fflags, int data, void *udata)
{
  struct kevent kev;
  EV_SET(&kev, ident, filter, flags, fflags, data, udata);
  kevent(kq, &kev, 1, NULL, 0, NULL);
}

void ServerManager::runKQ()
{
  #define LOG_LEVEL ERR_LEVEL
  Client *myClient;
  bool isCgiRead = false;
  bool isCgiWrite = false;
  // bool writing = true;
  ERR("socket: %s", "strerror(errno)");

  int numServers = _servers.size();
  ev_set_count = numServers;
  accepting = true; // to check if socket should be added to the queue.
  std::cout << "\n\n";
  createQ();
  while (true)
  {
    int nev = kevent(kq, NULL, 0, ev_list, MAX_EVENTS, nullptr);
    // std::map<int, Client *>::iterator it = (_clients.begin();
    // for (; it != _clients.end(); ++it)
    //   std::cout << "client = " << it->first << std::endl;
    if (nev == 0)
      continue;

    for (int i = 0; i < nev; i++)
    {
      for (int j = 0; j < numServers; j++)
      {
        if (ev_list[i].ident == (unsigned int)_servers[j].getSockFd())
        {
          acceptClient(j);
          break;
        }
      }
      if (!isListeningSocket(ev_list[i].ident))
      {
        myClient = getClient(ev_list[i].ident);
        if (myClient == nullptr)
        {
          myClient = getCgiRead(ev_list[i].ident);
          if (myClient == nullptr)
          {
            myClient = getCgiWrite(ev_list[i].ident);
            if (myClient == nullptr)
            {
              std::cout << ev_list[i].ident << ":client does not exist\n";
              continue;
            }
            else
            {
              std::cout << "cgi writer descriptor = " << ev_list[i].ident << "\n";
              isCgiWrite = true;
            }
          }
          else
          {
            std::cout << "cgi listenning descriptor = " << ev_list[i].ident << "\n";
            isCgiRead = true;
          }
        }
        else
          std::cout << "normal descriptor = " << ev_list[i].ident << "\n";
        if (ev_list[i].flags & EV_EOF)
        {
          if (isCgiRead)
          {
            updateEvent(ev_list[i].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
            updateEvent(myClient->getSockFD(), EVFILT_READ, EV_DISABLE, 0, 0, NULL);
            updateEvent(myClient->getSockFD(), EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
            close(ev_list[i].ident);
            // std::cout << "\n\nclosing connection with: " << ev_list[i].ident << "\n\n";
            isCgiRead = false;
            isCgiWrite = false;
          }
          else if (isCgiWrite)
          {
            updateEvent(ev_list[i].ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
            // close
            // int readFd = getCgiReadFd(myClient);
            // if (readFd == -1)
            //   exit(EXIT_FAILURE);
            // else
            //   updateEvent(readFd, EVFILT_READ, EV_ENABLE, 0, 0, NULL);
            // // updateEvent(myClient->getSockFD(), EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
            // close(ev_list[i].ident);
            // // std::cout << "\n\nclosing connection with: " << ev_list[i].ident << "\n\n";
            isCgiWrite = false;
          }
          else
          {
            std::cout << "trying to close connection\n";
            closeConnection(myClient);
          }
          std::cout << "\n\nclosing connection with: " << ev_list[i].ident << "\n\n";
          continue;
        }

        if (isCgiWrite)
        {
          if (!CgiWriteHandler(myClient, ev_list[i]))
          isCgiWrite = false;
          continue;
        }
        else if (isCgiRead)
        {
          CgiReadHandler(myClient, ev_list[i]);
          isCgiRead = false;
          continue;
        }
        if (ev_list[i].filter == EVFILT_READ)
        {
          std::cout << "going to read\n";
          int returnVal = readClient(myClient, ev_list[i].data);
          if (returnVal == 0)
          {
            updateEvent(ev_list[i].ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
            updateEvent(ev_list[i].ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
          }
          else if (returnVal == 2)
          {
            updateEvent(ev_list[i].ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
            updateEvent(ev_list[i].ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
          }
        }
        else if (ev_list[i].filter == EVFILT_WRITE)
        {
          std::cout << "going to write\n";
          if (!writeToClient(myClient, ev_list[i].data))
          {
            updateEvent(ev_list[i].ident, EVFILT_READ, EV_ENABLE, 0, 0, NULL);
            updateEvent(ev_list[i].ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
          }
        }
      }
    }
  }
}

void ServerManager::CgiReadHandler(Client *cl, struct kevent ev_list)
{
  char buffer[10000];
  ssize_t bytesRead;
  static std::string message = "";
  std::cout << "cgi listenning socket was: " << ev_list.ident << std::endl;
  bytesRead = read(ev_list.ident, buffer, sizeof(buffer));
  
  if (bytesRead == 0)
  {
    updateEvent(ev_list.ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
    close(cl->pipe_from_cgi[0]);
    close(cl->pipe_to_cgi[0]);
    wait(NULL);
    HTTPResponse cgiResponse;
    cgiResponse.setBody(message);
    Message cgiMessage = Message(cgiResponse);
    cl->setMessage(cgiMessage);
  }
  else if (bytesRead < 0)
  {
    updateEvent(ev_list.ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
    close(cl->pipe_from_cgi[0]);
    close(cl->pipe_to_cgi[0]);
    std::cerr << "something really bad happenned\n\n";
  }
  else
  {
    std::cout << "bytesRead: " << bytesRead << "\n\n";
    message += buffer;
    memset(buffer, 0, sizeof(buffer));
    updateEvent(ev_list.ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
    close(cl->pipe_from_cgi[0]);
    close(cl->pipe_to_cgi[0]);
    wait(NULL);
    HTTPResponse cgiResponse;
    cgiResponse.setBody(message);
    cgiResponse.addHeader("Content-Length", std::to_string(message.size()));
    Message cgiMessage = Message(cgiResponse);
    cl->setMessage(cgiMessage);

    updateEvent(cl->getSockFD(), EVFILT_READ, EV_DISABLE, 0, 0, NULL);
    updateEvent(cl->getSockFD(), EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
    // message.append("\0", 1);

  }
  // updateEvent(ev_list.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
}

// void ServerManager::CgiReadHandler(Client *cl, struct kevent ev_list)
// {
//   char buffer[10000];
//   ssize_t bytesRead;
//   std::string message = "";
//   std::cout << "listenning socket was: " << ev_list.ident << std::endl;
//   bytesRead = read(ev_list.ident, buffer, sizeof(buffer));
  
//   if (bytesRead == 0)
//   {
//     updateEvent(ev_list.ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
//     close(cl->pipe_from_cgi[0]);
//     close(cl->pipe_to_cgi[0]);
//     wait(NULL);
//   }
//   else if (bytesRead < 0)
//   {
//     updateEvent(ev_list.ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
//     close(cl->pipe_from_cgi[0]);
//     close(cl->pipe_to_cgi[0]);
//     std::cerr << "something really bad happenned\n\n";
//   }
//   else
//   {
//     std::cout << "bytesRead: " << bytesRead << "\n\n";
//     // message.append("\0", 1);

//     HTTPResponse cgiResponse;
//     cgiResponse.setBody(message);
//     Message cgiMessage = Message(cgiResponse);
//     cl->setMessage(cgiMessage);
//   }
//   // updateEvent(ev_list.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
// }

bool ServerManager::CgiWriteHandler(Client *cl, struct kevent ev_list)
{
  (void)cl;
  static size_t allSend = 0;
  std::string dataToSend = "username=mehdi&password=mirzaie";
  if (allSend == dataToSend.size())
  {
    updateEvent(ev_list.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    // close(cl->pipe_from_cgi[1]);
    close(cl->pipe_to_cgi[1]);
    return false;
  }
  size_t writeLen = write(ev_list.ident, dataToSend.c_str(), dataToSend.size());
  if (writeLen < 0)
  {
    updateEvent(ev_list.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    close(cl->pipe_from_cgi[1]);
    close(cl->pipe_to_cgi[1]);
    std::cerr << errno << std::endl;
    return true;
  }
  if (writeLen >= allSend)
    allSend = writeLen;
  // return false;
  return true;
}

void ServerManager::closeConnection(Client *cl)
{
  if (cl == NULL)
    return;
  updateEvent(cl->getSockFD(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
  updateEvent(cl->getSockFD(), EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  close(cl->getSockFD());
  _clients.erase(cl->getSockFD());
  delete cl;
}

// *unsure if recv will always read all the avialable data, need to learn.
int ServerManager::readClient(Client *cl, int dataLen)
{
  if (cl == NULL)
    return false;
  char ClientMessage[dataLen - cl->getBufferRead()];

  // memset(ClientMessage, 0, dataLen);
  if (cl->getBufferRead() != dataLen)
  {
    int readLen = recv(cl->getSockFD(), ClientMessage, dataLen, MSG_DONTWAIT); // MSG_DONTWAIT is similar to O_NONBLOCK
    cl->appendRecvMessage(ClientMessage);
    if (readLen >= cl->getBufferRead())
      cl->setBufferRead(readLen);
    std::cout << "recived: " YELLOW << ClientMessage << RESET << std::endl;
    // Response me = Response::deserialize(ClientMessage);
    if (readLen == 0)
    {
      std::cout << RED << "Client disconnected\n"
                << RESET;
      closeConnection(cl);
    }
    if (readLen < 0)
    {
      std::cout << RED << "Something went wrong when reading\n"
                << RESET;
      closeConnection(cl);
    }
    // return true;
  }
  if (cl->getBufferRead() == dataLen)
  {
    HTTPRequest *_req = parseRequest(cl, cl->getRecvMessage());
    if (_req->getMethod() == POST)
    {
      launchCgi(*_req, cl);
      return 2;
    }
    // (void)_req;
    HTTPResponse _resp(*_req);
    Message message(_resp);
    cl->setMessage(message);
    cl->setBufferRead(0);
    cl->resetRecvMessage();
    // HTTPRequest request = HTTPRequest::deserialize(ClientMessage, readLen);
    // request->parseRequest();
    return false;
  }
  return true;
  // std::cout << ClientMessage << readLen << std::endl;
}

/*
send may not send the full response, therefore we have to check
if the actual amount send was equal to the length of the respoinse string.
if not we have to send it in the second try.
*/
bool ServerManager::writeToClient(Client *cl, int dataLen)
{
  (void)dataLen;
  Message message = cl->getMessage();

  // static int buffer = 0;
  int attempSend = message.getMessageSize();
  if (message.getBufferSent() == attempSend)
    return false;
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
  // int pipe_to_cgi[2];
  // int pipe_from_cgi[2];

  pipe(cl->pipe_to_cgi);
  pipe(cl->pipe_from_cgi);
  // cl->setPipeFrom(pipe_from_cgi);
  // cl->setPipeTo(pipe_to_cgi);

  // Fork to create a child process for the CGI script
  pid_t pid = fork();
  if (pid == 0)
  {
    dup2(cl->pipe_to_cgi[0], STDIN_FILENO);
    dup2(cl->pipe_from_cgi[1], STDOUT_FILENO);
    close(cl->pipe_to_cgi[0]);
    close(cl->pipe_to_cgi[1]);
    close(cl->pipe_from_cgi[0]);
    close(cl->pipe_from_cgi[1]);

    // Execute the CGI script
    execl("application/cgiBin/login.sh", "application/cgiBin/login.sh", nullptr);

    // If execl fails
    perror("execl");
    std::cerr << "something happeneed to cgi\n";
  }
  else if (pid > 0)
  {
    updateEvent(cl->pipe_to_cgi[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
    updateEvent(cl->pipe_from_cgi[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);


    _cgiWrite.insert(std::pair<int, Client *>(cl->pipe_to_cgi[1], cl));
    _cgiRead.insert(std::pair<int, Client *>(cl->pipe_from_cgi[0], cl));
    
    std::cout << "write end = " << cl->pipe_to_cgi[1] << "\n";
    std::cout << "read end = " << cl->pipe_from_cgi[0] << "\n";
  }
  else
    perror("fork");
}











// void ServerManager::launchCgi(HTTPRequest const &request, Client *cl)
// {
//   (void)request;
//   std::cout << "POSTHandler" << std::endl;
//   int pipe_to_cgi[2];
//   int pipe_from_cgi[2];

//   pipe(pipe_to_cgi);
//   pipe(pipe_from_cgi);
//   cl->setPipeFrom(pipe_from_cgi);
//   cl->setPipeTo(pipe_to_cgi);

//   // Fork to create a child process for the CGI script
//   pid_t pid = fork();
//   if (pid == 0)
//   {
//     // Child process (CGI script)

//     // Close unused pipe ends
//     // close(pipe_to_cgi[1]);
//     // close(pipe_from_cgi[0]);

//     // Redirect standard input and output
//     dup2(pipe_to_cgi[0], STDIN_FILENO);
//     dup2(pipe_from_cgi[1], STDOUT_FILENO);

//     // Execute the CGI script
//     execl("application/cgiBin/login.sh", "application/cgiBin/login.sh", nullptr);

//     // If execl fails
//     perror("execl");
//     std::cerr << "something happeneed to cgi\n";
//     // exit(EXIT_FAILURE);
//   }
//   else if (pid > 0)
//   {
//     // Parent process (C++ server)

//     // Close unused pipe ends
//     // close(pipe_to_cgi[0]);
//     // close(pipe_from_cgi[1]);

//     // Write data to the CGI script
//     // const char *dataToSend = "username=mehdi&password=mirzaie";
//     updateEvent(pipe_to_cgi[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
//     _cgiWrite.insert(std::pair<int, Client *>(pipe_to_cgi[1], cl));
//     std::cout << "write end = " << pipe_to_cgi[1] << "\n";
//     // if (write(pipe_to_cgi[1], request.getBody().c_str(), request.getBody().size()) < 0)
//     //   std::cerr << errno << std::endl;

//     // close(pipe_to_cgi[1]);

//     // if (fcntl(pipe_from_cgi[0], F_SETFL, O_NONBLOCK) < 0)
//     // {
//     //   std::cerr << strerror(errno) << std::endl;
//     //   std::cout << "fcntl error: closing: " << pipe_from_cgi[0] << std::endl;
//     //   close((pipe_from_cgi[0]));
//     //   return;
//     // }
//     updateEvent(pipe_from_cgi[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
//     _cgiRead.insert(std::pair<int, Client *>(pipe_from_cgi[0], cl));
//     std::cout << "read end = " << pipe_from_cgi[0] << "\n";
//     // wait(NULL);
//   }
//   else
//   {
//     // Fork failed
//     perror("fork");
//     // exit(EXIT_FAILURE);
//   }
//   // return (-1);
// }