
SRC = $(shell find src/*.c)
OBJ = ${SRC:.c=.o}

CC = clang
PREFIX = /usr/local
CFLAGS = -Wall -Wno-unused-value -std=c99 -DEBUG_AST -g -O0
CFLAGS += -I deps

TEST_SRC = $(shell find {test,src}/*.c | sed '/luna/d')
TEST_OBJ = ${TEST_SRC:.c=.o}
CFLAGS += -I src

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
	rm -f luna test_runner $(OBJ) $(TEST_OBJ)

.PHONY: clean update test