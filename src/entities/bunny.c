#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "canvas.h"

typedef struct Pos {
    int x, y;
} Position;

typedef struct Bunny{
    int energy;
    int vision;
    Position pos;
    Position old_pos;
} Bunny;



static Bunny initialize_bunny(Bunny *b){
    int as = rand() % (100-1);
    int bs = rand() % (100-1);
    
    b->energy = 100;
    b->vision = 1;
    b->pos.x = as;
    b->pos.y = bs;
    b->old_pos = b->pos;

    return *b;
}

void initialize_bunnies(void *b_list, int size){
    if (b_list == NULL) return;

    Bunny *bunnies = (Bunny *)b_list;

    for (int i = 0; i < size; i++){
        initialize_bunny(&bunnies[i]);
    }
}

void *create_bunnies(int size) {
    if (size <= 0)
        return NULL;

    Bunny *bunnies = malloc(size * sizeof(Bunny));

    if (!bunnies)
        return NULL;

    initialize_bunnies(bunnies, size);

    return bunnies;
}

void destroy_bunnies(void *bunnies){
    free(bunnies);
}

static void process_bunny(Bunny *b) {
    int direction = rand() % 4;

    b->old_pos = b->pos;

    switch (direction) {
        case 0: b->pos.y++; break;
        case 1: b->pos.x++; break;
        case 2: b->pos.y--; break;
        case 3: b->pos.x--; break;
    }

}

void update_bunnies(void *b_list, int size){
    Bunny *bunnies = (Bunny *)b_list;
    
    for (int i = 0; i < size; i++) {
        process_bunny(&bunnies[i]);
    }
}

void draw_bunnies(void *b_list, int size) {
    Bunny *bunnies = b_list;

    for (int i = 0; i < size; i++) {
        // clean old position draw
        int old_x = bunnies[i].old_pos.x;
        int old_y = bunnies[i].old_pos.y;

        if (old_x >= 0 && old_x < W &&
            old_y >= 0 && old_y < H) {
            draw_pixel(old_x, old_y, BLACK);
        }

        int x = bunnies[i].pos.x;
        int y = bunnies[i].pos.y;

        if (x >= 0 && x < W && y >= 0 && y < H) {
            draw_pixel(x, y, WHITE);
        }
    }
}
