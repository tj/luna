
SRC = $(shell find src/*.c)
OBJ = ${SRC:.c=.o}

CC = gcc
PREFIX = /usr/local
CFLAGS = -std=c99 -g -O0 -Wno-parentheses -Wno-switch-enum
CFLAGS += -I deps

# linenoise

CFLAGS += -I deps/linenoise
OBJ += deps/linenoise/linenoise.o

TEST_SRC = $(shell find {test,src}/*.c | sed '/luna/d')
TEST_OBJ = ${TEST_SRC:.c=.o}
CFLAGS += -I src

luna: $(OBJ)
	$(CC) $^ $(LDFLAGS) -o $@

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