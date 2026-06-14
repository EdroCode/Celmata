#include "types.h"
#include "types.h"
#include <stdlib.h>

static Cell* get_neighbors(Map *m, int x, int y) {

    Cell *neighbors = malloc(8 * sizeof(Cell));
    int count = 0;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {

            if (dx == 0 && dy == 0)
                continue;

            int nx = x + dx;
            int ny = y + dy;

            if (nx >= 0 && nx < m->width &&
                ny >= 0 && ny < m->height) {

                neighbors[count++] = m->grid[ny][nx];
            }
        }
    }

    return neighbors;
}

void initMap(Map *map, int width, int height) {
    map->width = width;
    map->height = height;

    int num_clusters = width*height / 
    int initial_clusters[
    
    map->grid = malloc(height * sizeof(Cell *));

    for (int y = 0; y < height; y++) {
        map->grid[y] = malloc(width * sizeof(Cell));

        for (int x = 0; x < width; x++) {
            map->grid[y][x].x = x;
            map->grid[y][x].y = y;
            if (rand() % 100 < START_GRASS_CHANCE) {
                map->grid[y][x].grass = true;
                map->grid[y][x].decay = GRASS_DECAY + rand()%10;
            }
            
        }
    }
}

void update_map(Map *m) {
    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {

            Cell *cell = &m->grid[y][x];

            if (!cell->grass) {

                int count = 0;
                Cell *cells = get_neighbors(m, x, y);

                for (int k = 0; k < 8; k++) {
                    if (cells[k].grass)
                        count++;
                }

                free(cells);

                int chance = GRASS_CHANCE + count * GRASS_NEIGHBOOR_CHANCE_MULTIPLIER;
                if (chance > 100) chance = 100;

                if (rand() % 100 < chance && cell->cooldown <= 0) {
                    cell->grass = true;
                    cell->decay = GRASS_DECAY + rand() % 10;
                    cell->cooldown = GRASS_COOLDOWN;
                }

            } else {
                cell->cooldown--;
                if (--cell->decay <= 0) {
                    cell->grass = false;
                }
            }
        }
    }
}

void freeMap(Map *map) {
    for (int y = 0; y < map->height; y++) {
        free(map->grid[y]);
    }
    free(map->grid);
}

