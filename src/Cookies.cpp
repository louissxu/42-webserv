#include "Cookies.hpp"

std::string Cookies::getRandom(int ip)
{
    srand(time(NULL) + ip + rand());
    std::stringstream ss;
    for(int i = 0;i < 64;i++)
    {
            int i = rand() % 127;
            while(i < 32)
                    i = rand() % 127;
            ss << char(i);
    }
    return ss.str();
}