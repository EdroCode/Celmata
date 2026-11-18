#include "entities/bunny/bunny.h"
#include "entities/bush/bush.h"
#include "entities/wolf/wolf.h"

int game_loop(void *bushes, void *bunnies, void *wolf_pack) {
  // GAME
  // print_bunnies(bunnies);
  print_bunny_csv(bunnies);
  // LOOP: INPUT -> UPADTE -> RENDER
  // UPDATE BUSHES
  update_bushes(bushes);
  draw_bushes(bushes);
  // UPDATE BUNNIES
  update_bunnies(bunnies);
  draw_bunnies(bunnies);
  // UPDATE WOLFS
  update_wolfs(wolf_pack);
  draw_wolfs(wolf_pack);
  return 0;
}