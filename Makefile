#	o-----------------o
#	|    VARIABLES    |
#	o-----------------o

NAME = webserv
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 #-fsanitize=address -g

#	o-----------------o
#	|    INCLUDES     |
#	o-----------------o

INC = inc
LIBFT_INC = 42-libft/includes
LIBFT = 42-libft/libft.a

#	o-----------------o
#	|     SOURCES     |
#	o-----------------o

SRC_FILES = main.cpp

SRC_DIR = src

SRCS = $(addprefix $(SRC_DIR)/, $(SRC_FILES))

OBJ_DIR = obj

OBJ_RAW = $(SRC_FILES:.cpp=.o)

OBJS = $(addprefix $(OBJ_DIR)/, $(OBJ_RAW))

#	o-----------------o
#	|      RULES      |
#	o-----------------o

all: $(OBJ_DIR) $(NAME)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(OBJS) -L libft -lft $(LFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Making $<"
	$(CC) $(CFLAGS) -I $(INC) -I $(LIBFT_INC) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

$(LIBFT):
	@echo "Making libft"
	$(MAKE) -C libft

clean:
	$(MAKE) clean -C libft
	rm -f $(OBJS)
	rm -df $(OBJ_DIR)

fclean: clean
	$(MAKE) fclean -C libft
	rm -f $(NAME)

re: fclean all

.PHONY: all libft clean fclean re
