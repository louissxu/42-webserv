#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest():
  _request_method_name("NONE"),
  _request_uri("/"),
  _HTTP_version("HTTP/1.1")
{
}

HTTPRequest::HTTPRequest(std::string request):
  _request_method_name("NONE"),
  _request_uri("/"),
  _HTTP_version("HTTP/1.1")
{
  parseString(request);
}

HTTPRequest::HTTPRequest(HTTPRequest& other)
{
  *this = other;
}

HTTPRequest& HTTPRequest::operator=(HTTPRequest& other) {
  _request_method_name = other._request_method_name;
  _request_uri = other._request_uri;
  _HTTP_version = other._HTTP_version;
  return *this;
}

HTTPRequest::~HTTPRequest() {
}

void HTTPRequest::print() {
  std::cout << "---- Parsed HTTP Request Contents (rest was discarded) ----" << std::endl;
  std::cout << "HTTP Request" << std::endl;
  std::cout << "  Method name:  " << _request_method_name << std::endl;
  std::cout << "  URI:          " << _request_uri << std::endl;
  std::cout << "  HTTP version: " << _HTTP_version << std::endl;
  std::cout << "---- end of request ----" << std::endl;
}

void HTTPRequest::parseString(std::string str) {
  std::cout << std::endl << "---- Full Request Data is: ----" << std::endl;
  std::cout << str << std::endl;
  std::cout << std::endl << "---- End of request ----" << std::endl;

  std::stringstream ss(str);
  std::string line;

  std::getline(ss, line, '\n');

  std::stringstream line_stream(line);
  std::string item;

  std::getline(line_stream, item, ' ');
  _request_method_name = item;

  std::getline(line_stream, item, ' ');
  _request_uri = item;

  std::getline(line_stream, item, ' ');
  _HTTP_version = item;


  // std::stringstream first_line;
  // first_line << buff

  // first_line.getline(buff, 1024, " ");
  
  // _request_method_name = buff;
  
  // first_line.getline(buff, 1024, " ");
  // _request_uri = buff;

  // first_line.getline(buff, 1024, " ");
  // _HTTP_version = buff;
}
