NAME= ircserv
BNAME= gemini
FLAGS= -Wall -Wextra -Werror -std=c++98
CC= c++
RM= rm -f
PORT=6667
PASS=lol
HEADER= src/channel/channel.hpp src/client/client.hpp src/server/server.hpp src/include_guard.hpp components/*hpp

BHEADER= bot/gemini.hpp

# includes aliasses
INC= -I src/server -I src -I src/channel -I src/client -I src/command -I components

SRC=		src/main.cpp			\
			src/server/*.cpp		\
			src/client/*.cpp		\
			src/channel/*.cpp		\
			src/commands/*.cpp	\

BSRC=		bot/gemini.cpp			\
			bot/main.cpp			\

$(NAME): $(SRC) $(HEADER)
	$(CC) $(FLAGS) $(INC) $(SRC) -o $(NAME)

all: $(NAME)

$(BNAME): $(BSRC) $(BHEADER)
	$(CC) $(FLAGS) $(INC) $(BSRC) -o $(BNAME)

bonus: $(BNAME)

clean:
	$(RM) $(NAME)
	$(RM) $(BNAME)

fclean: clean

re: fclean all

run: all
	clear
	./$(NAME) $(PORT) $(PASS)

b_run: bonus
	clear
	./$(BNAME) $(PORT) $(PASS)

.PHONY: all clean fclean re run bonus