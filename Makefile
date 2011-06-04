
SRC = $(shell find src/*.c)
OBJ = ${SRC:.c=.o}

CC = clang
PREFIX = /usr/local
CFLAGS = -Wall -Wno-unused-value -std=c99 -DEBUG_PARSER -g -O0

luna: $(OBJ)
	@mkdir -p build
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f luna $(OBJ)

.PHONY: clean