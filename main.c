#include "src/drawing/canvas.h"
#include "src/entities/bunny.h"
#include "src/game_loop.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *win =
      SDL_CreateWindow("Simple", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       W * SCALE, H * SCALE, 0);
  SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC);

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"); // sharp pixels, no blur
  SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888,
                                       SDL_TEXTUREACCESS_STREAMING, W, H);

  srand((unsigned int)time(NULL));

  int bunny_start_count = 10;
  void *b_list = create_bunnies(bunny_start_count);

  float game_speed = 0.1;

  int running = 1, loops = 0;

  while (running) {
    SDL_Event e;
    while (SDL_PollEvent(&e))
      if (e.type == SDL_QUIT)
        running = 0;

    game_loop(b_list);

    SDL_UpdateTexture(tex, NULL, canvas_pixels(), W * 4);
    SDL_RenderCopy(ren, tex, NULL, NULL);
    SDL_RenderPresent(ren);

    loops++;
    clear_canvas();
    SDL_Delay(game_speed * 1000);
  }

  printf("Stopped at tick %d\n", loops);
  print_summary(b_list);
  destroy_bunnies(b_list);

  SDL_DestroyTexture(tex);
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return 0;
}