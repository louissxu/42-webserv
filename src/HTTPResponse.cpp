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

HTTPResponse::HTTPResponse(HTTPRequest const &_req)
{
	buildDefaultResponse();
	switch (_req.getMethod())
	{
	case Method(GET):
		GETHandler(_req.getUri());
		return;
	case Method(POST):
		if (_req.getHeader("Set-Cookie") != std::string())
			headers.insert(std::pair<std::string, std::string>("Set-Cookie", _req.getHeader("Set-Cookie")));
		return;
	case Method(DELETE):
		DELETEHandler();
	default:
		return;
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
	DEBUG("went in GETHandler");
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

void HTTPResponse::DELETEHandler()
{
	body = "<html><head><title>Ha Ha</title></head><body>Sorry Bud, Delete is not allowed on this server :(\n Go hack some other Server!<br /></body></html>";
	addHeader("Content-Length", std::to_string(body.size()));
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
	return true;
}

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
}