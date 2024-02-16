// #include "HTTPResponse.hpp"

// HTTPResponse::HTTPResponse():
//   _request_method_name("NONE"),
//   _request_uri("/"),
//   _HTTP_version("HTTP/1.1"),
//   _Connection_type("keep-alive")
// {
// }


// HTTPResponse::HTTPResponse(HTTPResponse const &src)
// {
//   *this = src;
// }

// HTTPResponse& HTTPResponse::operator=(HTTPResponse const &other) {
//   _request_method_name = other._request_method_name;
//   _request_uri = other._request_uri;
//   _HTTP_version = other._HTTP_version;
//   _Connection_type = other._Connection_type;
//   return *this;
// }

// HTTPResponse::~HTTPResponse() {
// }


// void HTTPResponse::setMethod( std::string method ){
// 	this->_request_method_name = method;
// }
// void HTTPResponse::setUri( std::string uri ){
// 	this->_request_uri = uri;
// }
// void HTTPResponse::setVersion( std::string version ){
// 	this->_HTTP_version = version;
// }
// void HTTPResponse::setConnection( std::string connection ){
// 	this->_Connection_type = connection;
// }
// void HTTPResponse::setContentType( std::string content ){
// 	this->_content_type = content;
// }
// void HTTPResponse::setVersionState( std::string version ){
// 	this->_HTTP_version_state = version;
// }


// std::string HTTPResponse::getMethod( ) const{
// 	return ( this->_request_method_name );
// }

// std::string HTTPResponse::getUri( ) const{
// 	return ( this->_request_uri );
// }

// std::string HTTPResponse::getVersion( ) const{
// 	return ( this->_HTTP_version );
// }

// std::string HTTPResponse::getVersionState( ) const{
// 	return ( this->_HTTP_version_state );
// }

// std::string HTTPResponse::getConnection( ) const{
// 	return ( this->_Connection_type );
// }

// std::string HTTPResponse::getContentType( ) const{
// 	return ( this->_content_type );
// }


// std::string HTTPResponse::getFileContents( std::string filePath ) {
// 	std::ifstream file;

// 	if ( filePath == std::string() ) {
// 		file.open("application/error404/errorPage.html", std::ios::in | std::ios::binary);
// 		if (!file.is_open()) {
// 			std::cout << "could not file error page\n" << std::endl;
// 		}
// 		this->setVersionState("404 Not Found");
// 		this->setContentType("text/html");
// 	}
// 	else {
// 		file.open(filePath, std::ios::in | std::ios::binary);
// 		if (!file.is_open()) {
// 			std::cout << "could not file error page\n" << std::endl;
// 		}
// 		this->setVersionState(" 200 OK");
// 	}
// 	std::ostringstream file_contents;
//     file_contents << file.rdbuf();
// 	file.close();
// 	return file_contents.str();
// }

// #include <unistd.h>
// 		#include <fcntl.h>
// #include "ServerManager.hpp"

// // void HTTPResponse::generateResponse( int fd ) {
// // 	std::string fileContents;
// // 	// if (this->cgi == true)
// // 	if (this->getUri().compare("/cgiBin/login.sh") == 0)
// // 	{
// // 		this->setVersionState(" 200 OK");
// // 		// std::cerr << "going inside cgi\n";

// // 		// Create pipes for communication
// // 		int pipe_to_cgi[2];
// // 		int pipe_from_cgi[2];

// // 		pipe(pipe_to_cgi);
// // 		pipe(pipe_from_cgi);

// // 		// Fork to create a child process for the CGI script
// // 		pid_t pid = fork();

// // 		if (pid == 0) {
// // 			// Child process (CGI script)

// // 			// Close unused pipe ends
// // 			close(pipe_to_cgi[1]);
// // 			close(pipe_from_cgi[0]);

// // 			// Redirect standard input and output
// // 			dup2(pipe_to_cgi[0], STDIN_FILENO);
// // 			dup2(pipe_from_cgi[1], STDOUT_FILENO);

// // 			// Execute the CGI script
// // 			execl("cgiBin/login.sh", "cgiBin/login.sh", nullptr);

// // 			// If execl fails
// // 			perror("execl");
// // 			exit(EXIT_FAILURE);
// // 		} else if (pid > 0) {
// // 			// Parent process (C++ server)

// // 			// Close unused pipe ends
// // 			close(pipe_to_cgi[0]);
// // 			close(pipe_from_cgi[1]);

// // 			// Write data to the CGI script
// // 			EV_SET(&ev_set[3], pipe_to_cgi[1], EVFILT_WRITE, EV_ADD, 0, 0, NULL);
// // 			if (fcntl(pipe_to_cgi[1], F_SETFL, O_NONBLOCK) < 0) {
// // 				std::cout << RED << "fcntl error: closing: " << pipe_to_cgi[1] << std::endl;
// // 				close(pipe_to_cgi[1]);
// // 			}
// // 			// ev_set_count++;
// // 			const char* dataToSend = "username=mehdi&password=mirzaie";
// // 			if (write(pipe_to_cgi[1], dataToSend, strlen(dataToSend)) < 0)
// // 				std::cerr << errno << std::endl;

