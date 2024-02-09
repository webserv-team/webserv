CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98

NAME = webserv

SRCS =  server.cpp ../parsing/Request.cpp ../parsing/ServerConf.cpp

HEADERS = 

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

run: all
	./$(NAME) ../default.toml

fclean: clean
	rm -f $(NAME)

re: fclean all


.PHONY: all clean fclean re
