#include "ConfigParser.hpp"

unsigned int ConfigParser::lastAssignedPort = 8000;

/*------------------------------------------\
|              CONSTRUCTORS                 |
\------------------------------------------*/

ConfigParser::ConfigParser() {
    //_directives.insert(std::make_pair("user", "nobody"));
    //_directives.insert(std::make_pair("worker_processes", "1"));
    _contextLvl = 0;
    _name = "default";
}

// change your constructors and assignment operator
ConfigParser::ConfigParser(const ConfigParser & src) : 
    _name(src._name), _contextLvl(src._contextLvl) {
    _directives = src._directives;
    _contexts = src._contexts;
}

const ConfigParser &ConfigParser::operator=(const ConfigParser & src) {
    if (this != &src) {
        _directives = src._directives;
        _contexts = src._contexts;
        _contextLvl = src._contextLvl;
        _name = src._name;
    }
    return *this;
}

ConfigParser::~ConfigParser() {}

/*------------------------------------------\
|                GETTERS                    |
\------------------------------------------*/

std::vector< std::pair <std::string, std::string> > ConfigParser::get_directives() const
{
    return _directives;
}

std::vector < ConfigParser > ConfigParser::get_contexts() const 
{
    return _contexts;
}


std::string		ConfigParser::getName(void) const
{
    return _name;
}

size_t	ConfigParser::get_contextLvl() const
{
    return _contextLvl;
}

// Find the first non-whitespace character
char getFnwc(std::string& str) {
    std::string::const_iterator it = std::find_if(
        str.begin(), 
        str.end(), 
        std::not1(std::ptr_fun<int, int>(std::isspace))
    );

    if (it == str.end()) {
        return '\0';
    }
    return *it;
}



std::string ConfigParser::getContextName(std::string line)
{
    std::string newStr;
    std::string::iterator it = line.begin();
   while (it != line.end() && *it != '{')//std::isalpha(*it))
   {
       newStr.push_back(*it);
       ++it;
   }
   removeWhiteSpace(newStr); //remove trailing and leading whitespace.
   return newStr;
}

e_lineType ConfigParser::getLineType(std::string line)
{
    
    if (isOnlyWhitespace(line))
	{
        return OTHER;
    }
	else if (isDirectiveInit(line))
    {
        return DIRECTIVE_INIT;
	}
	else if (isContextInit(line))
	{
		return CONTEXT_INIT;
	}
    else if (getFnwc(line) == '}')
    {
        if (_contextLvl == 0)
        {
            //std::cout << "Webserv: [internal]: Error! Cannot have context less than 0" << std::endl;
            return (OTHER);
        }
        return CONTEXT_END;
    }
    else
    {
        return OTHER;
    }
}

std::string ConfigParser::getListen() const {
    for (size_t i = 0; i < _directives.size(); ++i) {
        if (_directives[i].first == "listen") {
            return _directives[i].second;
        }
    }
    std::cout << "Error: No listen set! setting to default listen: " << lastAssignedPort << std::endl;
    return std::to_string(lastAssignedPort++);
}


/*------------------------------------------\
|                SETTERS                    |
\------------------------------------------*/

void		ConfigParser::setName(std::string name)
{
    this->_name = name;
}

void    ConfigParser::setContextLvl(size_t lvl)
{
    this->_contextLvl = lvl;
}

int ConfigParser::setContent(std::string fileName)
{
    if (!isFilePath(fileName))
    {
            std::cout <<" WAT. " << std::endl;
    }

    std::string line;
    if (fileName.empty() || fileName.length() == 0)
    {
        return (1);
    }
    std::ifstream  config_file(fileName.c_str());
    if (!config_file || !config_file.is_open())
    {    
        return (1);
    }
    size_t i = 0;
    std::string statement = "";
    while (std::getline(config_file, line))
    {
        removeComments(line);
        removeWhiteSpace(line);
        if (getLineType(line) ==  DIRECTIVE_INIT)
        {
            do
            {
                statement += line;
                if (!line.empty() && line.back() == ';')
                {
                    break;
                }
            }
            while (std::getline(config_file, line));
        }
        else
        {
            statement = line;
        }   
        if (!statement.empty())
        {
            _contents += statement + "\n";
            statement.clear();  // clear the statement for the next one
        }
        i++;
    }
    return (0);
}

