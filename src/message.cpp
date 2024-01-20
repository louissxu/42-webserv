#include "message.hpp"

Message::Message():
  _request_method_name("NONE"),
  _request_uri("/"),
  _HTTP_version("HTTP/1.1"),
  _Connection_type("keep-alive")
{
}


Message::Message(Message const &src)
{
  *this = src;
}

Message& Message::operator=(Message const &other) {
  _request_method_name = other._request_method_name;
  _request_uri = other._request_uri;
  _HTTP_version = other._HTTP_version;
  _Connection_type = other._Connection_type;
  return *this;
}

Message::~Message() {
}


void Message::setMethod( std::string method ){
	this->_request_method_name = method;
}
void Message::setUri( std::string uri ){
	this->_request_uri = uri;
}
void Message::setVersion( std::string version ){
	this->_HTTP_version = version;
}
void Message::setConnection( std::string connection ){
	this->_Connection_type = connection;
}
void Message::setContentType( std::string content ){
	this->_content_type = content;
}

std::string Message::getMethod( ) const{
	return ( this->_request_method_name );
}

std::string Message::getUri( ) const{
	return ( this->_request_uri );
}

std::string Message::getVersion( ) const{
	return ( this->_HTTP_version );
}

std::string Message::getConnection( ) const{
	return ( this->_Connection_type );
}

std::string Message::getContentType( ) const{
	return ( this->_content_type );
}


void Message::generateResponse(int fd){
	std::string filePath = getFileName( getUri() );
	std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
		// TODO: send 404 error
        perror("Error opening file");
        return;
    }

    std::ostringstream file_contents;
    file_contents << file.rdbuf();
	
	std::cout << getVersion() + " 200 OK\\r\\n"
		"Content-Type: " + getContentType() + "\\r\\n"
		"Connection: " + getConnection() + "\\r\\n"
		"\\r\\n" + file_contents.str() << std::endl;

	_response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: " + getContentType() + "\r\n"
		"Connection: " + getConnection() + "\r\n"
		"\r\n" + file_contents.str();
        // "HTTP/1.1 200 OK\r\n"
        // "Content-Type: " + content_type + "\r\n"
        // "Connection: keep-alive" + "\r\n"
        // "\r\n" + file_contents.str();
	// send_file(fd, filePath, "text" + getUri());
	send(fd, _response.c_str(), _response.length(), 0);
	file.close();
}


std::string Message::getFileName( std::string uri ) const {
	std::string dir = "webpages";
	std::string fullpath = dir + uri;
	std::string _default = "/";

	if (uri.compare(_default) == 0)
		return dir + "/menu.html";
	
	if (access(fullpath.c_str(), F_OK) == 0)
		return fullpath;
	return nullptr;
}