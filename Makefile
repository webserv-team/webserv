# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hassan <hassan@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/27 11:05:58 by ogorfti           #+#    #+#              #
#    Updated: 2024/03/08 17:31:00 by hassan           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror  -fsanitize=address -g -I./include -I./parsing #-std=c++98

PARSING =	${addprefix parsing/, Request.cpp ServerConf.cpp ErrUtils.cpp}

NAME = server

SRCS = $(addprefix srcs/, main.cpp WebServer.cpp Cgi.cpp helpers.cpp Response.cpp Mimes.cpp Socket.cpp Header.cpp)  $(PARSING) 
# SRCS = $(addprefix srcs/, main.cpp )  $(PARSING) 

HEADERS = parsing/Request.hpp parsing/ServerConf.hpp WebServer.hpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

%.o: %.cpp 
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJS) 
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

run: all
	./$(NAME) default.conf

.PHONY: all clean fclean re
