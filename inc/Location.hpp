#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <vector>
#include <cstdlib>
#include <map>

#include "Definitions.hpp"

//http request method ENUM.
enum e_HRM {
    GET,
    POST,
    PATCH,
    PUT,
    DELETE,
    HEAD,
    OPTIONS,
    CONNECT,
    TRACE
};

class Location
{
    public:

    // Constructors
        Location();
        Location(const Location& other);
        Location& operator=(const Location& other);
        ~Location();

    // Getters
        std::string                 getPath();
        std::string                 getRoot();
        std::string                 getIndex();
        std::string                 getFilePathPost();
        std::map<enum e_HRM, bool>  getMethodPermissions();
        bool                        getAutoIndex();
        size_t                      getClientMaxBodySize();

    // Setters
        void setPath(std::string newPath);
        void setRoot(std::string newRoot);       
        void setIndex(std::string newIndex);  
        void setFilePathPost(std::string newFilePathPost);  
        void setMethodPermissions(std::map<enum e_HRM, bool> newPermissions);
        void setMethodPermission(enum e_HRM, bool permissionState);
        void setAutoIndex(bool indexState);
        void setClientMaxBodySize(std::string size_str);
        void setClientMaxBodySize(size_t newClientMaxBodySize);

    // Other
        void initMethodPermissions();

    //Exceptions
    	class ErrorException : public std::exception
		{
			private:
				std::string _message;
			public:
				ErrorException(std::string message) throw()
				{
					_message = "SERVER CONFIG ERROR: " + message;
				}
				virtual const char* what() const throw()
				{
					return (_message.c_str());
				}
				virtual ~ErrorException() throw() {}
		};

    private:
        std::string                 _path;
        std::string                 _root;
        std::string                 _index;
        std::string                 _filePathPost;
        std::map<enum e_HRM, bool>  _methodPermissions;
        bool                        _autoIndex;
        size_t                      _clientMaxBodySize;
};

#endif