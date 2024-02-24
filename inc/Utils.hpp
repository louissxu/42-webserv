#ifndef UTILS_HPP
# define UTILS_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <map>

class Utils {
	public: 
        static std::string getFirst(const std::string& str);
        static std::string getSecond(const std::string& str);
        static void setColour(const std::string& colour);
};

#endif