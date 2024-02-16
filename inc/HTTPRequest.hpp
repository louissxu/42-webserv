// #pragma once

// #include "message.hpp"

// class HTTPRequest : public Message
// {
// public:
//   HTTPRequest();
//   HTTPRequest(std::string method, std::string uri);
//   HTTPRequest(HTTPRequest const &other);
//   HTTPRequest &operator=(HTTPRequest const &other);
//   ~HTTPRequest();

//   std::string const &getMethod() const;
//   std::string const &getUri() const;

// private:
//   std::string method;
//   std::string uri;
// };