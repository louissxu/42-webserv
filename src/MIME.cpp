#include "MIME.hpp"

std::string MimeTypes::getMimeType(const std::string &extension)
{
	static const std::map<std::string, std::string> mimeTypes = {
		{".html", "text/html"},
		{".css", "text/css"},
		{".js", "text/javascript"},
		{".jpg", "image/jpeg"},
		{".png", "image/png"},
		// Add more mappings as needed
	};

	std::map<std::string, std::string>::iterator it = mimeTypes.find(extension);
	if (it != mimeTypes.end())
	{
		return it->second;
	}
	else
	{
		// Default MIME type if extension not found
		return "application/octet-stream";
	}
}