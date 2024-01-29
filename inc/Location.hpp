#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <vector>
#include <map>

//http request method ENUM.
enum e_HRM {
    GET,
    POST,
    PATCH,
    PUT,
    DELETE,
    HEAD,
};

class Location
{
    public:
        Location();
        Location(const Location& other);
        Location& operator=(const Location& other);
        ~Location();

    private:

        std::string                 _path;
        std::string                 _root;
        std::string                 _index;
        std::string                 _errorPage;
        std::string                 _filePathPost;
        std::map<enum e_HRM, bool>  _method_permissions;
        std::map<int, std::string>	_err_page;
        bool                        _autoIndex;
        size_t          _client_max_body_size;
};

#endif