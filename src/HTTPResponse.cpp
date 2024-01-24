#include "HTTPResponse.hpp"

HTTPResponse::HTTPResponse():
  _request_method_name("NONE"),
  _request_uri("/"),
  _HTTP_version("HTTP/1.1"),
  _Connection_type("keep-alive")
{
}


HTTPResponse::HTTPResponse(HTTPResponse const &src)
{
  *this = src;
}

HTTPResponse& HTTPResponse::operator=(HTTPResponse const &other) {
  _request_method_name = other._request_method_name;
  _request_uri = other._request_uri;
  _HTTP_version = other._HTTP_version;
  _Connection_type = other._Connection_type;
  return *this;
}

HTTPResponse::~HTTPResponse() {
}


void HTTPResponse::setMethod( std::string method ){
	this->_request_method_name = method;
}
void HTTPResponse::setUri( std::string uri ){
	this->_request_uri = uri;
}
void HTTPResponse::setVersion( std::string version ){
	this->_HTTP_version = version;
}
void HTTPResponse::setConnection( std::string connection ){
	this->_Connection_type = connection;
}
void HTTPResponse::setContentType( std::string content ){
	this->_content_type = content;
}
void HTTPResponse::setVersionState( std::string version ){
	this->_HTTP_version_state = version;
}


std::string HTTPResponse::getMethod( ) const{
	return ( this->_request_method_name );
}

std::string HTTPResponse::getUri( ) const{
	return ( this->_request_uri );
}

std::string HTTPResponse::getVersion( ) const{
	return ( this->_HTTP_version );
}

std::string HTTPResponse::getVersionState( ) const{
	return ( this->_HTTP_version_state );
}

std::string HTTPResponse::getConnection( ) const{
	return ( this->_Connection_type );
}

std::string HTTPResponse::getContentType( ) const{
	return ( this->_content_type );
}


std::string HTTPResponse::getFileContents( std::string filePath ) {
	std::ifstream file;

	if ( filePath == std::string() ) {
		file.open("webpages/error404/errorPage.html", std::ios::in | std::ios::binary);
		if (!file.is_open()) {
			std::cout << "could not file error page\n" << std::endl;
		}
		this->setVersionState("404 Not Found");
		this->setContentType("text/html");
	}
	else {
		file.open(filePath, std::ios::in | std::ios::binary);
		if (!file.is_open()) {
			std::cout << "could not file error page\n" << std::endl;
		}
		this->setVersionState(" 200 OK");
	}
	std::ostringstream file_contents;
    file_contents << file.rdbuf();
	file.close();
	return file_contents.str();
}

void HTTPResponse::generateResponse( int fd ) {
	// ! print our HTTPResponse
	std::string fileContents = getFileContents(getFileName(getUri()));
	// std::cout << "HTTP/1.1 " + getVersionState() + "\\r\\n"
	// 	"Content-Type: " + getContentType() + "\\r\\n"
	// 	"Connection: " + getConnection() + "\\r\\n" << std::endl;

	_response =
		"HTTP/1.1 " + getVersionState() + "\r\n"
		"Content-Type: " + getContentType() + "\r\n"
		"Connection: " + getConnection() + "\r\n"
		"\r\n" + fileContents;

	send(fd, _response.c_str(), _response.length(), 0);
}


std::string HTTPResponse::getFileName( std::string uri ) const {
	std::string dir = "webpages";
	std::string fullpath = dir + uri;
	std::string _default = "/";

	if (uri.compare(_default) == 0)
		return dir + "/menu.html";

	if (access(fullpath.c_str(), F_OK | R_OK) == 0)
		return fullpath;
	// TODO if !R_OK return 403 Forbidden.
	std::cout << uri << " not found.\n" << std::endl;
	return std::string();
}