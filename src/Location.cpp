#include "Location.hpp"

/*------------------------------------------*\
|              CONSTRUCTORS                  |
\*------------------------------------------*/

void Location::initMethodPermissions()
{
    _methodPermissions[GET] = false;
    _methodPermissions[POST] = false;
    _methodPermissions[PATCH] = false;
    _methodPermissions[PUT] = false;
    _methodPermissions[DELETE] = false;
    _methodPermissions[HEAD] = false;
    _methodPermissions[OPTIONS] = false;
    _methodPermissions[CONNECT] = false;
    _methodPermissions[TRACE] = false;
}

Location::Location()
{
	this->_path = "";
	this->_root = "";
	this->_index = "";
	this->_filePathPost = "";
    this->_autoIndex = false;
	this->_clientMaxBodySize = MAX_CONTENT_LENGTH;
    initMethodPermissions();
}

//PARAMETERISED CONSTRUCTOR: USING THE PATH.
Location::Location(std::string &path)
{
	this->_path = path;
	this->_root = "";
	this->_index = "";
	this->_filePathPost = "";
    this->_autoIndex = false;
	this->_clientMaxBodySize = MAX_CONTENT_LENGTH;
    initMethodPermissions();
}

Location::Location(const Location& other)
{
	this->_path = other._path;
	this->_root = other._root;
	this->_index = other._index;
	this->_filePathPost = other._filePathPost;
    this->_autoIndex = other._autoIndex;
	this->_clientMaxBodySize = other._clientMaxBodySize;
    this->_methodPermissions = other._methodPermissions;
    initMethodPermissions();
}

Location &Location::operator=(const Location &rhs)
{
	if (this != &rhs)
	{
	    this->_path = rhs._path;
	    this->_root = rhs._root;
	    this->_index = rhs._index;
	    this->_filePathPost = rhs._filePathPost;
        this->_autoIndex = rhs._autoIndex;
	    this->_clientMaxBodySize = rhs._clientMaxBodySize;
        this->_methodPermissions = rhs._methodPermissions;
    }
	return (*this);
}

Location::~Location(){}

/*------------------------------------------*\
|                 GETTERS                    |
\*------------------------------------------*/

std::string Location::getPath()
{
    return _path;
}

std::string Location::getRoot()
{
    return _root;
}

std::string Location::getIndex()
{
    return _index;
}

std::string Location::getFilePathPost()
{
    return _filePathPost;
}

std::map<enum e_HRM, bool> Location::getMethodPermissions()
{
    return _methodPermissions;
}

bool Location::getAutoIndex()
{
    return _autoIndex;
}

size_t Location::getClientMaxBodySize()
{
    return _clientMaxBodySize;
}

/*------------------------------------------*\
|                 SETTERS                    |
\*------------------------------------------*/

void Location::setPath(std::string newPath)
{
    _path = newPath;
}


	// if (ConfigFile::getTypePath(parametr) != 2)
	// 	throw ServerConfig::ErrorException("root of location");
	// this->_root = parametr;
void Location::setRoot(std::string newRoot)
{
    _root = newRoot;
}

void Location::setIndex(std::string newIndex)
{
    _index = newIndex;
}

void Location::setFilePathPost(std::string newFilePathPost)
{
    _filePathPost = newFilePathPost;
}

void Location::setMethodPermissions(std::map<enum e_HRM, bool> newPermissions)
{
    _methodPermissions[GET]     = newPermissions[GET];
    _methodPermissions[POST]    = newPermissions[POST];
    _methodPermissions[PATCH]   = newPermissions[PATCH];
    _methodPermissions[PUT]     = newPermissions[PUT];
    _methodPermissions[DELETE]  = newPermissions[DELETE];
    _methodPermissions[HEAD]    = newPermissions[HEAD];
    _methodPermissions[OPTIONS] = newPermissions[OPTIONS];
    _methodPermissions[CONNECT] = newPermissions[CONNECT];
    _methodPermissions[TRACE]   = newPermissions[TRACE];
}

void Location::setMethodPermission(enum e_HRM test, bool permissionState)
{
    _methodPermissions[test] = permissionState;
}

void Location::setAutoIndex(bool indexState)
{
    _autoIndex = indexState;
}

void Location::setClientMaxBodySize(std::string size_str)
{
	unsigned long body_size = 0;

	for (size_t i = 0; i < size_str.length(); i++)
	{
		if (size_str[i] < '0' || size_str[i] > '9')
			throw Location::ErrorException("Wrong syntax: client_max_body_size");
	}
	if (!std::atoi(size_str.c_str()))
		throw Location::ErrorException("Wrong syntax: client_max_body_size");
	body_size = std::atoi(size_str.c_str());
	this->_clientMaxBodySize = body_size;
}

void Location::setClientMaxBodySize(size_t newClientMaxBodySize)
{
    _clientMaxBodySize = newClientMaxBodySize;
}

/*------------------------------------------*\
|                 OTHER                      |
\*------------------------------------------*/
  
