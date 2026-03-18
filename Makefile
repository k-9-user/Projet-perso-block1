# Projet Block1
# Auteur: kheira derdour

NAME = blockchain
TEST_NAME = blockchain_tests
CC = cc
CFLAGS = -Wall -Wextra -Werror

SRC = main.c hash.c
OBJ = $(SRC:.c=.o)
TEST_SRC = tests.c hash.c
TEST_OBJ = $(TEST_SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c block.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TEST_OBJ)

fclean: clean
	rm -f $(NAME) $(TEST_NAME)

re: fclean all

test: $(TEST_NAME)
	./$(TEST_NAME)

web: $(NAME)
	python3 web_server.py

$(TEST_NAME): $(TEST_OBJ)
	$(CC) $(CFLAGS) $(TEST_OBJ) -o $(TEST_NAME)

.PHONY: all clean fclean re test web
