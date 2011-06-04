
SRC = $(shell find src/*.c)
OBJ = ${SRC:.c=.o}

PREFIX = /usr/local
CC = clang
CFLAGS = -Wall -std=c99 -DEBUG_LEXER=1

luna: $(OBJ)
	@mkdir -p build
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f luna $(OBJ)

.PHONY: clean