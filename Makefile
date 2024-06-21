CFILES = $(wildcard src/*.c src/*/*.c util/*.c)
OFILES = $(patsubst %.c, build/%.o, $(CFILES))

CC = gcc
FLAGS = -g -std=c11 -Wall -Wextra -pedantic -Werror -Iinclude -Iutil -Isrc

.Phony: all
all: build/fir

build/fir: test/main.c $(OFILES)
	$(CC) $(FLAGS) $^ -o $@

build/%.o: %.c
	mkdir -p $(@D)
	$(CC) -c $(FLAGS) $(INCLUDES) $^ -o $@

.Phony: clean
clean:
	rm -r build

