#include "cgi.hpp"

Cgi::Cgi() {}

Cgi::~Cgi() {}

void Cgi::setEnv(HTTPRequest &req)
{
	_envVec.push_back("Content-Length=" + req.getHeader("Content-Length"));
	_envVec.push_back("User-Agent=" + req.getHeader("User-Agent"));
	_envVec.push_back("Content-Type=" + req.getHeader("Content-Type"));
	_envVec.push_back("User-Agent=" + req.getHeader("User-Agent"));
	_envVec.push_back("Method=" + req.getMethodString());
	_envVec.push_back("QUERY_STRING=" + req.getBody());

	if (!req.getHeader("Cookie").empty())
	{
		// req.setHeader("Cookie", req.getHeader("Cookie"));
		_envVec.push_back("Cookie=" + req.getHeader("Cookie"));
	}
	// _env = (char **)malloc(sizeof(char *) * (_envVec.size() + 1));
	_env = std::vector<char *>(_envVec.size() + 1);
	std::vector<std::string>::iterator it;
	int i = 0;
	for (it = _envVec.begin(); it != _envVec.end(); it++, i++)
	{
		_env[i] = new char[(*it).size() + 1];
		strcpy(_env[i], const_cast<char *>(it->c_str()));
	}
	_env[i] = nullptr;
}

void Cgi::setArgv(HTTPRequest const &req)
{
	_argv = std::vector<char *>(3);
	// std::string pythonPath = "/Library/Frameworks/Python.framework/Versions/3.10/bin/python3";
	std::string pythonPath = "/usr/local/bin/python3";
	std::string cgiScript = "application" + req.getUri();

	_argv[0] = new char[pythonPath.size() + 1];
	_argv[1] = new char[cgiScript.size() + 1];

	strcpy(_argv[0], const_cast<char *>(pythonPath.c_str()));
	strcpy(_argv[1], const_cast<char *>(cgiScript.c_str()));
	_argv[2] = nullptr;
}

