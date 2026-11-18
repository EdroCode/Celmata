#ifndef STATS_H
#define STATS_H

#include "entities/bunny/bunny.h"
#include "entities/bush/bush.h"
#include "entities/wolf/wolf.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#define SIDEBAR_WIDTH 180

int init_stats_ui(void);
void cleanup_stats_ui(void);

void render_stats_ui(SDL_Renderer *renderer, int canvas_screen_width,
                     int window_height, int tick, const BunnyList *bunnies,
                     const BushList *bushes, const WolfList *wolves);

#endif
