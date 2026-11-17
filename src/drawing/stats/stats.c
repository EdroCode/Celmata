#include "stats.h"
#include "drawing/canvas.h"
#include "entities/bunny/bunny_internal.h"
#include "entities/wolf/wolf_internal.h"
#include <stdio.h>

static TTF_Font *font_header = NULL;
static TTF_Font *font_item = NULL;

#define FONT_PATH_PRIMARY                                                      \
  "/usr/share/fonts/TTF/JetBrainsMonoNerdFont-Regular.ttf"
#define FONT_PATH_FALLBACK "/usr/share/fonts/noto/NotoSans-Regular.ttf"

int init_stats_ui(void) {
  if (TTF_Init() == -1) {
    fprintf(stderr, "TTF_Init Error: %s\n", TTF_GetError());
    return -1;
  }

  const char *path = FONT_PATH_PRIMARY;
  font_header = TTF_OpenFont(path, 16);
  if (!font_header) {
    path = FONT_PATH_FALLBACK;
    font_header = TTF_OpenFont(path, 16);
  }

  if (font_header) {
    TTF_SetFontStyle(font_header, TTF_STYLE_BOLD);
    font_item = TTF_OpenFont(path, 14);
  }

  return (font_header && font_item) ? 0 : -1;
}

void cleanup_stats_ui(void) {
  if (font_header) {
    TTF_CloseFont(font_header);
    font_header = NULL;
  }
  if (font_item) {
    TTF_CloseFont(font_item);
    font_item = NULL;
  }
  TTF_Quit();
}

static void draw_text(SDL_Renderer *ren, TTF_Font *font, const char *text,
                      int x, int y, SDL_Color color) {
  if (!font || !text || text[0] == '\0')
    return;

  SDL_Surface *surf = TTF_RenderUTF8_Blended(font, text, color);
  if (!surf)
    return;

  SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
  if (tex) {
    SDL_Rect dst = {x, y, surf->w, surf->h};
    SDL_RenderCopy(ren, tex, NULL, &dst);
    SDL_DestroyTexture(tex);
  }
  SDL_FreeSurface(surf);
}

void render_stats_ui(SDL_Renderer *renderer, int canvas_screen_width,
                     int window_height, int tick, const BunnyList *bunnies,
                     const BushList *bushes, const WolfList *wolves) {
  int start_x = canvas_screen_width;
  int panel_w = SIDEBAR_WIDTH;

  // Sidebar background
  SDL_Rect panel_rect = {start_x, 0, panel_w, window_height};
  SDL_SetRenderDrawColor(renderer, 24, 26, 33, 255);
  SDL_RenderFillRect(renderer, &panel_rect);

  // Vertical border line
  SDL_SetRenderDrawColor(renderer, 42, 46, 56, 255);
  SDL_RenderDrawLine(renderer, start_x, 0, start_x, window_height);

  // Count active entities
  int alive_bunnies = 0;
  if (bunnies) {
    for (int i = 0; i < bunnies->bunny_count; i++) {
      if (bunnies->bunnies[i].alive)
        alive_bunnies++;
    }
  }

  int alive_bushes = 0;
  if (bushes) {
    for (int i = 0; i < bushes->bush_count; i++) {
      if (bushes->bushes[i].food > 0)
        alive_bushes++;
    }
  }

  int alive_wolves = 0;
  if (wolves) {
    for (int i = 0; i < wolves->wolf_count; i++) {
      if (wolves->wolf_pack[i].alive)
        alive_wolves++;
    }
  }

  int x = start_x + 16;
  int y = 24;
  char buf[64];

  // Header
  SDL_Color col_title = {230, 235, 245, 255};
  draw_text(renderer, font_header, "Celmata", x, y, col_title);
  y += 28;

  // Tick
  SDL_Color col_sub = {130, 140, 155, 255};
  snprintf(buf, sizeof(buf), "Tick: %d", tick);
  draw_text(renderer, font_item, buf, x, y, col_sub);
  y += 32;

  // Divider
  SDL_SetRenderDrawColor(renderer, 40, 44, 54, 255);
  SDL_RenderDrawLine(renderer, x, y, start_x + panel_w - 16, y);
  y += 20;

  // Bunnies
  SDL_Rect dot_bunny = {x, y + 4, 8, 8};
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(renderer, &dot_bunny);
  snprintf(buf, sizeof(buf), "Bunnies: %d", alive_bunnies);
  draw_text(renderer, font_item, buf, x + 16, y,
            (SDL_Color){240, 240, 240, 255});
  y += 28;

  // Bushes
  SDL_Rect dot_bush = {x, y + 4, 8, 8};
  SDL_SetRenderDrawColor(renderer, 168, 230, 161, 255);
  SDL_RenderFillRect(renderer, &dot_bush);
  snprintf(buf, sizeof(buf), "Bushes: %d", alive_bushes);
  draw_text(renderer, font_item, buf, x + 16, y,
            (SDL_Color){168, 230, 161, 255});
  y += 28;

  // Wolves
  SDL_Rect dot_wolf = {x, y + 4, 8, 8};
  SDL_SetRenderDrawColor(renderer, 255, 107, 107, 255);
  SDL_RenderFillRect(renderer, &dot_wolf);
  snprintf(buf, sizeof(buf), "Wolves: %d", alive_wolves);
  draw_text(renderer, font_item, buf, x + 16, y,
            (SDL_Color){255, 120, 120, 255});
}
