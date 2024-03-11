#pragma once
#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <map>

enum ParseLineState {
  kStartLine,
  kHeaders,
  // kEmptyLine,
  kBody,
  kFinished,
};

enum HttpMethod {
  kGet,
  kPost,
  kDelete,
};

enum HttpVersion {
  kHttp_1_1,
};

class HTTPRequest {
  public:
    HTTPRequest();
    HTTPRequest(std::string request);
    HTTPRequest(HTTPRequest& other);
    HTTPRequest& operator=(HTTPRequest& other);
    ~HTTPRequest();

    void parseLine(std::string str);
    void print();
    bool isComplete();

  private:
    void parseStartLine(std::string str);
    void parseHeaderLine(std::string str);
    void parseBodyLine(std::string str);

    std::string GetHttpMethodAsString();
    std::string GetHttpVersionAsString();

    enum ParseLineState parse_line_state_;

    enum HttpMethod http_method_;
    std::string uri_;
    enum HttpVersion http_version_;

    std::map<std::string, std::string> headers_;
    std::string body_;
};

#endif