#include "Client.hpp"

Client::Client(int _sockFD, int _FDConnectedTo, sockaddr_in _client_addr): sockFD(_sockFD), client_addr(_client_addr), FDconnectedTo(_FDConnectedTo) {}

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

void Client::setMessage(Message const &src)
{
	this->message = src;	
}