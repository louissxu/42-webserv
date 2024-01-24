#include "HTTPResponse.hpp"

HTTPResponse::HTTPResponse(): Message()
{
}

HTTPResponse::HTTPResponse(HTTPResponse& other)
{
  *this = other;
}

HTTPResponse& HTTPResponse::operator=(HTTPResponse& other) {
  if (this == &other)
		return *this;
  this->setMethod(other.getMethod());
  this->setUri(other.getUri());
  this->setVersion(other.getVersion());
  this->setConnection(other.getConnection());
  this->setContentType(other.getContentType());
  return *this;
}

HTTPResponse::~HTTPResponse() {
}

void HTTPResponse::generateResponse(){
  std::string filePath = getFileName( getUri() );
  if (filePath == "\0")
  {
    _response = getMethod() + "200 OK\r\n"
		"Content-Type: " + getContentType() + "\r\n"
		"Connection: " + getConnection() + "\r\n"
		"\r\n" + file_contents.str();
  }

	std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
		// TODO: send 404 error
        perror("Error opening file");
        return;
    }

    std::ostringstream file_contents;
    file_contents << file.rdbuf();

	_response =
		getMethod() + "200 OK\r\n"
		"Content-Type: " + getContentType() + "\r\n"
		"Connection: " + getConnection() + "\r\n"
		"\r\n" + file_contents.str();
        // "HTTP/1.1 200 OK\r\n"
        // "Content-Type: " + content_type + "\r\n"
        // "Connection: keep-alive" + "\r\n"
        // "\r\n" + file_contents.str();
}


std::string HTTPResponse::getFileName( std::string uri ) const {
	std::string dir = "documents";
	std::string fullpath = dir + uri;
	std::string _default = "/";

	if (uri.compare(_default) == 0)
		return dir + "/menu.html";

	if (access(fullpath.c_str(), F_OK))
		return fullpath;
	return nullptr;
}