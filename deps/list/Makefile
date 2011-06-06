
AR ?= ar
CC ?= gcc
PREFIX ?= /usr/local

CFLAGS = -O3 -std=c99 -Wall

SRCS = src/list.c \
		   src/node.c \
		   src/iterator.c

OBJS = $(SRCS:.c=.o)

all: build/liblist.a

install: all
	cp -f build/liblist.a $(PREFIX)/lib/liblist.a
	cp -f src/list.h $(PREFIX)/include/list.h

uninstall:
	rm -f $(PREFIX)/lib/liblist.a
	rm -f $(PREFIX)/include/list.h

build/liblist.a: $(OBJS)
	@mkdir -p build
	$(AR) rcs $@ $^

bin/test: test.o $(OBJS)
	@mkdir -p bin
	$(CC) $^ -o $@

bin/benchmark: benchmark.o $(OBJS)
	@mkdir -p bin
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $< $(CFLAGS) -c -o $@

clean:
	rm -fr bin build *.o src/*.o

test: bin/test
	@./$<

benchmark: bin/benchmark
	@./$<

.PHONY: test benchmark clean install uninstall