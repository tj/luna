
SRC = $(wildcard src/*.c)
OBJ = ${SRC:.c=.o}

CC = gcc
PREFIX = /usr/local
CFLAGS = -std=c99 -g -O0 -Wno-parentheses -Wno-switch-enum -Wno-unused-value
CFLAGS += -Wno-switch
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
	@$(CC) -c $(CFLAGS) $< -o $@
	@echo CC $@

test: test_runner
	@./$<

test_runner: $(TEST_OBJ)
	$(CC) $^ -o $@

install: luna
	install luna $(PREFIX)/bin

uninstall:
	rm $(PREFIX)/bin/luna

clean:
	rm -f luna test_runner $(OBJ) $(TEST_OBJ)

.PHONY: clean test install uninstall
