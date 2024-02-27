#pragma once

#include <netinet/in.h>
#include <arpa/inet.h>

// #include "HTTPResponse.hpp"
#include "Message.hpp"

class Client
{
private:
	int sockFD;
	sockaddr_in client_addr;
	int FDconnectedTo;
	int bufferRead;
	std::string recvMessage;
	Message message;

public:
	int pipe_in[2];
	int pipe_out[2];
	Client();
	Client(int _sockFD, int _FDconnectedTo, sockaddr_in _client_addr);
	~Client();

	// void areFdsOpen(void);

	int getSockFD() const;
	int getSockFDconnectedTo() const;
	sockaddr_in getClinetAddr() const;
	Message const &getMessage() const;

	void setMessage(Message const &src);

	int const &getBufferRead() const;
	void setBufferRead(int const &buffer);

	void resetRecvMessage();
	void appendRecvMessage(std::string const &message, int len);
	void appendRecvMessage(char *message, int len);
	std::string const &getRecvMessage() const;

	void setPipeFrom(int pipe[2]);
	void setPipeTo(int pipe[2]);
};