#include "entities/bunny/bunny.h"
#include "entities/bush/bush.h"

int game_loop(void *bushes, void *bunnies) {
  // GAME
  // print_bunnies(bunnies);
  print_bunny_count(bunnies);
  // LOOP: INPUT -> UPADTE -> RENDER
  // UPDATE BUSHES
  update_bushes(bushes);
  draw_bushes(bushes);
  // UPDATE BUNNIES
  update_bunnies(bunnies);
  draw_bunnies(bunnies);

  // update bunnies
}