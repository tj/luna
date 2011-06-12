
SRC = $(shell find src/*.c)
OBJ = ${SRC:.c=.o}

CC = clang
PREFIX = /usr/local
CFLAGS = -Wall -Wno-unused-value -std=c99 -DEBUG_PARSER -g -O0 -I deps

TEST_SRC = $(shell find test/*.c)
TEST_OBJ = ${TEST_SRC:.c=.o}

luna: $(OBJ)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

test: test_runner
	@./$<

test_runner: $(TEST_OBJ)
	$(CC) $^ -o $@

update:
	cp -fr ../list/* deps/list/

clean:
	rm -f luna test_runner $(OBJ)

.PHONY: clean update test