void    ConfigParser::setContextConfig()
{
    std::string line;
    if (_contents.empty() || _contents.length() == 0)
    {
        std::cout << "error: contents not set!" << std::endl;
        return ;
    }
    std::ifstream  config_file(_contents);
   	if (!config_file || !config_file.is_open())
    {    
        return ;
    }
    size_t lineNumber = 0;
    while (std::getline(config_file, line))
    {
        e_lineType j = getLineType (line);
        std::cout << "line [" << lineNumber << "]: ";
        switch(j)
        {
            case DIRECTIVE_INIT:
            {
                std::cout << "Directive Initialization." << std::endl;
                break;
            }
            case CONTEXT_INIT:
            {
                std::cout << "Context Initialization." << std::endl;
                break;
            }
            case CONTEXT_END:
            {
                std::cout << "Context End." << std::endl;
                break;
            }
            case OTHER : 
            {
                std::cout << "Other!" << std::endl;
                break;
            }
        }
        lineNumber++;
    }
}

/*  @brief: set_direct_init
        assumes line is a directive initialization. sets our _directives container with its key and value.
    @param line : a string containing the line currently being assessed.
*/
bool ConfigParser::setDirectiveInit(std::string line)
{

    bool       keyDefined     = false;
    bool       valueDefined   = false;
    bool       hasWrote       = false;
    std::string key;
    std::string value;

 //   std::cout << "isDirectiveInit : checking : <" << line << ">. " << std::endl;
   for (std::string::iterator it = line.begin(); it != line.end();)
   {
       while (std::isspace(*it))
       {
           ++it;
       }
        if (*it == '\n')
        {
            return  false;
        }
       else if (keyDefined && valueDefined && *it == ';')
       {
           break;
       }
       else if (keyDefined == false && !std::isalpha(*it))
       {
           return false;
       }
       else if (!keyDefined)
       {
           while (!std::isspace(*it) && std::isprint(static_cast<unsigned char>(*it)))
           {
               key.push_back(*it);
               ++it;
           }
           keyDefined = true;
        //    std::cout << "key : <" << key << ">." << std::endl;
       }
       else
       {
            if (valueDefined && hasWrote)
            {
               value.push_back(' ');
            }
            if (*it == '{')
            {
               return false;
            }
            while (!std::isspace(*it) && std::isprint(static_cast<unsigned char>(*it)))
            {
                if (*it == ';' && valueDefined == false)
                {
                    return false;
                }
                if (*it == ';' && valueDefined == true)
                {
                    break;
                }
                value.push_back(*it);
                hasWrote = true;
                valueDefined = true;
               ++it;
           }
       }
   }
   //_directives.insert(std::make_pair(key, value));
   _directives.push_back(std::make_pair(key, value)); 
   return true;
}

void ConfigParser::setContextContent(std::string contents, size_t lineNumber)
{
    std::string line;
    std::istringstream config_file(contents);
    size_t currentLine = 0;
    std::string outputString;

    while (currentLine <= lineNumber && std::getline(config_file, line)) {
        currentLine++;
    }
    _contextLvl = 1; // Assuming the context starts at the line after lineNumber
    while (_contextLvl > 0 && std::getline(config_file, line)) {
        e_lineType j = getLineType (line);
        switch(j)
        {
           case CONTEXT_INIT:
           {
                //LOG("context init found.")
                _contextLvl++;
                break;
           }
           case CONTEXT_END:
           {
                //LOG("context end found.")
                _contextLvl--;
                break;
           }
           case DIRECTIVE_INIT:
            {
                //LOG("directive init found.")
                break;
            }
            case OTHER:
            {
                //LOG("other found.")
                break;
            }
        }
        // Add line to outputString if still in context
        if (_contextLvl > 0)
        {
            //outputString += line + "\n";
            this->_contents += line + "\n";
        }
        currentLine++;
    }
    // Now outputString contains all the lines from lineNumber+1 to the end of the context
}

void ConfigParser::setStateFromContent(size_t myContextLvl, bool print)
{
    std::string line;
    if (_contents.empty() || _contents.length() == 0)
    {
        //std::cout << "error: contents not set!" << std::endl;
        return ;
    }
    std::istringstream config_file(_contents);
    size_t lineNumber = 0;
    while (std::getline(config_file, line))
    {
        e_lineType j = getLineType (line);
        // if (print)
        //     std::cout << "line [" << lineNumber << "]: ";
        switch(j)
        {
           case DIRECTIVE_INIT:
           {
                // if (print)
                //     std::cout << "Directive Initialization." << std::endl;
                if (_contextLvl == myContextLvl)
                {
                    setDirectiveInit(line);
                }
                break;
           }
           case CONTEXT_INIT:
           {
                // if(print)
                //     std::cout << "Context Initialization." << std::endl;
                if (_contextLvl == myContextLvl)
                {
                    ConfigParser *newConfigParser = new ConfigParser();
                    newConfigParser->setName(getContextName(line));
                    newConfigParser->setContextContent(_contents, lineNumber);
                    newConfigParser->setContextLvl(myContextLvl + 1);
                    newConfigParser->setStateFromContent(myContextLvl + 1, false);
                    _contexts.push_back(*newConfigParser);
                }
                _contextLvl++;
                break;
           }
           case CONTEXT_END:
           {
                // if (print)
                //     std::cout << "Context End." << std::endl;
                _contextLvl--;
                break;
           }
           case OTHER : 
           {
                // if (print)
                //     std::cout << "Other!" << std::endl;
                break;
           }
        }
        lineNumber++;
    }
}


