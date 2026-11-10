#include "entities/bunny/bunny.h"

int game_loop(void *bunnies) {
  // GAME
  // print_bunnies(bunnies);
  print_bunny_count(bunnies);
  // LOOP: INPUT -> UPADTE -> RENDER
  // UPDATE
  update_bunnies(bunnies);
  draw_bunnies(bunnies);

  // update bunnies
}