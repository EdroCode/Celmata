CC = gcc
CFLAGS = -Wall -Wextra -std=c11 

NAME = simulation

all:
	$(CC) $(CFLAGS) main.c map.c user_interface.c mainloop.c game_logic.c rabbit_logic.c -o $(NAME)

clean:
	rm -f simulation