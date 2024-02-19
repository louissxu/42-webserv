#include "HTTPResponse.hpp"

HTTPResponse::HTTPResponse()
{
	status = OK;
	// base = "./application";
}

HTTPResponse::HTTPResponse(std::string const &uri)
{
	this->init();
	std::ifstream file;

	if (access(uri.c_str(), F_OK) != 0)
	{
		if (errno == ENOENT)
			std::cerr << "Error: File not found" << std::endl;
		else if (errno == EACCES)
			std::cerr << "Error: Permission denied" << std::endl;
		else
			std::cerr << "Error: Unknown error" << std::endl;
		// exit(EXIT_FAILURE);
		status = NOTFOUND;
		file.open("application/error404/errorPage.html", std::ios::in | std::ios::binary);
		if (!file.is_open()) {
			std::cout << "could not file error page\n" << std::endl;
		}
		this->status = NOTFOUND;
	}
	else {
		file.open(uri, std::ios::in | std::ios::binary);
		if (!file.is_open()) {
			std::cout << "could not file error page\n" << std::endl;
		}
		this->status = OK;
	}
	std::ostringstream file_contents;
    file_contents << file.rdbuf();
	file.close();
	body = file_contents.str();
	
	// base = "./application";
}

void HTTPResponse::setFilePath(std::string const &uri)
{
	if (uri)
}

void HTTPResponse::init()
{
	// this->base = "./application";
	this->status = OK;
	this->bufferSend = 0;
}

// HTTPResponse HTTPResponse::serialize(HTTPRequest const &request)
// {

// }