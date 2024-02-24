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
Location::Location(const std::string & path)
{
    std::cout << "Location: path constructor: " << path << std::endl;
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

void Location::setDirective(const std::string& name, const std::string& value) {
  
  if (name == "root") 
  {
    _root = value;
  } 
  else if (name == "allow_methods")
  {
    setAllowMethods(value);
  } 
  else if (name == "index")
  {
    _index = value;
  } 
  else if (name == "autoindex")
  {
    setAutoIndex(value);
  }
  else if (name == "cgi_ext")
  {
    _cgi_ext = value;
  }
  else if (name == "cgi_path")
  {
    _cgi_path = value;
  }
  else if (name == "return")
  {
    _return = value;
  }
}


/*------------------------------------------*\
|                 TODO                       |
\*------------------------------------------*/

void Location::setAllowMethods(const std::string& methods) {
    std::map<std::string, e_HRM> methodMap;
    methodMap["GET"] = GET;
    methodMap["POST"] = POST;
    methodMap["PATCH"] = PATCH;
    methodMap["PUT"] = PUT;
    methodMap["DELETE"] = DELETE;
    methodMap["HEAD"] = HEAD;
    methodMap["OPTIONS"] = OPTIONS;
    methodMap["CONNECT"] = CONNECT;
    methodMap["TRACE"] = TRACE;

    std::istringstream iss(methods);
    std::string method;
    while (iss >> method) {
        std::map<std::string, e_HRM>::iterator it = methodMap.find(method);
        if (it != methodMap.end()) {
            Utils::setColour("green");
            std::cout << "Location: "<< getPath() <<" setting "<<it->first<<" to true." << std::endl;
            Utils::setColour("reset");
            _methodPermissions[it->second] = true;
        } else {
            std::cout << "Unknown method: " << method << std::endl;
        }
    }
}

void Location::setAutoIndex(std::string stateString)
{
    std::cout << "setAutoIndex called: " << stateString << std::endl;
    if (stateString == "on")
    {
        _autoIndex = true;
    }
    else
    {
        _autoIndex = false;
    }
}

/*------------------------------------------*\
|                 OTHER                      |
\*------------------------------------------*/

bool Location::isValidLocationDirective(const std::string &src) {
    static const std::string validDirectiveNames[] = {
        "cgi_ext",
        "cgi_path",
        "root",
        "allow_methods",
        "index",
        "autoindex",
        "return"
    };
    static const size_t numDirectives = sizeof(validDirectiveNames) / sizeof(validDirectiveNames[0]);
    for (size_t i = 0; i < numDirectives; ++i) {
        if (validDirectiveNames[i] == src) {
            return true;
        }
    }
    return false;
}

void Location::initLocationDirectives(ConfigParser &src)
{
    size_t i = 0;
    Utils::setColour("yellow");
    std::cout << "Location " << this->getPath() << ": adding: " <<std::endl;
    Utils::setColour("reset");
    std::vector< std::pair < std::string, std::string> > temp = src.get_directives();
    if (temp.empty())
    {
        return;
    }
    for(std::vector< std::pair < std::string, std::string> >::iterator it = temp.begin(); it != temp.end(); ++it)
    {
        Utils::setColour("yellow");
        std::cout << "Directive [" << i << "]: Key: <" << it->first << "> Value: <" << it->second << ">." << std::endl;
        Utils::setColour("reset");
        if (isValidLocationDirective(it->first))
        {
            this->setDirective(it->first, it->second);
        }
        else
        {
            Utils::setColour("magenta");
            std::cout << it->first << "is not a Location directive I am aware of.." << std::endl;
            Utils::setColour("reset");
        }
        i++;
    }
}