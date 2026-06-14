#include "types.h"
#include <stdlib.h>
#include <math.h>


static int clamp(int v, int min, int max) {
    if (v < min) return min;
    if (v > max) return max;
    return v;
}

/* Initializes a list of rabbits with default values */
void initRabbits(RabbitList *list, int count, int mapWidth, int mapHeight) {
    list->rabbits  = malloc(count * sizeof(Rabbit));
    list->count    = count;
    list->capacity = count;

    for (int i = 0; i < count; i++) {
        list->rabbits[i].max_energy = 5 + rand() % MAX_START_ENERGY;
        list->rabbits[i].energy     = list->rabbits[i].max_energy;
        list->rabbits[i].vision     = 1 + rand() % MAX_START_VISION;
        list->rabbits[i].speed      = 1 + rand() % MAX_START_SPEED;
        list->rabbits[i].x          = rand() % mapWidth;
        list->rabbits[i].y          = rand() % mapHeight;
        list->rabbits[i].cooldown   = 0;
        list->rabbits[i].birthed    = false;
        list->rabbits[i].generation = 0;
        list->rabbits[i].cur_state  = IDLE;
    }
}

/* Adds a new rabbit to the rabbit list*/
void add_rabbit(RabbitList *list, Rabbit rabbit) {
    if (list->count >= list->capacity) {
        int new_capacity = (list->capacity == 0) ? 1 : list->capacity * 2;
        Rabbit *new_arr  = realloc(list->rabbits, new_capacity * sizeof(Rabbit));
        if (!new_arr) return;
        list->rabbits   = new_arr;
        list->capacity  = new_capacity;
    }
    list->rabbits[list->count++] = rabbit;
}

void kill_rabbit(RabbitList *list, int i) {
    list->rabbits[i] = list->rabbits[--list->count];
}

void freeRabbits(RabbitList *list) {
    free(list->rabbits);
    list->rabbits  = NULL;
    list->count    = 0;
    list->capacity = 0;
}

/* Returns pointer to rabbit at (x,y), or NULL */
Rabbit *get_rabbit_at(RabbitList *list, int x, int y) {
    for (int i = 0; i < list->count; i++)
        if (list->rabbits[i].x == x && list->rabbits[i].y == y)
            return &list->rabbits[i];
    return NULL;
}

/* Returns pointer to a random neighbouring rabbit, or NULL */
Rabbit *get_near_rabbit(Map *m, RabbitList *list, Pos p) {
    Rabbit *near[8];
    int count = 0;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;
            int nx = p.x + dx, ny = p.y + dy;
            if (nx < 0 || ny < 0 || nx >= m->width || ny >= m->height) continue;
            Rabbit *r = get_rabbit_at(list, nx, ny);
            if (r) near[count++] = r;
        }
    }

    return count ? near[rand() % count] : NULL;
}

/* Returns a random neighbouring grass cell, or {-1,-1} if none */
Pos get_grass_near(Map *m, Pos p) {
    Pos cells[8];
    int count = 0;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;
            int nx = p.x + dx, ny = p.y + dy;
            if (nx < 0 || ny < 0 || nx >= m->width || ny >= m->height) continue;
            if (m->grid[ny][nx].grass) {
                cells[count].x = nx;
                cells[count].y = ny;
                count++;
            }
        }
    }

    if (count == 0) { Pos bad = {-1,-1}; return bad; }
    return cells[rand() % count];
}

/* Returns a random neighbouring cell (grass or not), or {-1,-1} if none */
Pos get_random_neighbour(Map *m, Pos p) {
    Pos cells[8];
    int count = 0;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;
            int nx = p.x + dx, ny = p.y + dy;
            if (nx < 0 || ny < 0 || nx >= m->width || ny >= m->height) continue;
            cells[count].x = nx;
            cells[count].y = ny;
            count++;
        }
    }

    if (count == 0) { Pos bad = {-1,-1}; return bad; }
    return cells[rand() % count];
}

static void try_reproduce(Map *m, RabbitList *list, Rabbit *r, GameState *state) {
    Pos p = {r->x, r->y};
    Rabbit *partner = get_near_rabbit(m, list, p);

    if (!partner) return;
    if (partner->cooldown > 0) return;
    if (partner->energy < partner->max_energy * 0.6f) return;

    Rabbit baby;
    baby.x          = r->x;
    baby.y          = r->y;
    baby.max_energy = (rand() % 2) ? r->max_energy : partner->max_energy;
    baby.energy     = baby.max_energy;
    baby.speed      = (rand() % 2) ? r->speed      : partner->speed;
    baby.vision     = (rand() % 2) ? r->vision     : partner->vision;

    if (rand() % 100 < MUTATION_CHANCE) baby.speed      += rand() % 3 - 1;
    if (rand() % 100 < MUTATION_CHANCE) baby.max_energy += rand() % 3 - 1;

    baby.speed      = clamp(baby.speed,      1, 5);
    baby.max_energy = clamp(baby.max_energy, 2, 120);

    baby.generation = state->max_generation + 1;
    baby.cooldown   = 10;
    baby.birthed    = true;
    baby.cur_state  = IDLE;

    r->energy       /= 2;
    partner->energy /= 2;
    r->cooldown      = 10;
    partner->cooldown = 10;

    add_rabbit(list, baby);   

    state->total_birthed++;
    if (baby.generation > state->max_generation)
        state->max_generation = baby.generation;

    r->energy = r->energy / 2;
}

static void move_to(Map *m, Rabbit *r, Pos target) {
    if (target.x == -1) return;
    r->x = target.x;
    r->y = target.y;
    if (m->grid[r->y][r->x].grass) {
        m->grid[r->y][r->x].grass = 0;
        r->energy += 5;

        if (r->energy > r->max_energy)
            r->energy = r->max_energy;
    }
}

void process_rabbit(Map *m, RabbitList *list, Rabbit *r, GameState *state) {
    int energy_pct = (r->energy * 100) / r->max_energy;
    Pos rp = {r->x, r->y};

    for (int step = 0; step < r->speed; step++) {

        if (energy_pct <= 30) {
            r->cur_state = SEEK;

            Pos target = get_grass_near(m, rp);

            if (target.x == -1 && target.y == -1) {
                target = get_random_neighbour(m, rp);
            }

            move_to(m, r, target);

        } else if (energy_pct >= 90) {
            if (rand() % 100 <= REPRODUCE_CHANCE && r->cooldown == 0) {
                r->cur_state = REPRODUCE;
                try_reproduce(m, list, r, state);
            }
            else {
                r->cur_state = MOVE;
                Pos target = get_grass_near(m, rp);
                if (target.x == -1) target = get_random_neighbour(m, rp);
                move_to(m, r, target);
            }
        }
        else {
            r->cur_state = MOVE;
            Pos target = get_grass_near(m, rp);
            if (target.x == -1) target = get_random_neighbour(m, rp);
            move_to(m, r, target);
        }
        
        r->energy -= 2;
        
        rp.x = r->x;
        rp.y = r->y;
    }
    
    if (r->energy > r->max_energy) r->energy = r->max_energy;
    if (r->energy <= 0) r->energy = 0;
    if (r->cooldown > 0) r->cooldown--;
    return;
}


void update_rabbits(Map *m, RabbitList *list, GameState *state) {
    for (int i = list->count - 1; i >= 0; i--) {
        if (list->rabbits[i].energy <= 0) {
            kill_rabbit(list, i);
            state->total_dead++;
            continue;
        }
        process_rabbit(m, list, &list->rabbits[i], state);
    }
}