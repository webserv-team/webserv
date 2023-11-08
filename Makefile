# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/27 11:05:58 by ogorfti           #+#    #+#              #
#    Updated: 2023/11/07 09:58:04 by ogorfti          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SERVER =	${addprefix src/, main.cpp}

SRCS = $(SERVER)

OBJ_DIR = obj

OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRCS))

RM = rm -rf

NAME = webserv

$(OBJ_DIR)/%.o: %.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "\033[1;32mDONE!\033[0m"

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all
