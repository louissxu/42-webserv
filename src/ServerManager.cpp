#include "ServerManager.hpp"

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

void ServerManager::handleEOF(Client *cl, int fd, bool &isRead, bool &isWrite)
{
  if (isRead)
  {
    updateEvent(fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    updateEvent(cl->getSockFD(), EVFILT_READ, EV_DISABLE, 0, 0, NULL);
    updateEvent(cl->getSockFD(), EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
    close(fd);
    isRead = false;
  }
  else if (isWrite)
  {
    updateEvent(fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    isWrite = false;
  }
  else
  {
    closeConnection(cl);
    std::cout << "\n\nclosing connection with: " << fd << "\n\n";
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
        handleEOF(myClient, ev_list[i].ident, isCgiRead, isCgiWrite);
        continue;
      }
      if (isCgiWrite)
      {
        CgiWriteHandler(myClient, ev_list[i]);
        isCgiWrite = false;
      }
      else if (isCgiRead)
      {
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
  char buffer[10000];
  ssize_t bytesRead;
  static std::string message = "";
  std::cout << "cgi listenning socket was: " << ev_list.ident << std::endl;
  bytesRead = read(ev_list.ident, buffer, sizeof(buffer));

  if (bytesRead == 0)
  {
    updateEvent(ev_list.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
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
    updateEvent(ev_list.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    std::cout << "closing: " << cl->pipe_from_cgi[0] << " " << cl->pipe_to_cgi[0] << "\n";
    close(cl->pipe_from_cgi[0]);
    close(cl->pipe_to_cgi[0]);
    std::cerr << "something really bad happenned\n\n";
  }
  else
  {
    std::cout << "bytesRead: " << bytesRead << "\n\n";
    message += buffer;
    message.append("\0", 1);
    memset(buffer, 0, sizeof(buffer));
    updateEvent(ev_list.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    std::cout << "closing: " << cl->pipe_from_cgi[0] << " " << cl->pipe_to_cgi[0] << "\n";
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
    message.clear();
  }
  // updateEvent(ev_list.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
}

bool ServerManager::CgiWriteHandler(Client *cl, struct kevent ev_list)
{
    if (cl == NULL)
        return false;

    Message message = cl->getMessage();

    if ((size_t)message.getBufferSent() >= message.getMessage().size())
    {
        updateEvent(ev_list.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
        updateEvent(cl->pipe_from_cgi[0], EVFILT_READ, EV_ENABLE, 0, 0, NULL);
        std::cout << "closing: " << cl->pipe_from_cgi[1] << " " << cl->pipe_to_cgi[1] << "\n";
        close(cl->pipe_from_cgi[1]);
        close(cl->pipe_to_cgi[1]);
        return false;
    }
    std::cout<<message.getMessage().size()<<"\n";
    // size_t writeLen = send(ev_list.ident, message.getMessage().c_str() + allSend, message.getMessage().size() - allSend, 0);
    size_t writeLen = write(ev_list.ident, message.getMessage().c_str() + message.getBufferSent(), message.getMessage().size() - message.getBufferSent());

    if (writeLen < 0)
    {
        updateEvent(ev_list.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
        updateEvent(cl->pipe_from_cgi[0], EVFILT_READ, EV_ENABLE, 0, 0, NULL);
        std::cout << "closing: " << cl->pipe_from_cgi[1] << " " << cl->pipe_to_cgi[1] << "\n";
        close(cl->pipe_from_cgi[1]);
        close(cl->pipe_to_cgi[1]);
        std::cerr << "Send error: " << strerror(errno) << std::endl;
        return false;
    }

    message.setBufferSent(writeLen);
    cl->setMessage(message);
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
  std::cout << BOLDYELLOW << "closing connection with: " << cl->getSockFD() << RESET << "\n";
  delete cl;
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

  int attempSend = message.getMessageSize();
  if (message.getBufferSent() == attempSend)
  {
    // message.setBufferSent(0);
    // cl->setMessage(message);
    closeConnection(cl);
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
    execl("application/cgiBin/login.sh", "application/cgiBin/login.sh", NULL);

    // If execl fails
    perror("execl");
    std::cerr << "something happeneed to cgi\n";
  }
  else if (pid > 0)
  {
    updateEvent(cl->pipe_to_cgi[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
    updateEvent(cl->pipe_from_cgi[0], EVFILT_READ, EV_ADD | EV_DISABLE, 0, 0, NULL);

    _cgiWrite.insert(std::pair<int, Client *>(cl->pipe_to_cgi[1], cl));
    _cgiRead.insert(std::pair<int, Client *>(cl->pipe_from_cgi[0], cl));

    std::cout << "write end = " << cl->pipe_to_cgi[1] << "\n";
    std::cout << "read end = " << cl->pipe_from_cgi[0] << "\n";
  }
  else
    perror("fork");
}


/*------------------------------------------*\
|              ISAAC'S STUFF                 |
\*------------------------------------------*/

#include "ServerManager.hpp"

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

          p_c(*it);
        i++;
      }
      //starting the server now that the required fields have been populated.
      newServ.startServer();
      this->addServer(newServ);
    }
}

void    ServerManager::p_c(ConfigParser &src)
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
        p_c(*it);
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
      p_c(src);
    }
}
