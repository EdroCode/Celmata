CC      = gcc
CFLAGS  = -Iinclude -Wall -Wextra -g $(shell sdl2-config --cflags)
LDLIBS  = $(shell sdl2-config --libs) -lm
TARGET  = celmata

SRC = main.c $(shell find src -name '*.c')
HDR = $(shell find src -name '*.h')
OBJ = $(SRC:%.c=build/%.o)
DEP = $(OBJ:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -rf build $(TARGET)

format:
	clang-format -i $(SRC) $(HDR)

re: clean all

-include $(DEP)

.PHONY: all run clean format re