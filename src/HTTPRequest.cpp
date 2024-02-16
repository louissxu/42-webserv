#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest() {}

HTTPRequest::HTTPRequest(std::string _method, std::string _uri) : method(_method), uri(_uri) {}

HTTPRequest::HTTPRequest(HTTPRequest const &other)
{
  *this = other;
}

HTTPRequest &HTTPRequest::operator=(HTTPRequest const &other)
{
  this->method = other.method;
  this->uri = other.uri;
  return *this;
}

HTTPRequest::~HTTPRequest()
{
}

// void HTTPRequest::print() {
//   // std::cout << "---- Parsed HTTP Request Contents (rest was discarded) ----" << std::endl;
//   std::cout << "\033[31m" << "HTTP Request" << "\033[0m" << std::endl;
//   std::cout << "  Method name:  " << _request_method_name << std::endl;
//   std::cout << "  URI:          " << _request_uri << std::endl;
//   std::cout << "  HTTP version: " << _HTTP_version << std::endl;
//   std::cout << "  Connection type: " << _Connection_type << std::endl;
//   std::cout << "  Content type: " << _content_type << std::endl;
//   std::cout << "\033[31m" << "---- end of request ----\n" << "\033[0m" << std::endl;
// }

// void HTTPRequest::parseString(std::string str) {

//   // std::cout << "\033[34m" << str << "\n" << "\033[0m" << std::endl;

//   std::stringstream ss(str);
//   std::string line;

//   std::getline(ss, line, '\n');

//   std::stringstream line_stream(line);
//   std::string item;

//   std::getline(line_stream, item, ' ');
//   _request_method_name = item;

//   if (_request_method_name.compare("POST") == 0)
//     cgi = true;
//   else
//     cgi = false;

//   std::getline(line_stream, item, ' ');
//   _request_uri = item;

//   std::getline(line_stream, item, '\\');
//   _HTTP_version = item;

//   while (std::getline(ss, line, '\n'))
//   {
//     std::stringstream line_stream1(line);
//     std::string item;
//     std::getline(line_stream1, item, ' ');
//     if (item == "Accept:") {
//       std::getline(line_stream1, item, ',');
//       _content_type = item;
//     }
//     else if (item == "connection:") {
//       std::getline(line_stream1, item, '\n');
//       _Connection_type = item;
//     }
//   }
//   // this->print();
//   // while (item.compare(0, 11, "Connection:"))
//   // {
//   //   std::getline(line_stream, item, '\n');
//   //   continue;
//   // }
//   // _Connection_type = "keep-alive";
// }
