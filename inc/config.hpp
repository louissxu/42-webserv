#ifndef CONFIG_HPP
# define CONFIG_HPP


// * EXAMPLE config
/*
server {
    listen 80;
    server_name example.com;

    location / {
        root /var/www/html;
        index index.html;
    }
}

server {
    listen 443 ssl;
    server_name secure.example.com;

    ssl_certificate /etc/nginx/cert.crt;
    ssl_certificate_key /etc/nginx/cert.key;

    location / {
        root /var/www/secure;
        index index.html;
    }
}
*/

#include <string>
class config
{
	private:
		std::string port;
		std::string ip;
		std::string location;

	public:


};

#endif
