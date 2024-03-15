#include "Message.hpp"

Message::Message() : message(""), bufferSent(0) {}

Message::Message(const std::string &_message) : message(_message), bufferSent(0) {}

Message::Message(Message const &src) { *this = src; };

Message &Message::operator=(Message const &src)
{
    // this->message.clear();
    this->message = src.message;
    this->bufferSent = src.bufferSent;
    return *this;
}

Message::Message(const HTTPResponse &_resp) : message(""), bufferSent(0)
{
    this->serialize(_resp);
}

Message::~Message() {}

const std::string &Message::getMessage() const
{
    return this->message;
}

int Message::size() const
{
    return this->message.size();
}

const int &Message::getBufferSent() const
{
    return this->bufferSent;
}

void Message::setMessage(std::string const &_message)
{
    this->message = _message;
}

void Message::setBufferSent(int buffer)
{
    this->bufferSent = buffer;
}

void Message::addBufferSent(int buffer)
{
    this->bufferSent += buffer;
}

void Message::serialize(const HTTPResponse &_resp)
{
    // Serialize status line
    this->message += _resp.getVersion() + " " + std::to_string(_resp.getStatusCode()) + " " + _resp.getReason() + "\r\n";

    // Serialize headers
    const std::map<std::string, std::string> &headers = _resp.getHeaders();
    std::map<std::string, std::string>::const_iterator header = headers.begin();
    for (; header != headers.end(); ++header)
    {
        this->message += header->first + ": " + header->second + "\r\n";
    }

    // Add a blank line to indicate end of headers
    if (_resp.getCgiStatus() == false)
        this->message += "\r\n";

    // Serialize body
    this->message += _resp.getBody();
}
