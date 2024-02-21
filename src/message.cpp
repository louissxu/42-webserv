#include "Message.hpp"

Message::Message() : message(""), bufferSent(0) {}

Message::Message(const HTTPResponse &_resp) : message(""), bufferSent(0)
{
    this->serialize(_resp);
}

Message::~Message() {}

const std::string &Message::getMessage() const
{
    return this->message;
}

int Message::getMessageSize() const
{
    return this->message.size();
}

const int &Message::getBufferSent() const
{
	return this->bufferSent;
}

void Message::setBufferSent(int buffer)
{
	this->bufferSent = buffer;
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
    this->message += "\r\n";

    // Serialize body
    this->message += _resp.getBody();
	std::cout << this->message << std::endl;
}




// Message::Message(): message(""), bufferSend(0) {}

// Message::Message(HTTPResponse const &_resp): message(""), bufferSend(0)
// {
// 	this->serialize(_resp);
// }

// Message::~Message() {}

// std::string const &Message::getMessage() const
// {
// 	return this->message;
// }

// int const &Message::getBuffer() const
// {
// 	return this->bufferSend;
// }

// void Message::setBuffer(int const &size)
// {
// 	this->bufferSend = size;
// }

// void Message::serialize(HTTPResponse const &_resp)
// {
// 	std::cout << "\n";
// 	std::cout << _resp.getVersion();
// 	std::cout << "\n";
// 	std::cout << _resp.getStatusCode();
// 	std::cout << "\n";
// 	std::cout << _resp.getReason();
// 	std::cout << "\n";
// 	// message = "";
// 	this->message += _resp.getVersion() + " " + _resp.getStatus() + " " + _resp.getReason() + "\r\n";
// }

