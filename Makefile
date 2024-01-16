CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98

NAME = server

SRCS = main.cpp WebServer.cpp parsing/Request.cpp parsing/ServerConf.cpp

HEADERS = parsing/Request.hpp parsing/ServerConf.hpp WebServer.hpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

run: all
	./$(NAME)

.PHONY: all clean fclean re
