#pragma once

#include <netinet/in.h>
#include <arpa/inet.h>

class Client
{
	private:
		int sockFD;
		sockaddr_in client_addr;
		int FDconnectedTo;

	public:
		Client(int _sockFD, int _FDconnectedTo, sockaddr_in _client_addr);
		~Client();

		int getSockFD() const;
		int getSockFDconnectedTo() const;
		sockaddr_in getClinetAddr() const;
};