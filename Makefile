CC = gcc -std=c99
FLAGS = -g -Wall -Wextra -Werror -pedantic
INCLUDES = -Iinclude -Isrc

BUILD_DIR = build
LIB = $(BUILD_DIR)/fir.a
TEST = $(BUILD_DIR)/test

CFILES = $(wildcard src/*.c src/*/*.c)
OFILES = $(patsubst %.c, $(BUILD_DIR)/%.o, $(CFILES))

TEST_CFILES = $(wildcard test/*.c test/*/*.c)

# commands
.Phony: all
all: $(LIB)

.Phony: test
test: $(TEST)
	./$(TEST)

.Phony: clean
clean:
	rm -r $(BUILD_DIR)

$(LIB): $(OFILES)
	ar rcs $@ $^

$(TEST): $(TEST_CFILES) $(LIB)
	$(CC) $(FLAGS) $(INCLUDES) $^ -o $@

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(@D)
	$(CC) -c $(FLAGS) $(INCLUDES) $^ -o $@