/*------------------------------------------\
|                 BOOLEANS                  |
\------------------------------------------*/

// std::isspace returns non-zero if c is a whitespace character, and zero otherwise.
// We can convert this to bool by comparing the result to zero.
bool ConfigParser::isWhiteSpace(char c)
{
    return std::isspace(static_cast<unsigned char>(c)) != 0;
}

/*  @brief: is_direct_init
        returns whether or not a given line is a directive initialization.
    @param line : a string containing the line currently being assessed.
    @return:
        true : if our line is a directive initialization.
        false : if not
*/
bool ConfigParser::isDirectiveInit(std::string line)
{
   bool       keyDefined     = false;
   bool       valueDefined   = false;
   bool       hasWrote       = false;
   std::string key;
   std::string value;

 //   std::cout << "isDirectiveInit : checking : <" << line << ">. " << std::endl;
   for (std::string::iterator it = line.begin(); it != line.end();)
   {
       while (std::isspace(*it))
       {
           ++it;
       }
        if (*it == '\n')
        {
            return false;
        }
       else if (keyDefined && valueDefined && *it == ';')
       {
           break;
       }
       else if (keyDefined == false && !std::isalpha(*it))
       {
           return false;
       }
       else if (!keyDefined)
       {
           while (!std::isspace(*it) && std::isprint(static_cast<unsigned char>(*it)))
           {
               key.push_back(*it);
               ++it;
           }
           keyDefined = true;
        //    std::cout << "key : <" << key << ">." << std::endl;
       }
       else
       {
            if (valueDefined && hasWrote)
            {
               value.push_back(' ');
            }
            if (*it == '{')
            {
               return false;
            }
            while (!std::isspace(*it) && std::isprint(static_cast<unsigned char>(*it)))
            {
                if (*it == ';' && valueDefined == false)
                {
                    return false;
                }
                if (*it == ';' && valueDefined == true)
                {
                    break;
                }
                value.push_back(*it);
                hasWrote = true;
                valueDefined = true;
               ++it;
           }
            //valueDefined = true;
       }
   }
    if (valueDefined == true)
    {
       // std::cout << "value :<" << value << ">.";
    }
  // std::cout << std::endl;
   return true;
}

bool ConfigParser::isContextInit(std::string line)
{
  //  std::cout << "isContextInit : checking : <" << line << ">. " << std::endl;
    const char* WhiteSpace = " \t\t\v\r\n";
    std::size_t end = line.find_last_not_of(WhiteSpace);
	char lastCharacter = line[end];
    if (lastCharacter == '{')
    {
        return true;
    }
    else
    {
        return false;
    }
}

// determine whether a given string contains only whitespace.
bool ConfigParser::isOnlyWhitespace(std::string line)
{
   // Check if the line contains any non-whitespace characters
   if (std::find_if(line.begin(), line.end(), std::not1(std::ptr_fun<int, int>(std::isspace))) != line.end())
   {
       return false;
   }
   else
   {
       return true;
   }
}

int		ConfigParser::isFilePath(const std::string& path)
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0 )
	{
		if (s.st_mode & S_IFDIR)
        {
			return 0;
        }
		else if (s.st_mode & S_IFREG)
        {
			return 1;
        }
		else
        {
			return 0;
        }
	}
	else
    {
		return 0;
    }
}

/*------------------------------------------\
|                PRINTERS                    |
\------------------------------------------*/

void ConfigParser::printContents()
{
    std::cout << _contents;
}

// @param contents is a string we have written the file to.
void ConfigParser::printContents2()
{
    std::istringstream iss(_contents);
    std::string line;
    size_t it = 0;

    while (std::getline(iss, line))
    {
        e_lineType j = this->getLineType(line);
        std::cout << "line ["<< j <<"][" << it << "] :"<< line;
        ++it;
    }
}

