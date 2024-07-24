CFILES = $(wildcard src/*.c src/*/*.c)
OFILES = $(patsubst %.c, build/%.o, $(CFILES))

CC = gcc
FLAGS = -g -std=c11 -Wall -Wextra -pedantic -Werror -Iinclude -Iutil -Isrc

LIB = build/fir.a
TEST = build/test

.Phony: all
all: $(LIB)

.Phony: test
test: $(TEST)


$(LIB): $(OFILES)
	ar rcs $@ $^

$(TEST): test/main.c $(LIB)
	$(CC) $(FLAGS) $^ -o $@

build/%.o: %.c
	mkdir -p $(@D)
	$(CC) -c $(FLAGS) $(INCLUDES) $^ -o $@

.Phony: clean
clean:
	rm -r build

