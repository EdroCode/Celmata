#include "src/bunny.h"

int game_loop(void *bunnies) {
  // GAME
  print_bunnies(bunnies);

  // LOOP: INPUT -> UPADTE -> RENDER
  // UPDATE
  update_bunnies(bunnies);
  draw_bunnies(bunnies);

  // update bunnies
}