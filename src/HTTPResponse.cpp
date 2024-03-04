#include "HTTPResponse.hpp"

HTTPResponse::HTTPResponse()
{
	buildDefaultResponse();
}

HTTPResponse::HTTPResponse(std::string const &_version, Status const &_status, std::string const &_reason, std::map<std::string, std::string> const &_headers, std::string const &_body)
	: version(_version), status(_status), reason(_reason), headers(_headers), body(_body)
{
}

HTTPResponse::HTTPResponse(HTTPResponse const &src)
{
	*this = src;
}

HTTPResponse &HTTPResponse::operator=(HTTPResponse const &src)
{
	this->status = src.getStatusCode();
	this->reason = src.getReason();
	this->headers = src.getHeaders();
	this->body = src.getBody();
	return *this;
}

HTTPResponse::HTTPResponse(HTTPRequest const &request)
{
	buildDefaultResponse();

	// std::cout << "method was: " << request.getMethod() << std::endl;
	// if (request.getHeader("Cookie") != std::string())
	// 		headers.insert(std::pair<std::string, std::string>("Set-Cookie", request.getHeader("Set-Cookie")));

	switch (request.getMethod())
	{
	case Method(GET):
		GETHandler(request.getUri());
		return ;
	case Method(POST):
		if (request.getHeader("Set-Cookie") != std::string())
			headers.insert(std::pair<std::string, std::string>("Set-Cookie", request.getHeader("Set-Cookie")));
		return ;
	default:
		return ;
	}
}

std::string const &HTTPResponse::getVersion() const
{
	return this->version;
}

Status const &HTTPResponse::getStatusCode() const
{
	return this->status;
}

std::string HTTPResponse::getStatus() const
{
	switch (status)
	{
	case OK:
		return "OK";
	case CREATED:
		return "CREATED";
	case ACCEPTED:
		return "ACCEPTED";
	case NO_CONTENT:
		return "NO_CONTENT";
	case BAD_REQUEST:
		return "BAD_REQUEST";
	case FORBIDDEN:
		return "FORBIDDEN";
	case NOT_FOUND:
		return "NOT_FOUND";
	case REQUEST_TIMEOUT:
		return "REQUEST_TIMEOUT";
	case INTERNAL_SERVER_ERROR:
		return "INTERNAL_SERVER_ERROR";
	case BAD_GATEWAY:
		return "BAD_GATEWAY";
	case SERVICE_UNAVAILABLE:
		return "SERVICE_UNAVAILABLE";
		// TODO throw error when code is not valid
		// default:
		// 	throw;
		// 	break;
	}
}

void HTTPResponse::setVersion(std::string const &_version)
{
	this->version = _version;
}

void HTTPResponse::setStatus(Status const &_status)
{
	this->status = _status;
}

void HTTPResponse::setReason(std::string const &_reason)
{
	this->reason = _reason;
}

void HTTPResponse::addHeader(std::string const &_key, std::string const &_value)
{
	// TODO check if key is already in the map, or is the key "Cookie".
	std::map<std::string, std::string>::iterator it = headers.find(_key);
	if (it != headers.end())
		it->second = _value;
	else
		headers.insert(std::pair<std::string, std::string>(_key, _value));
}

void HTTPResponse::setBody(std::string const &_body)
{
	this->body = _body;
}

// Status const &HTTPResponse::getStatus() const
// {
// 	return this->status;
// }

std::string const &HTTPResponse::getReason() const
{
	return this->reason;
}

std::map<std::string, std::string> const &HTTPResponse::getHeaders() const
{
	return this->headers;
}

std::string const &HTTPResponse::getBody() const
{
	return this->body;
}

void HTTPResponse::GETHandler(std::string const &uri)
{
	std::cout << "went in GETHandler" << std::endl;
	if (uri.empty())
	{
		this->body = "";
		return;
	}
	if (uri.find("../") != std::string::npos && uri.find("/..") != std::string::npos)
	{
		this->body = "";
		return;
	}

	std::string path = "application" + uri;
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		// if (s.st_mode & S_IFDIR)
		// {
		//   // it's a directory
		// }
		if (s.st_mode & S_IFREG)
		{
			int len = s.st_size;
			if (!this->getResourse(path, len))
			{
				this->getDefaultResourse();
			}
		}
		else
		{

			//   something else
		}
	}
	else
	{
		this->getDefaultResourse();
		// this->GETHandler("error404/errorPage.html");
	}
	//   return "";
	// this->body = "";
}

void HTTPResponse::buildDefaultResponse()
{
	this->version = "HTTP/1.1";
	this->status = OK;
	this->reason = "OK";
	setDefaultBody();
	setDefaultHeaders();
}

void HTTPResponse::setDefaultHeaders()
{
	headers.insert(std::pair<std::string, std::string>("Content-Length", std::to_string(body.size())));
	headers.insert(std::pair<std::string, std::string>("Content-Type", "text/html"));
	headers.insert(std::pair<std::string, std::string>("Connection", "Keep-Alive"));
	headers.insert(std::pair<std::string, std::string>("Server", "mehdi's_webserv"));
}

