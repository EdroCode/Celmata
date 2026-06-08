#include <stdbool.h>
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "types.h"

void main_loop(Map m, RabbitList *rabbits, GameState *state) {

    struct timespec ts;


    ts.tv_sec = GAME_SPEED / 1000;
    ts.tv_nsec = (GAME_SPEED % 1000) * 1000000L;

    while (1) {
        srand(time(NULL));

        clear_screen();

        update_map(&m);

        update_rabbits(&m, rabbits, state);

        draw_map(m, rabbits, state);

        state->ticks++;

//        if (rabbits->count <= 0) return;
        nanosleep(&ts, NULL);
    }
}