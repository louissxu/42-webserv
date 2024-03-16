#include "HTTPResponse.hpp"

/*------------------------------------------*\
|               CONSTRUCTORS                 |
\*------------------------------------------*/

HTTPResponse::HTTPResponse()
{
	buildDefaultResponse();
}

HTTPResponse::HTTPResponse(std::string const &_version, Status const &_status, std::string const &_reason, std::map<std::string, std::string> const &_headers, std::string const &_body)
	: version(_version), status(_status), reason(_reason), headers(_headers), body(_body){}

HTTPResponse &HTTPResponse::operator=(HTTPResponse const &src)
{
	this->status = src.getStatusCode();
	this->reason = src.getReason();
	this->headers = src.getHeaders();
	this->body = src.getBody();
	return *this;
}

/*
 * @brief: HTTPResponse(HTTPRequest const &_req)
 * Creates an appropriate HTTPResponse from a given HTTPRequest.
*/
HTTPResponse::HTTPResponse(HTTPRequest const &_req)
{
	//std::cout << "HTTPResponse: Using server: " << std::endl;
	//_myServer.printState();

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


/*
 * @brief: HTTPResponse(HTTPRequest const &_req)
 * Creates an appropriate HTTPResponse from a given HTTPRequest.
*/
HTTPResponse::HTTPResponse(HTTPRequest const &_req, Server &_myServer)
{
	_server = _myServer;
	//std::cout << "HTTPResponse: Using server: " << std::endl;
	//_myServer.printState();

	//Check if we can call the current request in the requested location.
	if (!methodPermittedAtRoute(_req))
	{
		//Throw forbidden.

	}


	buildDefaultResponse();
	switch (_req.getMethod())
	{
		case Method(GET):
		{
			GETHandler(_req.getUri());
			return;
		}
		case Method(POST):
		{
			if (_req.getHeader("Set-Cookie") != std::string())
				headers.insert(std::pair<std::string, std::string>("Set-Cookie", _req.getHeader("Set-Cookie")));
			return;
		}
		case Method(DELETE):
		{
			DELETEHandler();
		}
		default:
			return;
	}
}

HTTPResponse::HTTPResponse(HTTPResponse const &src)
{
	*this = src;
}

//HTTPResponse::~HTTPResponse() {}

/*------------------------------------------*\
|                 GETTERS                    |
\*------------------------------------------*/

std::string const &HTTPResponse::getVersion() const
{
	return this->version;
}

Status const &HTTPResponse::getStatusCode() const
{
	return this->status;
}

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

/*------------------------------------------*\
|                 SETTERS                    |
\*------------------------------------------*/

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

/*------------------------------------------*\
|                BOOLEANS                    |
\*------------------------------------------*/

bool isValidURI(const std::string &uri) {
    // List of forbidden characters or patterns. Adjust according to your needs.
    const char* forbiddenChars = "<>{}|\\^~`";
    
    //Check for directory traversal attempts
    if (uri.find("../") != std::string::npos || uri.find("/..") != std::string::npos) {
        return false;
    }

    // Check for forbidden characters
    for (size_t i = 0; i < strlen(forbiddenChars); i++) {
        if (uri.find(forbiddenChars[i]) != std::string::npos) {
            return false;
        }
    }
    return true; // URI passes basic validation
}

/*------------------------------------------*\
|             METHOD HANDLERS                 |
\*------------------------------------------*/

void HTTPResponse::GETHandler(std::string const &uri)
{
	DEBUG("went in GETHandler");
//	std::string path = "application" + uri;
	std::string path = _server.getRoot() + uri;
	struct stat s;

	if (!isValidURI(uri))
	{
		this->geterrorResource(403);
		return;
	}

	if (uri == "/")
	{
		path = _server.getRoot() + _server.getIndex();
		//path = "application/src/index.html";
	}
	else if (uri == "/favicon.ico")
	{
		path = "application/assets/images/favicon.ico";
	}
	DEBUG("PATH == %s", path.c_str());
	if (uri.empty() || (uri.find("../") != std::string::npos && uri.find("/..") != std::string::npos))
	{
		this->body = "";
		return;
	}
	if (uri.compare(1, 7, "cgi-bin") == 0 && access(path.c_str(), F_OK) != -1)
	{
		if (uri.compare(8, uri.size(), "loogin.py") != 0)
			this->geterrorResource(403);
		return;
	}
	if (stat(path.c_str(), &s) == 0)
	{
		// if (s.st_mode & S_IFDIR)
		// {
		//   // it's a directory
		// }
		if (s.st_mode & S_IFREG)
		{
			int len = s.st_size;
			if (!this->getResource(path, len))
			{
				this->geterrorResource(404);
			}
		}
		else
		{}
	}
	else
	{
		this->geterrorResource(404);
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
	addHeader("Content-Length", std::to_string(body.size()));
	addHeader("Content-Type", "text/html");
	addHeader("Connection", "Keep-Alive");
	addHeader("Server", "Webserv");
}

void HTTPResponse::setDefaultBody()
{

	//body = getReasource("index.html");
	// body = "<html><head><title>Test Title</title></head><body>Hello World!<br /></body></html>";
}
// !helper functions

bool HTTPResponse::getResource(std::string const &path, int const &len)
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

	std::string filetype = path.substr(path.find("."), path.size());
	this->addHeader("Content-Type", MimeTypes::getMimeType(filetype));
	return true;
}

void HTTPResponse::geterrorResource(int errCode)
{
	DEBUG("went in getErrorResource attempting code: %d", errCode);
	struct stat s;
	std::string const filename = _server.getErrorPage(errCode);
	DEBUG("HTTPResponse: getting errorResource: %s", filename.c_str());
	std::string path = "application/error/" + filename;
	if (stat(path.c_str(), &s) == 0)
	{
		int len = s.st_size;
		if (!this->getResource(path, len))
		{
			ERR("unable to get error Resource: %s", path.c_str());
			return;
		}
	}
	if (filename.compare(0, 2, "403"))
		this->status = FORBIDDEN;
	else
		this->status = NOT_FOUND;
}

bool const &HTTPResponse::getCgiStatus() const
{
	return cgiStatus;
}

void HTTPResponse::setCgiStatus(bool _status)
{
	cgiStatus = _status;
}

/*------------------------------------------*\
|        METHOD-ROUTE VERIFICATION           |
\*------------------------------------------*/

bool HTTPResponse::locationIsConfigured(const std::string& reqUri) {

	DEBUG("Entered locationIsConfigured...")
	std::vector<Location>::const_iterator it = _server.getLocations().begin(); 
	std::vector<Location>::const_iterator end = _server.getLocations().end();

	DEBUG("Entered loop...")
    while (it != end) {
		std::cout << "Checking location: " << it->getPath() << "..." << std::endl;
        if (it->getPath() == reqUri) {
            return true; // There is an entry in the config file with the same path.
        }
		++it;
    }
    return false; 
}



bool HTTPResponse::methodPermittedAtRoute(HTTPRequest const &_req)
{

	std::cout << YELLOW << "This HTTPRequest is trying to make a ";
	switch (_req.getMethod())
	{
		case Method(GET):
		{
			std::cout << "GET request.." << std::endl;
			break;
		}
		case Method(POST):
		{
			std::cout << "POST request.." << std::endl;
			break;
		}
		case Method(DELETE):
		{
			std::cout << "DELETE request.." << std::endl;
			break;
		}
		default:
			return false;
	}
	std::cout << "At location: " << _req.getUri()<< RESET << std::endl;

	if (locationIsConfigured(_req.getUri()))
	{
		std::cout << "We have configured settings for this location..." << std::endl;
	}
	else
	{
		std::cout << "We have not configured settings for this location..." << std::endl;
	}
	//if (_req.getLocation().methodIsAllowed())
	return (true);
}