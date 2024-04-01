#include "Client.hpp"

Client::Client() : _sockFD(0), _FDconnectedTo(-1), _bufferRead(0) {}

Client::Client(int _sockFD, int _FDConnectedTo, sockaddr_in _client_addr) : _sockFD(_sockFD), _client_addr(_client_addr), _FDconnectedTo(_FDConnectedTo)
{
	_bufferRead = 0;
}

Client::~Client()
{
	// delete _req;
}

int Client::getSockFD() const
{
	return this->_sockFD;
}

int Client::getSockFDconnectedTo() const
{
	return this->_FDconnectedTo;
}

sockaddr_in Client::getClinetAddr() const
{
	return this->_client_addr;
}

Message Client::getMessage() const
{
	return this->_message;
}

void Client::setMessage(Message const &src)
{
	this->_message = src;
}

int Client::getBufferRead() const
{
	return this->_bufferRead;
}

void Client::setBufferRead(int const &buffer)
{
	this->_bufferRead = buffer;
}

void Client::resetRecvMessage()
{
	_recvMessage = "";
}

void Client::appendRecvMessage(std::string const &message, int len)
{
	_recvMessage.append(message, len);
}

void Client::appendRecvMessage(char *message, int len)
{
	_recvMessage.append(message, len);
}

std::string const &Client::getRecvMessage() const
{
	return this->_recvMessage;
}

void Client::setPipeFrom(int *pipe)
{
	this->_pipe_in[0] = pipe[0];
	this->_pipe_in[1] = pipe[1];
}

void Client::setPipeTo(int *pipe)
{
	this->_pipe_out[0] = pipe[0];
	this->_pipe_out[1] = pipe[1];
}


void Client::setResquest(HTTPRequest req)
{
	this->_req = req;
}

HTTPRequest Client::getRequest() const
{
	return this->_req;
}