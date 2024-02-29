#include "cgi.hpp"

// https://github.com/php/php-src/blob/master/ext/session/session.c#L279
const int PS_MAX_SID_LENGTH = 32; // Define your maximum session ID length here

static std::string genSessionID(int sid_length, int sid_bits_per_character) {
    // Seed the random number generator
    srand(static_cast<unsigned int>(time(nullptr)));

    // Allocate memory for the session ID
    unsigned char rbuf[PS_MAX_SID_LENGTH];

    // Generate random bytes for the session ID
    for (int i = 0; i < sid_length; ++i) {
        rbuf[i] = rand() % 256; // Generate a random byte (0-255)
    }

    // Convert binary data to a readable string
    std::string outid;
    for (int i = 0; i < sid_length; ++i) {
        // Determine the character representing this byte
        char character = '0' + (rbuf[i] % sid_bits_per_character);

        // Append the character to the output string
        outid.push_back(character);
    }

    return outid;
}

Cgi::Cgi() {}

Cgi::~Cgi() {}

void Cgi::setEnv(HTTPRequest const &req)
{
	_env.push_back("Content-Type=" + req.getHeader("Content-Type"));
	_env.push_back("Content-Length=" + req.getHeader("Content-Length"));
	_env.push_back("User-Agent=" + req.getHeader("User-Agent"));
	_env.push_back("User-Agent=" + req.getHeader("User-Agent"));
	_env.push_back("Method=" + req.getMethodString());
	if (req.getMethod() == POST)
	{
		_env.push_back("session_id=" + genSessionID(32, 5));
	}
}

void Cgi::setArgv(HTTPRequest const &req)
{
	std::string temp = "application/cgi-bin" + req.getUri();

	_argv = (char **)malloc(sizeof(char *) * 3);
	_argv[0] = "/Library/Frameworks/Python.framework/Versions/3.10/bin/python3";
	_argv[1] = (char *)temp.c_str();
	_argv[2] = NULL;
}