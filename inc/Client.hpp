#pragma once

#include <netinet/in.h>
#include <arpa/inet.h>

#include "Message.hpp"
#include "HTTPRequest.hpp"

class Client
{
private:
	int _sockFD;
	sockaddr_in _client_addr;
	int _FDconnectedTo;
	int _bufferRead;
	std::string _recvMessage;
	Message _message;
	HTTPRequest _req;

public:
	bool _isCgi;
	pid_t _Cgipid;
	int _pipe_in[2];
	int _pipe_out[2];
	Client();
	Client(int sockFD, int FDconnectedTo, sockaddr_in client_addr);
	~Client();

	// getters
	int getSockFD() const;
	int getSockFDconnectedTo() const;
	sockaddr_in getClinetAddr() const;
	Message getMessage() const;
	HTTPRequest getRequest() const;
	int getBufferRead() const;
	std::string const &getRecvMessage() const;

	// setters
	void setBufferRead(int const &buffer);
	void setResquest(HTTPRequest req);
	void setMessage(Message const &src);
	void resetRecvMessage();

	void appendRecvMessage(std::string const &message, int len);
	void appendRecvMessage(char *message, int len);

	void setPipeFrom(int pipe[2]);
	void setPipeTo(int pipe[2]);
};