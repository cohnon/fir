CFILES = $(wildcard src/*.c src/*/*.c)
OFILES = $(patsubst src/%.c, build/%.o, $(CFILES))

CC = gcc
FLAGS = -g -std=c11 -Wall -Wextra -pedantic -Iinclude

.Phony: all
all: fir

fir: test/main.c $(OFILES)
	$(CC) $(FLAGS) $^ -o $@

build/%.o: src/%.c
	mkdir -p build
	$(CC) -c $(FLAGS) $(INCLUDES) $^ -o $@

.Phony: clean
clean:
	rm -r fir build