// // 			close(pipe_to_cgi[1]);
// // 			// Read data from the CGI script
// // 			// std::cerr << "data was sent\n";
// // 			char buffer[1024];
// // 			ssize_t bytesRead;
// // 			while ((bytesRead = read(pipe_from_cgi[0], buffer, sizeof(buffer))) > 0) {
// // 				// std::cerr << buffer << std::endl;
// // 				fileContents.append(buffer, bytesRead);
// // 			}
// // 			fileContents.append("\0", 1);
// // 			close(pipe_from_cgi[0]);
// // 			wait(nullptr);
// // 		} else {
// // 			// Fork failed
// // 			perror("fork");
// // 			exit(EXIT_FAILURE);
// // 		}

// // 	}
// // 	else
// // 		fileContents = getFileContents(getFileName(getUri()));
// // 	_response =
// // 			"HTTP/1.1 " + getVersionState() + "\r\n"
// // 			"Content-Type: " + getContentType() + "\r\n"
// // 			"Connection: " + getConnection() + "\r\n"
// // 			// "Set-Cookie: session_id=abc123; Path=/; HttpOnly \r\n"
// // 			"\r\n" + fileContents;

// // 	// std::cout << "HTTP/1.1 " + getVersionState() + "\\r\\n"
// // 	// 		"Content-Type: " + getContentType() + "\\r\\n"
// // 	// 		"Connection: " + getConnection() + "\\r\\n" << std::endl;
// // 	std::cout << "response sent to " << fd << std::endl;
// // 	send(fd, _response.c_str(), _response.length(), 0);
// // }

// void HTTPResponse::generateResponse( int fd ) {
// 	std::string fileContents;
// 	// if (this->cgi == true)
// 	if (this->getUri().compare("/cgiBin/login.sh") == 0)
// 	{
// 		this->setVersionState(" 200 OK");
// 		// std::cerr << "going inside cgi\n";

// 		// Create pipes for communication
// 		int pipe_to_cgi[2];
// 		int pipe_from_cgi[2];

// 		pipe(pipe_to_cgi);
// 		pipe(pipe_from_cgi);

// 		// Fork to create a child process for the CGI script
// 		pid_t pid = fork();

// 		if (pid == 0) {
// 			// Child process (CGI script)

// 			// Close unused pipe ends
// 			close(pipe_to_cgi[1]);
// 			close(pipe_from_cgi[0]);

// 			// Redirect standard input and output
// 			dup2(pipe_to_cgi[0], STDIN_FILENO);
// 			dup2(pipe_from_cgi[1], STDOUT_FILENO);

// 			// Execute the CGI script
// 			execl("cgiBin/login.sh", "cgiBin/login.sh", nullptr);

// 			// If execl fails
// 			perror("execl");
// 			exit(EXIT_FAILURE);
// 		} else if (pid > 0) {
// 			// Parent process (C++ server)

// 			// Close unused pipe ends
// 			close(pipe_to_cgi[0]);
// 			close(pipe_from_cgi[1]);

// 			// Write data to the CGI script
// 			// EV_SET(&ev_set[j], pipe_to_cgi[1], EVFILT_WRITE, EV_ADD, 0, 0, NULL);
// 			// if (fcntl(pipe_to_cgi[1], F_SETFL, O_NONBLOCK) < 0) {
// 			// 	std::cout << RED << "fcntl error: closing: " << pipe_to_cgi[1] << std::endl;
// 			// 	close(pipe_to_cgi[1]);
// 			// }
// 			const char* dataToSend = "username=mehdi&password=mirzaie";
// 			if (write(pipe_to_cgi[1], dataToSend, strlen(dataToSend)) < 0)
// 				std::cerr << errno << std::endl;

// 			close(pipe_to_cgi[1]);
// 			// Read data from the CGI script
// 			// std::cerr << "data was sent\n";
// 			char buffer[1024];
// 			ssize_t bytesRead;
// 			while ((bytesRead = read(pipe_from_cgi[0], buffer, sizeof(buffer))) > 0) {
// 				// std::cerr << buffer << std::endl;
// 				fileContents.append(buffer, bytesRead);
// 			}
// 			fileContents.append("\0", 1);
// 			close(pipe_from_cgi[0]);
// 			wait(nullptr);
// 		} else {
// 			// Fork failed
// 			perror("fork");
// 			exit(EXIT_FAILURE);
// 		}

// 	}
// 	else
// 		fileContents = getFileContents(getFileName(getUri()));
// 	_response =
// 			"HTTP/1.1 " + getVersionState() + "\r\n"
// 			"Content-Type: " + getContentType() + "\r\n"
// 			"Connection: " + getConnection() + "\r\n"
// 			// "Set-Cookie: session_id=abc123; Path=/; HttpOnly \r\n"
// 			"\r\n" + fileContents;

// 	// std::cout << "HTTP/1.1 " + getVersionState() + "\\r\\n"
// 	// 		"Content-Type: " + getContentType() + "\\r\\n"
// 	// 		"Connection: " + getConnection() + "\\r\\n" << std::endl;
// 	std::cout << "response sent to " << fd << std::endl;
// 	send(fd, _response.c_str(), _response.length(), 0);
// }


// std::string HTTPResponse::getFileName( std::string uri ) const {
// 	std::string dir = "application";
// 	std::string fullpath = dir + uri;
// 	std::string _default = "/";

// 	// if (this->cgi == true)
// 	if (uri.compare("/cgiBin/login.sh") == 0)
// 		return ("cgiBin/login.sh");
// 	if (uri.compare(_default) == 0)
// 		return dir + "/menu.html";

// 	if (access(fullpath.c_str(), F_OK) == 0)
// 		return fullpath;
// 	// TODO if !R_OK return 403 Forbidden.
// 	std::cout << uri << " not found.\n" << std::endl;
// 	return std::string();
// }