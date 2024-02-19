#include "../inc/message.hpp"

Message::Message() { this->init(); }

Message::Message(char *request, int len)
{
	this->request = request;
	this->requestLen = len;
}

Message::~Message()
{
}

Message::Message(Message const &src)
{
	*this = src;
}

Message &Message::operator=(Message const &src)
{
	this->headers = src.headers;
	this->body = src.body;
	return *this;
}

void Message::init()
{
	this->request = NULL;
	this->requestLen = 0;
}

void Message::parseRequest()
{
	std::string key, value; // header, value

	std::stringstream ss(this->request);
	std::string line;

	std::getline(ss, line, '\n');
	/*
	first line will always have the request method
	e.g. GET, POST, PUT, and it is formated different to the rest.
	this will need to be
	*/
	std::stringstream line_stream(line);
	std::getline(line_stream, key, ' ');
	std::getline(line_stream, value, '\n');
	this->headers.insert(std::pair<std::string, std::string>(key, value));

	/*
	the rest of the headers have the same formate "key: value"
	so we can use the following while loop to add the key and value to the map.
	*/
	while (std::getline(ss, line, '\n'))
	{
		line_stream = std::stringstream(line);
		getline(line_stream, key, ':');
		std::getline(line_stream, value, '\n');
		this->headers.insert(std::pair<std::string, std::string>(key, value));
	}

	/*
	the rest of the request is the body, since we do not have cgi right now
	we will leave it empty.
	*/

	// std::unordered_map<std::string, std::string>::iterator it = headers.begin();
	// for (; it != headers.end(); ++it)
	// 	std::cout << it->first << ": " << it->second << std::endl;
}

void Message::parseRequest(std::string const &request, int requestLen)
{
	(void)requestLen;
	std::string key, value; // header, value

	std::stringstream ss(request);
	std::string line;

	std::getline(ss, line, '\n');

	/*
	first line will always have the request method
	e.g. GET, POST, PUT, and it is formated different to the rest.
	this will need to be
	*/
	std::stringstream line_stream(line);
	std::getline(line_stream, key, ' ');
	std::getline(line_stream, value, '\n');
	this->headers.insert(std::pair<std::string, std::string>(key, value));

	/*
	the rest of the headers have the same formate "key: value"
	so we can use the following while loop to add the key and value to the map.
	*/
	while (std::getline(ss, line, '\n'))
	{
		line_stream = std::stringstream(line);
		getline(line_stream, key, ':');
		std::getline(line_stream, value, '\n');
		this->headers.insert(std::pair<std::string, std::string>(key, value));
	}

	/*
	the rest of the request is the body, since we do not have cgi right now
	we will leave it empty.
	*/

	// std::unordered_map<std::string, std::string>::iterator it = headers.begin();
	// for (; it != headers.end(); ++it)
	// 	std::cout << it->first << ": " << it->second << std::endl;
}

// int main(void)
// {
// 	Message request("GET / HTTP/1.1\nHost: localhost:8080\nSec-Fetch-Site: none\nCookie: session_id=abc123\nConnection: keep-alive\nUpgrade-Insecure-Requests: 1\nSec-Fetch-Mode: navigate\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.2.1 Safari/605.1.15\nAccept-Language: en-AU,en;q=0.9\nSec-Fetch-Dest: document\nAccept-Encoding: gzip, deflate\n");
// 	return 0;
// }

std::string Message::getHeaderValue(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator it = this->headers.find(key);
	return (it != headers.end()) ? it->second : "";
}

std::string Message::getBody() const
{
	return this->body;
}

// // Method to serialize the message into a string
// std::string Message::toString() const
// {
// 	std::stringstream ss;
// 	// Serialize headers
// 	std::unordered_map<std::string, std::string>::const_iterator it;
// 	for (it = headers.begin(); it != headers.end(); ++it)
// 	{
// 		ss << it->first << ": " << it->second << "\r\n";
// 	}
// 	ss << "\r\n"; // Empty line separating headers and body
// 	ss << body;	  // Add message body
// 	return ss.str();
// }

// // Method to deserialize a string into a message
// void Message::fromString(const std::string &data)
// {
// 	std::istringstream ss(data);
// 	std::string line;

// 	// Parse headers
// 	while (std::getline(ss, line) && !line.empty())
// 	{
// 		size_t colonPos = line.find(':');
// 		if (colonPos != std::string::npos)
// 		{
// 			std::string key = line.substr(0, colonPos);
// 			std::string value = line.substr(colonPos + 2); // Skip ': ' after colon
// 			headers[key] = value;
// 		}
// 	}

// 	// Get the rest as the body
// 	body = ss.str();
// 	// Remove headers from the body
// 	body.erase(0, ss.tellg());
// }