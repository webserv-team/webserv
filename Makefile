# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/27 11:05:58 by ogorfti           #+#    #+#              #
#    Updated: 2024/03/30 19:17:54 by hoigag           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror   -I./include -I./parsing -std=c++98 #-fsanitize=address -g

SRCS = $(wildcard srcs/cgi/*.cpp) $(wildcard srcs/server/*.cpp) $(wildcard srcs/response/*.cpp) $(wildcard srcs/*.cpp) $(wildcard srcs/parsing/*.cpp)

HEADERS = $(wildcard include/*.hpp) $(wildcard parsing/*.hpp)

OBJ_DIR = obj

OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRCS))

NAME = webserv

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "\033[1;32mDONE!\033[0m"

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

run: all
	./$(NAME) config/default.conf

.PHONY: all clean fclean re