void Cgi::CgiReadHandler(ServerManager &sm, Client *cl, struct kevent ev_list)
{
	char buffer[BUFFERSIZE * 2];
	memset(buffer, 0, sizeof(buffer));
	int bytesRead = 0;
	static std::string message = "";
	bytesRead = read(ev_list.ident, buffer, BUFFERSIZE * 2);
	DEBUG("cgiReadHandler: Read: %s", buffer);
	if (bytesRead == 0)
	{
		DEBUG("cgiReadHandler: Bytes Read = 0");
		sm.updateEvent(ev_list.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		close(cl->pipe_in[0]);
		close(cl->pipe_out[0]);
		
		HTTPResponse cgiResponse;

		int status;
		waitpid(cl->Cgipid, &status, WCONTINUED);
		if (!WIFEXITED(status))
		{
			std::map<std::string, std::string> _;
			HTTPRequest errorReq(_, "", GET, "/error/E50x.html", HTTP_1_1, false);
			cgiResponse = HTTPResponse(errorReq);
			return ;
		}

		cgiResponse.setBody(message);
		cgiResponse.addHeader("Content-Length", std::to_string(message.size()));
		Message cgiMessage = Message(cgiResponse);
		cl->setMessage(cgiMessage);
		message.clear();

		sm.updateEvent(cl->getSockFD(), EVFILT_READ, EV_DISABLE, 0, 0, NULL);
		sm.updateEvent(cl->getSockFD(), EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
	}
	else if (bytesRead < 0)
	{
		ERR("cgiReadHandler: %s", strerror(errno));
		sm.updateEvent(ev_list.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		close(cl->pipe_in[0]);
		close(cl->pipe_out[0]);
		DEBUG("PIPES ends closing: %d %d", cl->pipe_in[0], cl->pipe_out[0]);
	}
	else
	{
		DEBUG("Bytes Read: %d", bytesRead);
		message.append(buffer);

		HTTPResponse cgiResponse = sm.getResponse();
		cgiResponse.setVersion("HTTP/1.1");
		cgiResponse.setBody(message);
		// if (cgiResponse.getUri() == "/cgi-bin/loogin.py")
		cgiResponse.addHeader("Content-Length", std::to_string(message.size()));
		// cgiResponse.addHeader("session-id", )
		Message cgiMessage = Message(cgiResponse);
		cl->setMessage(cgiMessage);
		message.clear();
		
		int status;
		waitpid(cl->Cgipid, &status, WCONTINUED);
		if (WIFEXITED(status))
		{
			std::cout << WEXITSTATUS(status) << std::endl;
			if (WEXITSTATUS(status) == 1)
			{
				std::map<std::string, std::string> _;
				HTTPRequest errorReq(_, "", GET, "/error/E50x.html", HTTP_1_1, false);
				cgiResponse = HTTPResponse(errorReq);
				Message cgiMessage = Message(cgiResponse);
				cl->setMessage(cgiMessage);
				message.clear();
			}
		}
		if (!WIFCONTINUED(status))
		{
			sm.updateEvent(cl->getSockFD(), EVFILT_READ, EV_DISABLE, 0, 0, NULL);
			sm.updateEvent(cl->getSockFD(), EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
		}
	}
}

bool Cgi::CgiWriteHandler(ServerManager &sm, Client *cl, struct kevent ev_list)
{
	int bytes_sent;
	if (cl == NULL)
		return false;

	Message message = cl->getMessage();

	if (message.size() == 0)
	{
		bytes_sent = 0;
		DEBUG("Body sent to CGI-Script: %s", message.getMessage().c_str() + message.getBufferSent());
	}
	else if (message.size() >= BUFFERSIZE)
	{
		bytes_sent = write(ev_list.ident, message.getMessage().c_str() + message.getBufferSent(), BUFFERSIZE);
		DEBUG("Body sent to CGI-Script: %s", message.getMessage().c_str() + message.getBufferSent());
	}
	else
	{
		bytes_sent = write(ev_list.ident, message.getMessage().c_str() + message.getBufferSent(), message.size());
		DEBUG("Body sent to CGI-Script: %s", message.getMessage().c_str() + message.getBufferSent());
	}

	if (bytes_sent < 0)
	{
		ERR("Unable to send Body to CGI-Script");
		return false;
		// sm.deleteCgi(sm.get_cgiWrite(), cl, EVFILT_WRITE);
	}

	else if (bytes_sent == 0 || bytes_sent == message.size())
	{
		// deleteCgi(_cgiWrite, cl, EVFILT_WRITE);
		sm.updateEvent(cl->pipe_out[0], EVFILT_READ, EV_ENABLE, 0, 0, NULL);
		return false;
	}
	else
	{
		message.setMessage(message.getMessage().substr(bytes_sent));
		cl->setMessage(message);
	}
	return true;
}

// !unsure when to free _argv and _env
void Cgi::launchCgi(HTTPRequest &req, Client *cl)
{
	setArgv(req);
	setEnv(req);

	DEBUG("stepping into launchCgi");
	if (pipe(cl->pipe_out) < 0)
	{
		ERR("Failed pipe_out cgi");
		return;
	}
	if (pipe(cl->pipe_in) < 0)
	{
		ERR("Failed pipe_in cgi");
		return;
	}
	// std::vector<char *>::iterator it;
	// for (it = _env.begin(); it != _env.end(); ++it)
	// 	DEBUG("%s", *it);
	// Fork to create a child process for the CGI script
	cl->Cgipid = fork();
	if (cl->Cgipid == 0)
	{
		dup2(cl->pipe_in[0], STDIN_FILENO);
		dup2(cl->pipe_out[1], STDOUT_FILENO);
		close(cl->pipe_out[0]);
		close(cl->pipe_out[1]);
		close(cl->pipe_in[0]);
		close(cl->pipe_in[1]);
		execve(_argv[0], _argv.data(), _env.data());
		// If execl fails
		ERR("execve: %s %s", _argv[1], strerror(errno));
		// perror("execl");
		// std::cerr << "something happeneed to cgi\n";
		exit(EXIT_FAILURE);
	}
	else if (cl->Cgipid > 0)
	{
		// DEBUG("child process id = %d", pid);
		// updateEvent(cl->pipe_in[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
		// updateEvent(cl->pipe_out[0], EVFILT_READ, EV_ADD | EV_DISABLE, 0, 0, NULL);
		close(cl->pipe_in[0]);
		close(cl->pipe_out[1]);
		// _cgiWrite.insert(std::pair<int, Client *>(cl->pipe_in[1], cl));
		// _cgiRead.insert(std::pair<int, Client *>(cl->pipe_out[0], cl));

		// DEBUG("%s %d", "pipe_in[0] = ", cl->pipe_in[0]);
		// DEBUG("%s %d", "pipe_in[1] = ", cl->pipe_in[1]);
		// DEBUG("%s %d", "pipe_out[0] = ", cl->pipe_out[0]);
		// DEBUG("%s %d", "pipe_out[1] = ", cl->pipe_out[1]);
	}
	else
	{
		ERR("Failed to fork: %s", strerror(errno));
	}
	// perror("fork");
}
