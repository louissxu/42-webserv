#include "Location.hpp"
Location Location::NullLocation;
/*------------------------------------------*\
|              CONSTRUCTORS                  |
\*------------------------------------------*/


void Location::initMethodPermissions()
{
    _methodPermissions[r_GET] = false;
    _methodPermissions[r_POST] = false;
    _methodPermissions[r_DELETE] = false;
}

void Location::initMethodPermissions(std::map<enum e_HRM, bool> srcPermissions)
{
    _methodPermissions[r_GET] = false;
    _methodPermissions[r_POST] = false;
    _methodPermissions[r_DELETE] = false;

    for (std::map<enum e_HRM, bool>::const_iterator it = srcPermissions.begin(); it != srcPermissions.end(); ++it) {
        _methodPermissions[it->first] = it->second;
    }
}

Location::Location()
{
    DEBUG("\t\tDefault constructor called.");

    //std::cout << RED << "Location\t\t: " << RESET
    //<<"Default constructor called." << std::endl;
	this->_path = "";
	this->_root = "";
	this->_index = "";
	this->_filePathPost = "";
    this->_autoIndex = false;
	this->_clientMaxBodySize = MAX_CONTENT_LENGTH;
    initMethodPermissions();
}

//PARAMETERISED CONSTRUCTOR: USING THE PATH AND SERVER PERMISSIONS.
Location::Location(const std::string & path, std::map<enum e_HRM, bool> srcPermissions)
{
    DEBUG("\t\tParam constructor called.");
    // std::cout << RED << "Location: " << RESET
    // <<"path constructor called: "<< path << std::endl;

	this->_path = path;
	this->_root = "";
	this->_index = "";
    this->_return = "";
	this->_filePathPost = "";
    this->_autoIndex = false;
	this->_clientMaxBodySize = MAX_CONTENT_LENGTH;
    initMethodPermissions(srcPermissions);
}

Location::Location(const Location& other)
{
    DEBUG("\t\tCopy constructor called.");
	this->_path = other._path;
	this->_root = other._root;
	this->_index = other._index;
    this->_return = other._return;
	this->_filePathPost = other._filePathPost;
    this->_autoIndex = other._autoIndex;
	this->_clientMaxBodySize = other._clientMaxBodySize;
    this->_methodPermissions = other._methodPermissions;
}

Location &Location::operator=(const Location &rhs)
{
    DEBUG("\t\tEquals override constructor called.");
	if (this != &rhs)
	{
	    this->_path = rhs._path;
	    this->_root = rhs._root;
	    this->_index = rhs._index;
        this->_return = rhs._return;
	    this->_filePathPost = rhs._filePathPost;
        this->_autoIndex = rhs._autoIndex;
	    this->_clientMaxBodySize = rhs._clientMaxBodySize;
        this->_methodPermissions = rhs._methodPermissions;
    }
	return (*this);
}

Location::~Location(){
    DEBUG("\t\tDestructor called.");
}

/*------------------------------------------*\
|                 GETTERS                    |
\*------------------------------------------*/

std::string const &Location::getPath() const
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

std::string const &Location::getReturn() const
{
    return this->_return;
}

bool Location::getMethodPermission(enum e_HRM method) const {
    std::map<enum e_HRM, bool>::const_iterator it = _methodPermissions.find(method);
    if (it != _methodPermissions.end()) {
        DEBUG("\t\tCan use that method at that location: %s", it->second ? "TRUE" : "FALSE");
        return it->second;
    }
    return false; // Default if not set
}


/*------------------------------------------*\
|                 SETTERS                    |
\*------------------------------------------*/

void Location::setReturn(std::string const &path)
{
    _return = path;
}

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
    _methodPermissions[r_GET]     = newPermissions[r_GET];
    _methodPermissions[r_POST]    = newPermissions[r_POST];
    _methodPermissions[r_DELETE]  = newPermissions[r_DELETE];
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
  #ifdef _PRINT_
  std::cout << YELLOW << "Location " << this->getPath()
  << ": added: <" << value <<"> to <" << name
  << ">" << RESET << std::endl;
  #endif
}

/*------------------------------------------*\
|               PRINTERS                     |
\*------------------------------------------*/

 void Location::printMethodPermissions() const {
        std::cout << BLUE << "Method Permissions:\n";
        std::map<enum e_HRM, bool>::const_iterator it;
        for (it = _methodPermissions.begin(); it != _methodPermissions.end(); ++it) {
            const char* method;
            switch (it->first) {
                case r_GET: method = "GET"; break;
                case r_POST: method = "POST"; break;
                case r_DELETE: method = "DELETE"; break;
                default: method = "Unknown"; break;
            }
            std::cout << method << ": " << (it->second ? "true" : "false") << "\n";
        }
        std::cout << RESET << std::endl;
    }
/*------------------------------------------*\
|                 TODO                       |
\*------------------------------------------*/

// void Location::setAllowMethods(const std::string& methods) {
//     std::map<std::string, e_HRM> methodMap;
//     methodMap["GET"] = r_GET;
//     methodMap["POST"] = r_POST;
//     methodMap["DELETE"] = r_DELETE;

//     std::istringstream iss(methods);
//     std::string method;
//     while (iss >> method) {
//         std::map<std::string, e_HRM>::iterator it = methodMap.find(method);
//         if (it != methodMap.end()) {
//             #ifdef _PRINT_
//             std::cout << GREEN <<  "Location: "<< getPath() <<" setting "
//             <<it->first<<" to true." << RESET << std::endl;
//             #endif
//             _methodPermissions[it->second] = true;
//         } else {
//             std::cout << "Unknown method: " << method << std::endl;
//         }
//     }
// }


/*
 * After a bit of discussion and review, for now we have decided if the allow methods directive is 
 * defined at a given location, "Clean slate" the allowed methods to false before then
 * defining what is allowed.
 *
 * If allow_methods directive isnt defined in the location's scope, it keeps the default server 
 * permissions it received when it was created.
 */

void Location::setAllowMethods(const std::string& methods) {
    // Reset permissions to a clean slate.
    _methodPermissions[r_GET] = false;
    _methodPermissions[r_POST] = false;
    _methodPermissions[r_DELETE] = false;

    std::istringstream iss(methods);
    std::string method;
    while (iss >> method) {
        if (method == "GET") {
            _methodPermissions[r_GET] = true;
        } else if (method == "POST") {
            _methodPermissions[r_POST] = true;
        } else if (method == "DELETE") {
            _methodPermissions[r_DELETE] = true;
        } else {
            // Log or handle the unrecognized method name
            DEBUG("Invalid method name: %s", method.c_str());
        }
    } 
}


void Location::setAutoIndex(std::string stateString)
{
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

bool Location::isNull()
{ 
    return this == &NullLocation;
}

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
    // size_t i = 0;
    std::vector< std::pair < std::string, std::string> > temp = src.get_directives();
    if (temp.empty())
    {
        return;
    }
    for(std::vector< std::pair < std::string, std::string> >::iterator it = temp.begin(); it != temp.end(); ++it)
    {
        if (isValidLocationDirective(it->first))
        {
            this->setDirective(it->first, it->second);
        }
        else
        {
            #ifdef _PRINT_
            std::cout << MAGENTA << it->first << "is not a Location directive I am aware of.."
            << RESET << std::endl;
            #endif
        }
        // i++;
    }
}
