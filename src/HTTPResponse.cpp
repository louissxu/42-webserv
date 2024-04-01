#include "HTTPResponse.hpp"

/*------------------------------------------*\
|               CONSTRUCTORS                 |
\*------------------------------------------*/

HTTPResponse::HTTPResponse(): _version("HTTP/1.1"), _status(OK), _reason("OK"), _cgiStatus(false)
{
	buildDefaultResponse();
}

HTTPResponse::HTTPResponse(std::string const &_version, Status const &_status, std::string const &_reason, std::map<std::string, std::string> const &_headers, std::string const &_body)
	: _version(_version), _status(_status), _reason(_reason), _headers(_headers), _body(_body){}

HTTPResponse &HTTPResponse::operator=(HTTPResponse const &src)
{
	this->_status = src.getStatusCode();
	this->_reason = src.getReason();
	this->_headers = src.getHeaders();
	this->_body = src.getBody();
	return *this;
}

/*
 * @brief: HTTPResponse(HTTPRequest const &_req)
 * Creates an appropriate HTTPResponse from a given HTTPRequest.
*/
HTTPResponse::HTTPResponse(HTTPRequest const &_req, Server &_myServer)
{
	_server = _myServer;
	const std::string uri = _req.getUri();

	buildDefaultResponse();
	int methodState = methodPermittedAtRoute(_req);
	if (methodState) // meaning the method is not allowed
	{
		this->getErrorResource(methodState);
		return;
	}

	// uri = _req.getUri();
	_path = createFullPath(_req);
	// WARN("FULL PATH SET TO: %s", _path.c_str());


	switch (_req.getMethod())
	{
		case Method(GET):
		{
			GETHandler(_req);
			return;
		}
		case Method(POST):
		{
			// if (_req.getHeader("Set-Cookie") != std::string())
			// 	headers.insert(std::pair<std::string, std::string>("Set-Cookie", _req.getHeader("Set-Cookie")));
			return;
		}
		case Method(DELETE):
		{
			DELETEHandler(_req);
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

std::string HTTPResponse::getVersion() const
{
	return this->_version;
}

Status HTTPResponse::getStatusCode() const
{
	return this->_status;
}

std::string HTTPResponse::getReason() const
{
	return this->_reason;
}

std::map<std::string, std::string> HTTPResponse::getHeaders() const
{
	return this->_headers;
}

std::string HTTPResponse::getBody() const
{
	return this->_body;
}

std::string HTTPResponse::getStatus()
{
	switch (_status)
	{
	case OK:
		return "OK";
	case CREATED:
		return "Created";
	case ACCEPTED:
		return "Accepted";
	case NON_AUTHORITATIVE_INFORMATION:
		return "Non-Authoritative Information";
	case NO_CONTENT:
		return "No Content";
	case MOVED_PERMANENTLY:
		return "Moved Permanently";
	case FOUND:
		return "Found";
	case BAD_REQUEST:
		return "Bad Request";
	case FORBIDDEN:
		return "Forbidden";
	case NOT_FOUND:
		return "Not Found";
	case METHOD_NOT_ALLOWED:
		return "Method Not Allowed";
	case CONTENT_TOO_LARGE:
		return "Content Too Large";
	case REQUEST_TIMEOUT:
		return "Request Timeout";
	case INTERNAL_SERVER_ERROR:
		return "Internal Server Error";
	case BAD_GATEWAY:
		return "Bad Gateway";
	case SERVICE_UNAVAILABLE:
		return "Service Unavailable";
		// TODO throw error when code is not valid
	default:
		this->_status = INTERNAL_SERVER_ERROR;
		return "Internal Server Error";
		break;
	}
}

std::string HTTPResponse::getMethodString(enum e_HRM method) const
{
	switch (method)
	{
	case r_POST:
		return "POST";
		break;
	case r_GET:
		return "GET";
		break;
	case r_DELETE:
		return "DELETE";
		break;
	default:
		return std::string();
		break;
	}
}

/*------------------------------------------*\
|                 SETTERS                    |
\*------------------------------------------*/

void HTTPResponse::setVersion(std::string const &_version)
{
	this->_version = _version;
}

void HTTPResponse::setStatus(Status const &_status)
{
	this->_status = _status;
}

void HTTPResponse::setReason(std::string const &_reason)
{
	this->_reason = _reason;
}

void HTTPResponse::addHeader(std::string const &_key, std::string const &_value)
{
	// TODO check if key is already in the map, or is the key "Cookie".
	std::map<std::string, std::string>::iterator it = _headers.find(_key);
	if (it != _headers.end())
		it->second = _value;
	else
		_headers.insert(std::pair<std::string, std::string>(_key, _value));
}

void HTTPResponse::setBody(std::string const &_body)
{
	this->_body = _body;
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

bool HTTPResponse::isDirectory(std::string const &uri) {
    // Directly return true if the URI ends with a "/"
    if (!uri.empty() && uri.back() == '/') {
        return true;
    }

    // Find the last occurrence of "/"
    size_t lastSlashPos = uri.find_last_of("/");

    // If there's no "/", it can't be a directory as per our current logic
    if (lastSlashPos == std::string::npos) {
        return false; // It might be just a plain string without any path
    }

    // Now, let's find if there's a "." after the last "/"
    size_t dotPos = uri.find_last_of(".");

    // If there's no ".", it's intended to be a directory
    if (dotPos == std::string::npos) {
        return true;
    }

    // If the last "." is before the last "/", it's also a directory
    if (dotPos < lastSlashPos) {
        return true;
    } else {
        // There's a dot after the last slash, implying a file, not a directory
        return false;
    }
}

void HTTPResponse::buildRedirectResponse(std::string const &redirectPath)
{
	this->_version = "HTTP/1.1";
	this->_status = Status(301);
	this->_reason = getStatus();
	addHeader("Location", redirectPath);
}

//void HTTPResponse::GETHandler(std::string const &uri)
void HTTPResponse::GETHandler(HTTPRequest const &_req)
{
	const std::string uri = _req.getUri();
	if (!isValidURI(uri))
	{
		this->getErrorResource(403);
		return;
	}

	// check for redirect path
	std::string redirectPath = _server.getReturnPath(uri);
	if (redirectPath != std::string())
	{
		buildRedirectResponse(redirectPath);
		return ;
	}

	if (uri.empty() || (uri.find("../") != std::string::npos && uri.find("/..") != std::string::npos))
	{
		// WARN("URI EMPTY/INVALID?");
		this->_body = "";
		return;
	}

	// WARN("RUNNING STAT WITH PATH: %s", _path.c_str());
	
	struct stat s;
	if (stat(_path.c_str(), &s) == 0)
	{
		//If its a file.
		if (s.st_mode & S_IFREG)
		{
			int len = s.st_size;
			//Try to serve the file. If fail, serve 404.
			if (!this->getResource(_path, len))
			{
				// WARN("FAILED TO GET RESOURCE AT %s, returning 404.", _path.c_str());
				this->getErrorResource(404);
			}
		}
		// If it's a directory
		else if (s.st_mode & S_IFDIR)
		{
			Location	&myLocation = _server.getLocationByPath(uri);
			if (myLocation.isNull())
			{
				// WARN("No location settings, returning 404: Location: %s", myLocation.getPath().c_str());
				this->getErrorResource(404);
			}

			//If index is set, serve it!
			if (!(myLocation.getIndex() == ""))
			{
				_path = _path + myLocation.getIndex();
				this->getResource(_path, 0);
				// WARN("_path Location + Index (is set!): Path = %s", _path.c_str());
			} 
			else
			{
				if (isAutoIndexOn(_req))
				{
					this->makeDirectoryPage(_path);
					return;
				}
				else {

					this->getErrorResource(403);
					return;
				}
			}
		}
		else
		{
			this->getErrorResource(505);
		}

	}
	else
	{
		// DEBUG("SOMETHING WENT WRONGG! VEEEENASAUURRR!");
		this->getErrorResource(404);
		// this->GETHandler("error404/errorPage.html");
	}
	//   return "";
	// this->body = "";

}

bool HTTPResponse::isAutoIndexOn(HTTPRequest const &req)
{
	Location	&myLocation = _server.getLocationByPath(req.getUri());

	if (myLocation.isNull())
	{
		return _server.isAutoIndex();
	}
	else
	{
		return myLocation.getAutoIndex();
	}
}

void HTTPResponse::DELETEHandler(const HTTPRequest &req)
{
	std::string path = _server.getRoot() + req.getUri();
	if (remove(path.c_str()) < 0)
	{
		_body = "<html><head><title>Deleting file</title></head><body>unable to delete " + req.getUri() + " <br /></body></html>";
	}
	else
		_body = "<html><head><title>Deleting file</title></head><body>deleted " + req.getUri() + " <br /></body></html>";
	// body = "<html><head><title>Ha Ha</title></head><body>Sorry Bud, Delete is not allowed on this server :(\n Go hack some other Server!<br /></body></html>";
	addHeader("Content-Length", std::to_string(_body.size()));
}

void HTTPResponse::buildDefaultResponse()
{
	this->_version = "HTTP/1.1";
	this->_status = OK;
	this->_reason = "OK";
	setDefaultBody();
	setDefaultHeaders();
}

void HTTPResponse::setDefaultHeaders()
{
	addHeader("Content-Length", std::to_string(_body.size()));
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
	this->_body = oss.str();

	this->addHeader("Content-Length", std::to_string(this->_body.size()));

	std::string filetype = path.substr(path.find("."), path.size());
	this->addHeader("Content-Type", MimeTypes::getMimeType(filetype));
	return true;
}

void HTTPResponse::getErrorResource(int errCode)
{
	std::string const filename = _server.getErrorPage(errCode);
	std::string path = "application/src/error/" + filename;
	struct stat s;

	if (stat(path.c_str(), &s) == 0)
	{
		int len = s.st_size;
		if (!this->getResource(path, len))
		{
			ERR("unable to get error Resource: %s", path.c_str());
			return;
		}
	}
	this->_status = Status(errCode);
	this->_reason = getStatus();
}

void HTTPResponse::makeDirectoryPage(std::string path) {

	std::string server_root = _server.getRoot();
	
	std::string body =
	"<html>"
	"<body>"
	"<h1>Directory</h1>"
	"<h2>Path: " + path + "</h2>"
	"<h2>Files:</h2>"
	"<ul>";

	DIR *d;
	struct dirent *dir;
	d = opendir(path.c_str());
	if (d) {
		size_t file_count = 0;
		while ((dir = readdir(d)) != NULL) {
			std::string file_name = dir->d_name;
			if (file_name[file_name.size() - 1] == '.') {
				continue;
			}
			std::string link = path.substr(server_root.size()) + "/" + file_name;
			body.append("<li><a href='" + link + "'>" + file_name + "</a></li>");
			file_count++;
		}
		if (file_count <= 0) {
			body.append("<li>Empty directory</li>");
		}
		closedir(d);
	} else {
		body.append("<li>No items</li>");
	}

	body.append(
	"</ul>"
	"</body>"
	"</html>"
	);

	this->_body = body;
	this->_status = Status(200);
	addHeader("Content-Length", std::to_string(body.size()));
	std::cout << "Directory page made" << std::endl;
}

bool HTTPResponse::getCgiStatus() const
{
	return _cgiStatus;
}

void HTTPResponse::setCgiStatus(bool _status)
{
	_cgiStatus = _status;
}

/*------------------------------------------*\
|        METHOD-ROUTE VERIFICATION           |
\*------------------------------------------*/

std::string HTTPResponse::stripFileName(std::string const &reqUri)
{
    std::size_t found = reqUri.find_last_of("/");
    if (found == 0 || found == std::string::npos)
    {
        return "/";
    }
    else // if we find / further in the string.
    {
        return reqUri.substr(0, found + 1); // Return the substring from the start of the string to the last "/"
    }
}

bool HTTPResponse::getMethodPermission(enum e_HRM method, Location &Location) const {

	if (Location.getMethodPermission(method))
	{
		DEBUG("Method is allowed at location: %s", Location.getPath().c_str());
		return true;
	}
	else
	{
		DEBUG("Method is NOT allowed at location: %s", Location.getPath().c_str());
		return false;
	}
}

int HTTPResponse::methodPermittedAtRoute(HTTPRequest const &req)
{
	e_HRM		myMethod;

	switch (req.getMethod())
	{
		case Method(GET):
		{
			myMethod = r_GET;
			DEBUG("\tThis HTTPRequest is trying to make a GET request..");
			break;
		}
		case Method(POST):
		{
			myMethod = r_POST;
			DEBUG("\tThis HTTPRequest is trying to make a POST request..");
			break;
		}
		case Method(DELETE):
		{
			myMethod = r_DELETE;
			DEBUG("\tThis HTTPRequest is trying to make a DELETE request..");
			break;
		}
		default:
			return 403;
	}
	std::string strippedUri = stripFileName(req.getUri().c_str());
	DEBUG("\tAt location: %s", strippedUri.c_str());

	Location	&myLocation = _server.getLocationByPath(strippedUri);
	if (myLocation.isNull()) {
		DEBUG("\tNo settings for this location. Defaulting to server default permissions.");
		if (!_server.getMethodPermission(myMethod))
		{
			// setAllowedPermissions()
			addHeader("Allow", ""); // since the default permissions are NONE
			return 405;
		}
		else
		{
			return 0;
		}
	}
	else if (!myLocation.getMethodPermission(myMethod))
	{
		DEBUG("\tWe have configured settings for this location...not allowed");
		addHeader("Allow", myLocation.getAllowedMethods());
		return 405;
	}
	//Successful request, return 0.
	return 0;
}

/*------------------------------------------*\
|               ROOT SETTING                 |
\*------------------------------------------*/


/*
 * @brief: createFullPath
 * Takes a string and a server and creates a file path. If the request does not correspond to a known location,
 * or _root has not been set in said location, use default/server _root instead.
*/

std::string HTTPResponse::createFullPath(HTTPRequest const &req)
{
	DEBUG("\tEnter create Full path..");
	Location	&myLocation = _server.getLocationByPath(req.getUri());
	
	if (myLocation.isNull())
	{
		DEBUG("\tNull location.. returning: (_server.getRoot() (%s) + req.getUri()) (%s)\n", _server.getRoot().c_str(), req.getUri().c_str());
		return (_server.getRoot() + req.getUri());
	}
	else
	{
		if (myLocation.getRoot() != "" )
		{
			DEBUG("\tLocation set, root set in. Returning: myLocation.getRoot() (%s) + req.getUri() (%s)\n", myLocation.getRoot().c_str(), req.getUri().c_str());
			return myLocation.getRoot() + req.getUri();
		}
		else
		{
			DEBUG("\tLocation set, root not set. Returning (_server.getRoot() (%s) + req.getUri()) (%s)\n", _server.getRoot().c_str(), req.getUri().c_str());
			return (_server.getRoot() + req.getUri());
		}
	}
}
