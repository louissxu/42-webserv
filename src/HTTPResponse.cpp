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

// ! error pages other than 404 are not loading.
/*
 * @brief: HTTPResponse(HTTPRequest const &_req)
 * Creates an appropriate HTTPResponse from a given HTTPRequest.
*/
/*
HTTPResponse::HTTPResponse(HTTPRequest const &_req)
{
	//std::cout << "HTTPResponse: Using server: " << std::endl;
	//_myServer.printState();

	buildDefaultResponse();
	switch (_req.getMethod())
	{
	case Method(GET):
		GETHandler(_req);
		return;
	case Method(POST):
		// if (_req.getHeader("Set-Cookie") != std::string())
		// 	headers.insert(std::pair<std::string, std::string>("Set-Cookie", _req.getHeader("Set-Cookie")));
		return;
	case Method(DELETE):
		DELETEHandler(_req);
	default:
		return;
	}
}
*/

/*
 * @brief: HTTPResponse(HTTPRequest const &_req)
 * Creates an appropriate HTTPResponse from a given HTTPRequest.
*/
HTTPResponse::HTTPResponse(HTTPRequest const &_req, Server &_myServer)
{
	_server = _myServer;
	buildDefaultResponse();

	int methodState = methodPermittedAtRoute(_req);
	if (methodState) // meaning the method is not allowed
	{
		this->getErrorResource(methodState);
		return;
	}

	const std::string uri = _req.getUri();
	_path = createFullPath(_req);
	WARN("FULL PATH SET TO: %s", _path.c_str());





	// _req.createPath()




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

std::string HTTPResponse::getStatus()
{
	switch (status)
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
		this->status = INTERNAL_SERVER_ERROR;
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

// std::string const &HTTPResponse::isRedirect(std::string const &uri)
// {
// 	return (_server.getReturnPath(uri) == std::string() ? std::string() : )
// 	// if (_server.getReturnPath(uri) != std::string())
// 	// {

// 	// }
// }

// bool HTTPResponse::isDirectory(std::string const &uri) {
//     // If the last character is "/", it's definitely a directory
//     if (!uri.empty() && uri.back() == '/') {
//         return true;
//     }

//     // Find the last occurrence of "/"
//     size_t lastSlashPos = uri.find_last_of("/");

//     // Now, let's find if there's a "." after the last "/"
//     size_t dotPos = uri.find_last_of(".");

//     // If there's no ".", or the last "." is before the last "/"
//     if (dotPos == std::string::npos || dotPos < lastSlashPos) {
//         // It's intended to be a directory (no file extension present in the last segment)
//         return true;
//     } else {
//         // There's a dot after the last slash, implying a file, not a directory
//         return false;
//     }
// }


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
	this->version = "HTTP/1.1";
	this->status = Status(301);
	this->reason = getStatus();
	addHeader("Location", redirectPath);
}

//void HTTPResponse::GETHandler(std::string const &uri)
void HTTPResponse::GETHandler(HTTPRequest const &_req)
{
	const std::string uri = _req.getUri();
	DEBUG("\tWent in GETHandler");


	// int methodState = methodPermittedAtRoute(_req);
	// if (methodState) // meaning the method is not allowed
	// {
	// 	this->getErrorResource(methodState);
	// 	return;
	// }
	//Checking if the uri is a directory?
	

	/*
	if (isDirectory(uri))
	{
		DEBUG("IS A DIRECTORY: %s", uri.c_str());
		Location	&myLocation = _server.getLocationByPath(_req.getUri());
		
		//Is this a location context within our server?
		if (myLocation != Location::NullLocation)
		{
			//DEBUG("LOCATION IS SET: %s", myLocation.getPath().c_str());
			//std::cout << YELLOW << "LOCATION IS SET: " << myLocation.getPath() << RESET << std::endl;
			//Is the index directive defined within said Location?
			//std::cout << YELLOW << "LOCATION:INDEX IS: " << myLocation.getIndex() << RESET << std::endl;
			//std::cout << YELLOW << "SERVER:INDEX IS: " << _server.getIndex() << RESET << std::endl;

			if (myLocation.getIndex() != "" && myLocation.getIndex() != _server.getIndex())
			{
				std::cout << GREEN << "LOCATION:INDEX HAS BEEN SET! " << RESET << std::endl;
			}



		}
		else
		{
			std::cout << RED << "LOCATION NOT SET: " << myLocation.getPath() << RESET << std::endl;
			//DEBUG("LOCATION NOT SET: %s", myLocation.getPath().c_str());
		}
		
	}
	*/	
		
		//  && myLocation.indexIsDefined())
		// {
		// 	DEBUG("INDEX DIRECTIVE IS DEFINED AT LOCATION: %s", myLocation.getPath().c_str());
		// }
		// else
		// {
		// 	DEBUG("INDEX DIRECTIVE IS DEFINED AT LOCATION: %s", myLocation.getPath().c_str());
		// }
		/*
		//If index is defined.
		if (indexIsDefined)
		{
			//return the index page that has been defined.
		}
		// if auto index is on.
		else if (autoIndex == on)
		{
			//Call auto index shit. (Louis pls).
		}
		else
		{
			//Throw some error page 404?
		}
		*/





	//	std::string path = "application" + uri;
	std::string path = _server.getRoot() + uri;
	struct stat s;

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

	if (uri == "/")
	{
		path = _server.getRoot() + _server.getIndex();
		//path = "application/src/index.html";
	}
	else if (uri == "/favicon.ico")
	{
		path = "application/assets/images/favicon.ico";
	}
	DEBUG("\tPATH == %s", path.c_str());
	if (uri.empty() || (uri.find("../") != std::string::npos && uri.find("/..") != std::string::npos))
	{
		this->body = "";
		return;
	}
	// if (uri.compare(1, 7, "cgi-bin") == 0 && access(path.c_str(), F_OK) != -1)
	// {
	// 	// if (uri.compare(8, uri.size(), "loogin.py") != 0)
	// 	// 	this->getErrorResource(403);
	// 	return;
	// }
	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
		{
		  // it's a directory
			if (isAutoIndexOn(_req))
			{
				this->makeDirectoryPage(path);
				return;
			}
			else {
				this->getErrorResource(403);
				return;
			}
		}
		if (s.st_mode & S_IFREG)
		{
			int len = s.st_size;
			if (!this->getResource(path, len))
			{
				this->getErrorResource(404);
			}
		}
		else
		{}
	}
	else
	{
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
		body = "<html><head><title>Deleting file</title></head><body>unable to delete " + req.getUri() + " <br /></body></html>";
	}
	else
		body = "<html><head><title>Deleting file</title></head><body>deleted " + req.getUri() + " <br /></body></html>";
	// body = "<html><head><title>Ha Ha</title></head><body>Sorry Bud, Delete is not allowed on this server :(\n Go hack some other Server!<br /></body></html>";
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

void HTTPResponse::getErrorResource(int errCode)
{
	DEBUG("went in getErrorResource attempting code: %d", errCode);
	struct stat s;
	std::string const filename = _server.getErrorPage(errCode);
	DEBUG("HTTPResponse: getting errorResource: %s", filename.c_str());
	std::string path = "application/src/error/" + filename;
	if (stat(path.c_str(), &s) == 0)
	{
		int len = s.st_size;
		if (!this->getResource(path, len))
		{
			ERR("unable to get error Resource: %s", path.c_str());
			return;
		}
	}
	// if (filename.compare(0, 2, "403"))
	this->status = Status(errCode);
	this->reason = getStatus();
	// else
	// 	this->status = NOT_FOUND;
}

void HTTPResponse::makeDirectoryPage(std::string path) {

	std::string server_root = "application";
	
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

	this->body = body;
	this->status = Status(200);
	addHeader("Content-Length", std::to_string(body.size()));
	std::cout << "Directory page made" << std::endl;
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

// bool HTTPResponse::isDirectory(std::string const &uri) {
//     // Find the last occurrence of "/"
//     size_t lastSlashPos = uri.find_last_of("/");

//     // Now, let's find if there's a "." after the last "/"
//     size_t dotPos = uri.find_last_of(".");

//     // If there's no ".", or the last "." is before the last "/"
//     if (dotPos == std::string::npos || dotPos < lastSlashPos) {
// 		DEBUG("IS A DIRECTORY!!------------------------------------------------");
//         // It's intended to be a directory (no file extension present in the last segment)
//         return true;
//     } else {
//         // There's a dot after the last slash, implying a file, not a directory
// 		DEBUG("IS NOT A DIRECTORY!!------------------------------------------------");
//         return false;
//     }
// }

std::string HTTPResponse::stripFileName(std::string const &reqUri)
{
    std::size_t found = reqUri.find_last_of("/");
    if (found == 0 || found == std::string::npos)
    {
        return "/";
    }
    else // if we find / further in the string.
    {
        return reqUri.substr(0, found); // Return the substring from the start of the string to the last "/"
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

// bool HTTPResponse::autoIndexPermittedAtRoute(HTTPRequest const &req)
// {
// 	Location	&myLocation = _server.getLocationByPath(req.getUri());
// 	if (!myLocation.isNull())
// 	{
// 		if (myLocation.getAutoIndex())	
// 			return true;
// 	}
// 	return false;
// 	// 	return false;
// 	// else if (myLocation.getAutoIndex())
// 	// 	return true;
// 	// return false;
// }

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
	DEBUG("Enter create Full path..");
	Location	&myLocation = _server.getLocationByPath(req.getUri());
	
	if (myLocation.isNull())
	{
		DEBUG("Null location.. returning: (_server.getRoot() (%s) + req.getUri()) (%s)\n", _server.getRoot().c_str(), req.getUri().c_str());
		return (_server.getRoot() + req.getUri());
	}
	else
	{
		if (myLocation.getRoot() != "" )
		{
			DEBUG("Location set, root set in. Returning: myLocation.getRoot() (%s) + req.getUri() (%s)\n", myLocation.getRoot().c_str(), req.getUri().c_str());
			return myLocation.getRoot() + req.getUri();
		}
		else
		{
			DEBUG("Location set, root not set. Returning (_server.getRoot() (%s) + req.getUri()) (%s)\n", _server.getRoot().c_str(), req.getUri().c_str());
			return (_server.getRoot() + req.getUri());
		}
	}
}