void HTTPResponse::setDefaultBody()
{
	body = "<html><head><title>Test Title</title></head><body>Hello World!<br /></body></html>";
}
#include "Cout.hpp"
// !helper functions

bool HTTPResponse::getResourse(std::string const &path, int const &len)
{
	(void)len;
	// char contents[len + 1];
	std::ifstream file(path, std::ios::in | std::ios::binary);
	if (!file.is_open())
	{
		ERR("Open: %s", strerror(errno));
		return false;
	}

	std::ostringstream oss;
    oss << file.rdbuf();
    this->body = oss.str();

	this->addHeader("Content-Length", std::to_string(this->body.size()));
	std::string filetype = path.substr(path.find(".") + 1, path.size());
	if (filetype != "png")
		this->addHeader("Content-Type", "text/" + filetype);
	else
		this->addHeader("Content-Type", "image/" + path.substr(path.find(".") + 1, path.size()));
	// this->addHeader("Content-Type", "text/" + path.substr(path.find(".") + 1, path.size()));
	return true;
}

// bool HTTPResponse::getResourse(std::string const &path, int const &len)
// {
// 	char contents[len + 1];
// 	std::ifstream file;

// 	file.open(path, std::ios::in | std::ios::binary);
// 	if (!file.is_open())
// 	{
// 		ERR("Open: %s", strerror(errno));
// 		return false;
// 	}
// 	file.read(contents, len);
// 	contents[file.gcount()] = '\0';
// 	this->body = contents;
// 	this->addHeader("Content-Length", std::to_string(this->body.size()));
// 	std::string filetype = path.substr(path.find(".") + 1, path.size());
// 	if (filetype != "png")
// 		this->addHeader("Content-Type", "text/" + filetype);
// 	else
// 		this->addHeader("Content-Type", "image/" + path.substr(path.find(".") + 1, path.size()));
// 	// this->addHeader("Content-Type", "text/" + path.substr(path.find(".") + 1, path.size()));
// 	return true;
// }

void HTTPResponse::getDefaultResourse()
{
	struct stat s;
	std::string path = "application/error404/errorPage.html";
	if (stat(path.c_str(), &s) == 0)
	{
		int len = s.st_size;
		if (!this->getResourse(path, len))
		{
			std::cout << "unable to get resourse: " << path << std::endl;
		}
	}
	this->status = NOT_FOUND;
	this->reason = "NOT_FOUND";
	// this->addHeader("Content-Length", std::to_string(this->body.size()));
	// this->addHeader("Content-Type", "text/html");
}
#include <fcntl.h>
// int const &HTTPResponse::POSTHandler(HTTPRequest const &request)
// {
// 	// (void)request;
// 	// return ;
// 	std::cout << "POSTHandler" << std::endl;
// 	int pipe_to_cgi[2];
// 	int pipe_from_cgi[2];

// 	pipe(pipe_to_cgi);
// 	pipe(pipe_from_cgi);

// 	// Fork to create a child process for the CGI script
// 	pid_t pid = fork();
// 	if (pid == 0)
// 	{
// 		// Child process (CGI script)

// 		// Close unused pipe ends
// 		close(pipe_to_cgi[1]);
// 		close(pipe_from_cgi[0]);

// 		// Redirect standard input and output
// 		dup2(pipe_to_cgi[0], STDIN_FILENO);
// 		dup2(pipe_from_cgi[1], STDOUT_FILENO);

// 		// Execute the CGI script
// 		execl("application/cgiBin/login.sh", "application/cgiBin/login.sh", nullptr);

// 		// If execl fails
// 		perror("execl");
// 		exit(EXIT_FAILURE);
// 	}
// 	else if (pid > 0)
// 	{
// 		// Parent process (C++ server)

// 		// Close unused pipe ends
// 		close(pipe_to_cgi[0]);
// 		close(pipe_from_cgi[1]);

// 		// Write data to the CGI script
// 		// const char *dataToSend = "username=mehdi&password=mirzaie";
// 		if (write(pipe_to_cgi[1], request.getBody().c_str(), request.getBody().size()) < 0)
// 			std::cerr << errno << std::endl;

// 		close(pipe_to_cgi[1]);
// 		// Read data from the CGI script
// 		// std::cerr << "data was sent\n";
// 		// char buffer[1024];
// 		// ssize_t bytesRead;
// 		// this->body = "";
// 		// while ((bytesRead = read(pipe_from_cgi[0], buffer, sizeof(buffer))) > 0)
// 		// {
// 		// 	// std::cerr << buffer << std::endl;
// 		// 	this->body.append(buffer, bytesRead);
// 		// }
// 		// this->body.append("\0", 1);
// 		// this->addHeader("Content-Length", std::to_string(this->body.size()));
// 		// close(pipe_from_cgi[0]);
// 		// wait(nullptr);
// 		return (pipe_from_cgi[0]);
// 	}
// 	else
// 	{
// 		// Fork failed
// 		perror("fork");
// 		exit(EXIT_FAILURE);
// 	}
// 	return (-1);
// }
