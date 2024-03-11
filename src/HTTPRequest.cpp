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
  std::cout << "---- Parsed HTTP Request Contents ----" << std::endl;
  std::cout << "HTTP Request" << std::endl;
  std::cout << "  Method name:  " << GetHttpMethodAsString() << std::endl;
  std::cout << "  URI:          " << uri_ << std::endl;
  std::cout << "  HTTP version: " << GetHttpVersionAsString() << std::endl;
  std::cout << "  Headers: " << headers_.size() << " count" << std::endl;
  for (std::map<std::string, std::string>::iterator it = headers_.begin(); it != headers_.end(); ++it) {
    std::string key = it->first;
    std::string value = it->second;

    std::cout << "    " << key << " : " << value << "" << std::endl;
  }
  std::cout << "  Body:" << std::endl;
  std::cout << "    (start)" << std::endl;
  std::cout << body_ << std::endl;
  std::cout << "    (end)" << std::endl;
  std::cout << "  ParseState: " << parse_line_state_ << std::endl;
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
    if (str == "\r") {
      parse_line_state_ = kBody;
    } else {
      parseHeaderLine(str);
    }
  } else if (parse_line_state_ == kBody) {
    parseBodyLine(str);
    if (str == "" || str == "\r" || str == "\n") {
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

  if (headers_[key].substr(0, 1) == " ") {
    headers_[key] = headers_[key].substr(1);
  }
  
  char last_char = headers_[key][headers_[key].size() - 1];
  if (last_char == '\n' || last_char == '\r') {
    headers_[key].pop_back();
  }


}

void HTTPRequest::parseBodyLine(std::string str) {
  body_ = body_ + str;
}

std::string HTTPRequest::GetHttpMethodAsString() {
  switch (http_method_) {
    case kGet:
      return "GET";
    case kPost:
      return "POST";
    case kDelete:
      return "DELETE";
    default:
      // TODO Handle default value
      break;
  }
}

std::string HTTPRequest::GetHttpVersionAsString() {
  switch (http_version_) {
    case kHttp_1_1:
      return "HTTP/1.1";
    default:
      // TODO Handle default value
      break;
  }
}