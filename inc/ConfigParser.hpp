#ifndef CONFIG_PARSER_HPP
# define CONFIG_PARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm> 
#include <cctype>
#include <map>
#include <vector>
#include <sys/stat.h>

//#include "Utils.hpp"

enum e_lineType
{
    DIRECTIVE_INIT = 0,
    CONTEXT_INIT = 1,
    CONTEXT_END = 2,
    OTHER = 3
};	

class ConfigParser
{

	public:
		ConfigParser();
		ConfigParser(const ConfigParser & src);
		const ConfigParser &operator=(const ConfigParser & src);
		virtual ~ConfigParser();		

		//getters : 
		std::string &	get_directive(std::string &directiveName);
		std::string		getName(void);
		std::string		getContextName(std::string line);
		e_lineType 		getLineType(std::string line);

		//setters :
		int				setContent(std::string fileName);
		void    		setContextLvl(size_t lvl);
		void			setName(std::string name);

		//void    		setContextContents(std::string line);
		void    		setContextConfig();
		void			setConfig();
		void 			setStateFromContent(bool print);
		bool			setDirectiveInit(std::string line);
		void			setStateFromContent(size_t myContextLvl, bool print);

		//booleans
		bool 			isDirectiveInit(std::string line);
		bool    		is_charCheck01 (char c);
		bool			isWhiteSpace(char c);
		bool			isOnlyWhitespace(std::string line);
		bool			isContextInit(std::string line);
		std::string		getContextString(std::string line);
        int		        isFilePath(const std::string& path);

		//printers
		void			printContents();
		void			printContents2();
		void			printDirectives(void);
		void			printContexts( void );

		//other : 
		void			setContextContent(std::string contents, size_t lineNumber);
		std::string		read_file(std::string fileName);
		void 			removeComments(std::string &content);
		void			removeWhiteSpace(std::string &content);

		//Exception handling.
		class Exception : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};

		class FileNotFoundException : public ConfigParser::Exception
		{
			public:
				virtual const char* what() const throw();
		};

		class InvalidFileNameException : public ConfigParser::Exception
		{
			public:
				virtual const char* what() const throw();
		};


	private:
		std::vector< std::pair <std::string, std::string> > _directives;
		std::vector < ConfigParser >  _contexts;
		std::string _name;
		std::string _contents;
		size_t	_contextLvl;
};
#endif