CC      = gcc
CFLAGS  = -Iinclude -Wall -Wextra -g $(shell sdl2-config --cflags) 
LDLIBS  = $(shell sdl2-config --libs)
TARGET  = cellmata

SRC = $(shell find src -name '*.c')
OBJ = $(patsubst src/%.c,build/%.o,$(SRC))
DEP = $(OBJ:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

build:
	mkdir -p build

run: all
	./$(TARGET)

clean:
	rm -rf build $(TARGET)

re: clean all

-include $(DEP)

.PHONY: all run clean re