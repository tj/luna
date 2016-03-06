define ccprint
	@printf '\e[36mCC\e[90m %s\e[0m\n' $1
endef

SHELL = /bin/bash
SRC = $(wildcard src/*.c)
OBJ = ${SRC:.c=.o}

CC = clang
PREFIX = /usr/local
CFLAGS = -std=c99 -g -O0 -Wno-parentheses -Wno-switch-enum -Wno-unused-value
CFLAGS += -Wno-switch
CFLAGS += -I deps
CFLAGS += -D_BSD_SOURCE
LDFLAGS += -lm

# linenoise

CFLAGS += -I deps/linenoise
OBJ += deps/linenoise/linenoise.o
OBJ += deps/linenoise/utf8.o

TEST_SRC = $(shell find {test,src}/*.c | sed '/luna/d')
TEST_OBJ = ${TEST_SRC:.c=.o}
CFLAGS += -I src

# output

OUT = luna
ifdef SystemRoot
	OUT = luna.exe
endif

$(OUT): $(OBJ)
	$(CC) $^ $(LDFLAGS) -lm -o $@

%.o: %.c
	@$(CC) -c $(CFLAGS) $< -o $@
	$(call ccprint, $@)

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
