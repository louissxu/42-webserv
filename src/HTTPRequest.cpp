#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest():
  parse_line_state_(kStartLine),
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
  parse_line_state_ = other.parse_line_state_;

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

void HTTPRequest::parseLine(std::string str) {
  if (parse_line_state_ == kStartLine) {
    parseStartLine(str);
    parse_line_state_ = kHeaders;
  } else if (parse_line_state_ == kHeaders) {
    parseHeaderLine(str);
    if (str == "") {
      parse_line_state_ = kBody;
    }
  } else if (parse_line_state_ == kBody) {
    parseBodyLine(str);
    if (str == "") {
      parse_line_state_ = kFinished;
    }
  } else {
    // TODO Out of range handling
    std::cout << "we're done" << std::endl;
  }
}

void HTTPRequest::parseStartLine(std::string str) {
  std::stringstream stream(str);

  char buff[301];

  memset(buff, '\0', 301);
  stream.getline(buff, 300, ' ');

  std::string method(buff);
  if (method == "GET") {
    http_method_ = kGet;
  } else if (method == "POST") {
    http_method_ = kPost;
  } else if (method == "DELETE") {
    http_method_ = kDelete;
  } else {
    // TODO better error handling
    throw std::runtime_error("unrecognised http method");
  }

  memset(buff, '\0', 301);
  stream.getline(buff, 300, ' ');
  uri_ = buff;

  memset(buff, '\0', 301);
  stream.getline(buff, 300);

  std::string version(buff);
  std::cout << "version: <" << version << ">" << std::endl;
  if (version == "HTTP/1.1\r" || version == "HTTP/1.1\n" || version == "HTTP/1.1") {
    http_version_ = kHttp_1_1;
  } else {
    // TODO better error handling
    throw std::runtime_error("unrecognised http version");
  }
}

void HTTPRequest::parseHeaderLine(std::string str) {
  std::stringstream stream(str);

  char buff[1001];
  memset(buff, '\0', 1001);
  stream.getline(buff, 1000, ':');
  std::string key(buff);
  headers_[key] = "";

  while (true) {
    memset(buff, '\0', 1001);
    stream.getline(buff, 1000);
    headers_[key] = headers_[key] + buff;

    if (stream.eof()) {
      break;
    }
  }
}

void HTTPRequest::parseBodyLine(std::string str) {
  body_ = body_ + str;
}