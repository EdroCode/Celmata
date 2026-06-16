#include <stdbool.h>
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "types.h"

void main_loop(Map m, RabbitList *rabbits, WolfList *wolfs, GameState *state) {

    FILE *log_file = fopen("celmata_log.csv", "w");
    fprintf(log_file, "tick,population,grass_count,avg_energy,avg_speed,avg_max_energy,max_generation,total_born,total_dead\n");

    struct timespec ts;

    ts.tv_sec = GAME_SPEED / 1000;
    ts.tv_nsec = (GAME_SPEED % 1000) * 1000000L;

    while (1) {
        srand(time(NULL));

        clear_screen();

        update_map(&m);

        update_rabbits(&m, rabbits, state);

        draw_map(m, rabbits, wolfs, state);

        state->ticks++;

        int pop = rabbits->count;
        int grass_count = 0;
        float avg_energy = 0, avg_speed = 0, avg_max_energy = 0;

        for (int i = 0; i < pop; i++) {
            avg_energy     += rabbits->rabbits[i].energy;
            avg_speed      += rabbits->rabbits[i].speed;
            avg_max_energy += rabbits->rabbits[i].max_energy;
        }
        if (pop > 0) {
            avg_energy     /= pop;
            avg_speed      /= pop;
            avg_max_energy /= pop;
        }
        for (int y = 0; y < state->map.height; y++)
            for (int x = 0; x < state->map.width; x++)
                if (state->map.grid[y][x].grass) grass_count++;

        fprintf(log_file,
            "%d,%d,%d,%.2f,%.2f,%.2f,%d,%d,%d\n",
            state->ticks, pop, grass_count,
            avg_energy, avg_speed, avg_max_energy,
            state->max_generation, state->total_birthed, state->total_dead
        );
        
        if (rabbits->count <= 0) {
            print_stats(state,rabbits);
            return;
        }
        
        fflush(log_file); 
        nanosleep(&ts, NULL);

        
    }

    fclose(log_file);
}