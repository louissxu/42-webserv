#include "MIME.hpp"

std::map<std::string, std::string> createMIMEMap()
{
	std::map<std::string, std::string> mimeTypes;

	mimeTypes[".html"] = "text/html";
	mimeTypes[".js"] = "text/javascript";
	mimeTypes[".css"] = "text/css";
	mimeTypes[".jpg"] = "image/jpeg";
	mimeTypes[".png"] = "image/png";
	return mimeTypes;
}

std::string MimeTypes::getMimeType(const std::string &extension)
{
	static const std::map<std::string, std::string> mimeTypes = createMIMEMap();

	std::map<std::string, std::string>::const_iterator it = mimeTypes.find(extension);
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