void ConfigParser::printDirectives()
{
    //std::cout << "ConfigParser: printDirectives called." << std::endl;
    size_t i = 0;
    size_t j;

    for(std::vector< std::pair < std::string, std::string> >::iterator it = this->_directives.begin(); it != this->_directives.end(); ++it)
    {
        j = 0;
        while (j < this->_contextLvl)
        {
            std::cout <<"\t\t";
            j++;
        }
        //std::cout << "Directive [" << i << "]: Key: <" << it->first << "> Value: <" << it->second << ">." << std::endl;
        i++;
    }
}

void    ConfigParser::printContexts( void )
{
    size_t i = 0;
    size_t j; // = 0;

    this->printDirectives();
    for(std::vector< ConfigParser >::iterator it = _contexts.begin(); it != _contexts.end(); ++it)
    {
        j = 0;
        while (j < this->_contextLvl)
        {
            std::cout <<"\t\t";
            j++;
        }
        //std::cout << "context["<<i<<"]: name : <" << (*it).getName() << ">" << std::endl;
        //(*it).printDirectives();
        (*it).printContexts();
        i++;
    }
}

/*
int ConfigParser::printServerInformation()
{
	std::cout << "------------- ConfigParser -------------" << std::endl;
	for (size_t i = 0; i < _servers.size(); i++)
	{
		std::cout << "Server #" << i + 1 << std::endl;
		std::cout << "Server name: " << _servers[i].getServerName() << std::endl;
		std::cout << "Host: " << _servers[i].getHost() << std::endl;
		std::cout << "Root: " << _servers[i].getRoot() << std::endl;
		std::cout << "Index: " << _servers[i].getIndex() << std::endl;
		std::cout << "Port: " << _servers[i].getPort() << std::endl;
		std::cout << "Max BSize: " << _servers[i].getClientMaxBodySize() << std::endl;
		std::cout << "Error pages: " << _servers[i].getErrorPages().size() << std::endl;
		std::map<short, std::string>::const_iterator it = _servers[i].getErrorPages().begin();
		// while (it != _servers[i].getErrorPages().end())
		// {
		// 	std::cout << (*it).first << " - " << it->second << std::endl;
		// 	++it;
		// }
		// std::cout << "Locations: " << _servers[i].getLocations().size() << std::endl;
		// std::vector<Location>::const_iterator itl = _servers[i].getLocations().begin();
		// while (itl != _servers[i].getLocations().end())
		// {
		// 	std::cout << "name location: " << itl->getPath() << std::endl;
		// 	std::cout << "methods: " << itl->getPrintMethods() << std::endl;
		// 	std::cout << "index: " << itl->getIndexLocation() << std::endl;
		// 	if (itl->getCgiPath().empty())
		// 	{
		// 		std::cout << "root: " << itl->getRootLocation() << std::endl;
		// 		if (!itl->getReturn().empty())
		// 			std::cout << "return: " << itl->getReturn() << std::endl;
		// 		if (!itl->getAlias().empty())
		// 			std::cout << "alias: " << itl->getAlias() << std::endl;
		// 	}
		// 	else
		// 	{
		// 		std::cout << "cgi root: " << itl->getRootLocation() << std::endl;
		// 		std::cout << "sgi_path: " << itl->getCgiPath().size() << std::endl;
		// 		std::cout << "sgi_ext: " << itl->getCgiExtension().size() << std::endl;
		// 	}
		// 	++itl;
		// }
		// itl = _servers[i].getLocations().begin();
		std::cout << "-----------------------------" << std::endl;
	}
	return (0);
}
*/

/*------------------------------------------\
|             OTHER METHODS                 |
\------------------------------------------*/

void ConfigParser::removeWhiteSpace(std::string& content) {
    size_t start = content.find_first_not_of(" \t\t\n\v\f\r");
    if (start == std::string::npos) start = 0;

    size_t end = content.find_last_not_of(" \t\t\n\v\f\r");
    if (end != std::string::npos) end += 1;

    content = content.substr(start, end - start);
}

void    ConfigParser::removeComments(std::string &content)
{
    size_t pos;
    size_t pos_end;

    pos = content.find("#");
    while (pos != std::string::npos)
    {
        pos_end = content.find('\n', pos);
        content.erase(pos, pos_end - pos);
        pos = content.find('#');
    }
}

/*------------------------------------------\
|              Exceptions                   |
\------------------------------------------*/

const char* ConfigParser::Exception::what() const throw()
{
	return ("ConfigParser Exception: ");
}

const char* ConfigParser::FileNotFoundException::what() const throw()
{
	return ("File not found!");
}

const char* ConfigParser::InvalidFileNameException::what() const throw()
{
	return ("Not a file!");
};