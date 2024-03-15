#include <string.h>
#include <stdio.h>
#include "Cout.hpp"

#define LOG_LEVEL 4 // for printing logs.

#define __PFILE__ (strrchr(__FILE__, '/')           \
					   ? strrchr(__FILE__, '/') + 1 \
					   : __FILE__)

#define LOG(level, format, ...)                                              \
	if (strcmp(level, "record") == 0)                                        \
	{                                                                        \
		printf("%s[%s]%s: " format "\n\n", BOLDGREEN, level, RESET,          \
			   ##__VA_ARGS__);                                               \
	}                                                                        \
	else if (strcmp(level, "err") == 0)                                      \
	{                                                                        \
		fprintf(stderr, "\033[31m[%s]\033[0m %s: " format "\n", level,       \
				__PFILE__, ##__VA_ARGS__);                                   \
	}                                                                        \
	else if (strcmp(level, "deb") == 0)                                      \
	{                                                                        \
		printf("%s[%s]%s %s: " format "\n", CYAN, level, RESET, __PFILE__,   \
			   ##__VA_ARGS__);                                               \
	}                                                                        \
	else                                                                     \
	{                                                                        \
		printf("%s[%s]%s %s: " format "\n", YELLOW, level, RESET, __PFILE__, \
			   ##__VA_ARGS__);                                               \
	}

#define RECORD_LEVEL 1
#define ERR_LEVEL 2
#define WARN_LEVEL 3
#define DEBUG_LEVEL 4

#if LOG_LEVEL >= RECORD_LEVEL
#define RECORD(format, ...) \
	LOG("record", format, ##__VA_ARGS__)
#else
#define RECORD(...)
#endif

#if LOG_LEVEL >= ERR_LEVEL
#define ERR(format, ...) \
	LOG("err", format, ##__VA_ARGS__)
#else
#define ERR(...)
#endif

#if LOG_LEVEL >= WARN_LEVEL
#define WARN(format, ...) \
	LOG("warn", format, ##__VA_ARGS__)
#else
#define WARN(...)
#endif

#if LOG_LEVEL >= DEBUG_LEVEL
#define DEBUG(format, ...) \
	LOG("deb", format, ##__VA_ARGS__)
#else
#define DEBUG(...)
#endif