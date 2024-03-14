#include "Client.hpp"

Client::Client() : sockFD(0), FDconnectedTo(-1), bufferRead(0)
{
	_last_msg_time = time(NULL);
}

Client::Client(int _sockFD, int _FDConnectedTo, sockaddr_in _client_addr) : sockFD(_sockFD), client_addr(_client_addr), FDconnectedTo(_FDConnectedTo)
{
	bufferRead = 0;
	_last_msg_time = time(NULL);
}

Client::~Client()
{
	// delete client_addr;
}

int Client::getSockFD() const
{
	return this->sockFD;
}

int Client::getSockFDconnectedTo() const
{
	return this->FDconnectedTo;
}

sockaddr_in Client::getClinetAddr() const
{
	return this->client_addr;
}

Message const &Client::getMessage() const
{
	return this->message;
}

void Client::setMessage(Message const &src)
{
	this->message = src;
}

int const &Client::getBufferRead() const
{
	return this->bufferRead;
}

void Client::setBufferRead(int const &buffer)
{
	this->bufferRead = buffer;
}

void Client::resetRecvMessage()
{
	recvMessage = "";
}

void Client::appendRecvMessage(std::string const &message, int len)
{
	recvMessage.append(message, len);
}

void Client::appendRecvMessage(char *message, int len)
{
	recvMessage.append(message, len);
}

std::string const &Client::getRecvMessage() const
{
	return this->recvMessage;
}

void Client::setPipeFrom(int *pipe)
{
	this->pipe_in[0] = pipe[0];
	this->pipe_in[1] = pipe[1];
}

void Client::setPipeTo(int *pipe)
{
	this->pipe_out[0] = pipe[0];
	this->pipe_out[1] = pipe[1];
}

const time_t     &Client::getLastTime() const
{
    return (_last_msg_time);
}

void Client::updateTime()
{
    _last_msg_time = time(NULL);
}