#include "types.h"
#include <stdlib.h>
#include <math.h>

static int is_close(Rabbit *a, Rabbit *b) {
    return abs(a->x - b->x) <= 1 && abs(a->y - b->y) <= 1;
}

static int clamp(int v, int min, int max) {
    if (v < min) return min;
    if (v > max) return max;
    return v;
}

void initRabbits(RabbitList *list, int count, int mapWidth, int mapHeight) {
    list->rabbits = malloc(count * sizeof(Rabbit));
    list->count = count;
    list->capacity = count;

    for (int i = 0; i < count; i++) {
        list->rabbits[i].max_energy = 5 + rand() % MAX_START_ENERGY;
        list->rabbits[i].energy = list->rabbits[i].max_energy;

        list->rabbits[i].vision = 1 + rand() % MAX_START_VISION;
        list->rabbits[i].speed = 1 + rand() % MAX_START_SPEED;

        list->rabbits[i].x = rand() % mapWidth;
        list->rabbits[i].y = rand() % mapHeight;

        list->rabbits[i].cooldown = 0;
        list->rabbits[i].birthed = false;

        list->rabbits[i].generation = 0;
        
    }
}

void add_rabbit(RabbitList *list, Rabbit rabbit) {
    if (list->count >= list->capacity) {
        int new_capacity = (list->capacity == 0) ? 1 : list->capacity * 2;

        Rabbit *new_arr = realloc(list->rabbits, new_capacity * sizeof(Rabbit));
        if (!new_arr) return;

        list->rabbits = new_arr;
        list->capacity = new_capacity;
    }

    list->rabbits[list->count++] = rabbit;
}

void kill_rabbit(RabbitList *list, int i) {
    for (int k = i; k < list->count - 1; k++) {
        list->rabbits[k] = list->rabbits[k + 1];
    }
    list->count--;
}

void update_rabbits(Map *m, RabbitList *list, GameState *state) {


    for (int i = 0; i < list->count; i++) {

        Rabbit *r = &list->rabbits[i];

        r->birthed = false;

        if (r->cooldown > 0)
            r->cooldown--;

        for (int j = 0; j < r->speed; j++) {

            if (r->energy <= 0)
                break;

            if (rand() % 100 < TIRENESS)
                r->energy--;

            if (r->energy <= 0)
                break;

            int x = r->x;
            int y = r->y;

            if (x >= 0 && x < m->width && y >= 0 && y < m->height) {

                Cell *cell = &m->grid[y][x];

                if (cell->grass) {
                    r->energy++;
                    if (r->energy > r->max_energy)
                        r->energy = r->max_energy;

                    cell->grass = false;
                }


                if (j == 0) {
                    r->dir = rand() % 4;
                }

                switch (r->dir) {
                    case 0: r->y++; break;
                    case 1: r->y--; break;
                    case 2: r->x++; break;
                    case 3: r->x--; break;
                }

                if (r->x < 0) r->x = 0;
                if (r->x >= m->width) r->x = m->width - 1;
                if (r->y < 0) r->y = 0;
                if (r->y >= m->height) r->y = m->height - 1;
            }
        }
    }


    for (int i = 0; i < list->count; i++) {
        for (int j = i + 1; j < list->count; j++) {

            Rabbit *a = &list->rabbits[i];
            Rabbit *b = &list->rabbits[j];

            if (a->cooldown > 0 || b->cooldown > 0)
                continue;

            if (a->energy <= a->max_energy * 0.6)
                continue;

            if (b->energy <= b->max_energy * 0.6)
                continue;

            if (!is_close(a, b))
                continue;

            int avg_energy = (a->energy + b->energy) / 2;
            int chance = REPRODUCE_CHANCE * avg_energy / a->max_energy;

            if (rand() % 100 < chance) {

                Rabbit baby;

                baby.x = a->x;
                baby.y = a->y;

                /* inheritance */
                baby.max_energy = (rand() % 2) ? a->max_energy : b->max_energy;
                baby.energy = baby.max_energy;

                baby.speed  = (rand() % 2) ? a->speed  : b->speed;
                baby.vision = 1; // still unused 

                if (rand() % 100 < MUTATION_CHANCE)
                    baby.speed += (rand() % 3 - 1);

                if (rand() % 100 < MUTATION_CHANCE)
                    baby.max_energy += (rand() % 3 - 1);

                baby.speed = clamp(baby.speed, 1, 5);
                baby.max_energy = clamp(baby.max_energy, 2, 120);

                baby.generation =
                    (a->generation > b->generation ? a->generation : b->generation) + 1;

                baby.cooldown = 10;
                baby.birthed = true;

                a->energy /= 2;
                b->energy /= 2;

                a->cooldown = 10;
                b->cooldown = 10;

                add_rabbit(list, baby);

                state->total_birthed++;
            }
        }
    }

    for (int i = 0; i < list->count; ) {
        if (list->rabbits[i].energy <= 0) {
            kill_rabbit(list, i);
        } else {
            i++;
        }
    }
}

void freeRabbits(RabbitList *list) {
    free(list->rabbits);
    list->rabbits = NULL;
    list->count = 0;
    list->capacity = 0;
}