#pragma once
#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>

/*
	TODO add later:
	Server: AMAnix\r\n
	Date: Fri, 19 Jan 2024 05:55:55 UTC\r\n\r\n
*/

class Message {
	protected:
		std::string _request_method_name;
		std::string _request_uri;
		std::string _HTTP_version;
		std::string _Connection_type;
		std::string _content_type;

		
		// std::string Response();
		// std::string getFileContents();
		std::string _response;
	
	public:
		Message();
		virtual ~Message();
		Message( Message const &src );
		Message &operator=( Message const &src );

		void setMethod( std::string method );
		void setUri( std::string method );
		void setVersion( std::string method );
		void setConnection( std::string method );
		void setContentType( std::string method );

		std::string getMethod( ) const;
		std::string getUri( ) const;
		std::string getVersion( ) const;
		std::string getConnection( ) const;
		std::string getContentType( ) const;

		void generateResponse(int fd);
		std::string getFileName( std::string uri ) const;
};		

#endif