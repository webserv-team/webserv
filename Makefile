CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g -I./include -I./parsing

NAME = server

SRCS_DIR = srcs

SRCS = $(addprefix $(SRCS_DIR)/, main.cpp WebServer.cpp Cgi.cpp helpers.cpp Response.cpp Mimes.cpp Socket.cpp Header.cpp)  parsing/Request.cpp parsing/ServerConf.cpp 

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
	./$(NAME) default.toml

.PHONY: all clean fclean re
