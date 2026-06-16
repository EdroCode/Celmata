#include "types.h"
#include <stdlib.h>
#include <math.h>


/* Initializes a list of wolfs with default values */
void initWolfs(WolfList *list, int count, int mapWidth, int mapHeight) {
    list->pack  = malloc(count * sizeof(Wolf));
    list->count    = count;
    list->capacity = count;

    for (int i = 0; i < count; i++) {
        list->pack[i].max_energy = 30 + rand() % MAX_START_ENERGY;
        list->pack[i].energy     = list->pack[i].max_energy;
        list->pack[i].vision     = 1 + rand() % MAX_START_VISION;
        list->pack[i].speed      = 1 + rand() % MAX_START_SPEED;
        list->pack[i].x          = rand() % mapWidth;
        list->pack[i].y          = rand() % mapHeight;
        list->pack[i].cooldown   = 0;
        list->pack[i].birthed    = false;
        list->pack[i].generation = 0;
        list->pack[i].cur_state  = IDLE;
    }
}

void freeWolfs(WolfList *list) {
    free(list->pack);
    list->pack  = NULL;
    list->count    = 0;
    list->capacity = 0;
}