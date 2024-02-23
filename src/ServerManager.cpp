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
  Client *myClient;
  // bool writing = true;

  int numServers = _servers.size();
  ev_set_count = numServers;
  accepting = true; // to check if socket should be added to the queue.
  std::cout << "\n\n\n\n\n";
  createQ();
  while (true)
  {
    int nev = kevent(kq, NULL, 0, ev_list, MAX_EVENTS, nullptr);
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
          std::cout << ev_list[i].ident << ":client does not exist\n";
          continue;
        }
        if (ev_list[i].flags & EV_EOF)
        {
          std::cout << "\n\nclosing connection with: " << ev_list[i].ident << "\n\n";
          closeConnection(myClient);
          continue;
        }
        if (ev_list[i].filter == EVFILT_READ)
        {
          if (!readClient(myClient, ev_list[i].data))
          {
            updateEvent(ev_list[i].ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
            updateEvent(ev_list[i].ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
          }
        }
        else if (ev_list[i].filter == EVFILT_WRITE)
        {
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
bool ServerManager::readClient(Client *cl, int dataLen)
{
  if (cl == NULL)
    return false;
  char ClientMessage[
    dataLen - cl->getBufferRead()];

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
      std::cout << RED << "Client disconnected\n" << RESET;
      closeConnection(cl);
    }
    if (readLen < 0)
    {
      std::cout << RED << "Something went wrong when reading\n" << RESET;
      closeConnection(cl);
    }
    // return true;
  }
  if (cl->getBufferRead() == dataLen)
  {
    HTTPRequest *_req = parseRequest(cl, cl->getRecvMessage());
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
  int actualSend = send(cl->getSockFD(), message.getMessage().c_str(), attempSend, 0);
  std::cout << GREEN << "send the httpResponse\n" << RESET;
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
      if (!file.is_open()) {
        std::cout << "could not file error page\n" << std::endl;
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
  std::cout << "\n\n\nbody: " << body << "\n\n\n\n";

  Method meth;
  if (method == "GET")
    meth = GET;
  if (method == "POST")
    meth = POST;
  return (new HTTPRequest(headers, body, meth, uri, HTTP_1_1));
}