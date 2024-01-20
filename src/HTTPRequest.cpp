#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest(): Message()
{
}

HTTPRequest::HTTPRequest(std::string request)
{
  parseString(request);
}

HTTPRequest::HTTPRequest(HTTPRequest& other)
{
  *this = other;
}

HTTPRequest& HTTPRequest::operator=(HTTPRequest& other) {
  if (this == &other)
		return *this;
  this->setMethod(other.getMethod());
  this->setUri(other.getUri());
  this->setVersion(other.getVersion());
  this->setConnection(other.getConnection());
  this->setContentType(other.getContentType());
  return *this;
}

HTTPRequest::~HTTPRequest() {
}

void HTTPRequest::print() {
  // std::cout << "---- Parsed HTTP Request Contents (rest was discarded) ----" << std::endl;
  std::cout << "\033[31m" << "HTTP Request" << "\033[0m" << std::endl;
  std::cout << "  Method name:  " << _request_method_name << std::endl;
  std::cout << "  URI:          " << _request_uri << std::endl;
  std::cout << "  HTTP version: " << _HTTP_version << std::endl;
  std::cout << "  Connection type: " << _Connection_type << std::endl;
  std::cout << "\033[31m" << "---- end of request ----\n" << "\033[0m" << std::endl;
}

void HTTPRequest::parseString(std::string str) {

  std::stringstream ss(str);
  std::string line;

  std::getline(ss, line, '\n');

  std::stringstream line_stream(line);
  std::string item;

  std::getline(line_stream, item, ' ');
  _request_method_name = item;

  std::getline(line_stream, item, ' ');
  _request_uri = item;

  std::getline(line_stream, item, '\\');
  _HTTP_version = item;

  // while (item.compare(0, 11, "Connection:"))
  // {
  //   std::getline(line_stream, item, '\n');
  //   continue;
  // }
  _Connection_type = "keep-alive";
  static bool first = true;
  if (first)
  {
  _content_type = "text/html";
  first = false;
  }
  else
  {
    _content_type = "text/css";
    first = true;
  }
}


// void HTTPRequest::parseString(std::string str) {
//     std::stringstream ss(str);
//     std::string line;
    
//     std::getline(ss, line, '\n');

//     std::stringstream line_stream(line);
//     std::string item;

//     std::getline(line_stream >> std::ws, item, ' ');  // Trim leading whitespaces
//     _request_method_name = item;

//     std::getline(line_stream >> std::ws, item, ' ');  // Trim leading whitespaces
//     _request_uri = item;

//     std::getline(line_stream >> std::ws, item, ' ');  // Trim leading whitespaces
//     _HTTP_version = item;

//     // Find the "Connection:" header
//     std::getline(ss, line, '\n');
//     line_stream(line);
//     while (std::getline(line_stream, item) && item.find("Connection:") == std::string::npos) {
//         // Skip other headers
//     }
//     _Connection_type = item;
// }