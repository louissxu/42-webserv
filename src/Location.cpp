#include "Location.hpp"

    // private:
    //     std::string                 _path;
    //     std::string                 _root;
    //     std::string                 _index;
    //     std::string                 _filePathPost;
    //     std::map<enum e_HRM, bool>  _methodPermissions;
    //     std::map<int, std::string>	_errPage;
    //     bool                        _autoIndex;
    //     size_t                      _clientMaxBodySize;


/*------------------------------------------*\
|              CONSTRUCTORS                  |
\*------------------------------------------*/

Location::Location()
{

}

Location::Location(const Location& other)
{

}

Location::Location& operator=(const Location& other)
{

}

Location::~Location()
{

}

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

std::map<int, std::string> Location::getErrPage()
{
    return _errPage;
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

}

void Location::setRoot(std::string newRoot)
{

}

void Location::setIndex(std::string newIndex)
{

}

void Location::setFilePathPost(std::string newFilePathPost)
{

}

void Location::setMethodPermissions(std::map<enum e_HRM, bool> newPermissions)
{

}

void Location::setMethodPermission(enum e_HRM, bool permissionState)
{

}

void Location::setErrPages(std::map<int, std::string> newErrPage)
{

}

void Location::setErrPage(int pageId, std::string page)
{

}

void Location::setAutoIndex(bool indexState)
{

}

void Location::setClientMaxBodySize(size_t newClientMaxBodySize)
{

}

  
