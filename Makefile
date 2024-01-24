#	o-----------------o
#	|    VARIABLES    |
#	o-----------------o

NAME = webserv
CC = c++
CFLAGS = -Wall -Wextra -Werror -g -std=c++98 #-fsanitize=address -g

#	o-----------------o
#	|    INCLUDES     |
#	o-----------------o

INC = inc

#	o-----------------o
#	|     SOURCES     |
#	o-----------------o

SRC_FILES = \
	HTTPResponse.cpp \
	HTTPRequest.cpp \
	ServerManager.cpp \
	Server.cpp \
	Connection.cpp \
	Cout.cpp \
	main.cpp

SRC_DIR = src

SRCS = $(addprefix $(SRC_DIR)/, $(SRC_FILES))

OBJ_DIR = obj

OBJ_RAW = $(SRC_FILES:.cpp=.o)

OBJS = $(addprefix $(OBJ_DIR)/, $(OBJ_RAW))

#	o-----------------o
#	|      RULES      |
#	o-----------------o

all: $(OBJ_DIR) $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Making $<"
	$(CC) $(CFLAGS) -I $(INC) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm -f $(OBJS)
	rm -df $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
